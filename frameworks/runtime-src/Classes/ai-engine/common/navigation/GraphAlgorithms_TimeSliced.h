#ifndef TIME_SLICED_GRAPHALGORITHMS_H
#define TIME_SLICED_GRAPHALGORITHMS_H
#pragma warning (disable:4786)
//------------------------------------------------------------------------
//
//  Name: GraphAlgorithms_TimeSliced.h
//
//  Desc: classes to implement graph algorithms that can be distributed
//          over multiple update-steps
//
//          Any graphs passed to these functions must conform to the
//          same interface used by the SparseGraph
//          
//
//------------------------------------------------------------------------
#include <vector>
#include <list>
#include <queue>
#include <stack>

#include "common/misc/PriorityQueue.h"
#include "common/navigation/SearchTerminationPolicies.h"
#include "common/navigation/PathEdge.h"



//these enums are used as return values from each search update method
enum 
{
    target_found,
    target_not_found, 
    search_incomplete
};



//------------------------ Graph_SearchTimeSliced -----------------------------
//
// base class to define a common interface for graph search algorithms
//-----------------------------------------------------------------------------
template <class edge_type>
class Graph_SearchTimeSliced
{
public:
    enum SearchType{AStar, Dijkstra};

    Graph_SearchTimeSliced(SearchType type):m_SearchType(type){}

    virtual ~Graph_SearchTimeSliced(){}

    //When called, this method runs the algorithm through one search cycle. The
    //method returns an enumerated value (target_found, target_not_found,
    //search_incomplete) indicating the status of the search
    virtual int cycleOnce()=0;

    //returns the vector of edges that the algorithm has examined
    virtual std::vector<const edge_type*> getSPT()const=0;

    //returns the total cost to the target
    virtual float getCostToTarget()const=0;

    //returns a list of node indexes that comprise the shortest path
    //from the source to the target
    virtual std::list<int> getPathToTarget()const=0;

    //returns the path as a list of PathEdges
    virtual std::list<PathEdge> getPathAsPathEdges()const=0;

    SearchType getType()const{return m_SearchType;}

private:
    SearchType m_SearchType;  
};




//-------------------------- Graph_SearchAStar_TS -----------------------------
//
//  a A* class that enables a search to be completed over multiple update-steps
//-----------------------------------------------------------------------------
template <class graph_type, class heuristic>
class Graph_SearchAStar_TS : public Graph_SearchTimeSliced<typename graph_type::EdgeType>
{
private:
    //create typedefs for the node and edge types used by the graph
    typedef typename graph_type::EdgeType Edge;
    typedef typename graph_type::NodeType Node;

private:
    const graph_type& m_Graph;

    //indexed into my node. Contains the 'real' accumulative cost to that node
    std::vector<float> m_GCosts; 

    //indexed into by node. Contains the cost from adding m_GCosts[n] to
    //the heuristic cost from n to the target node. This is the vector the
    //iPQ indexes into.
    std::vector<float> m_FCosts;

    std::vector<const Edge*> m_ShortestPathTree;
    std::vector<const Edge*> m_SearchFrontier;

    int m_iSource;
    int m_iTarget;

    //create an indexed priority queue of nodes. The nodes with the
    //lowest overall F cost (G+H) are positioned at the front.
    indexedPriorityQLow<float>* m_pPQ;

public:
    Graph_SearchAStar_TS(const graph_type& G,
                                                    int source,
                                                    int target):Graph_SearchTimeSliced<Edge>(AStar),
                                                    m_Graph(G),
                                                    m_ShortestPathTree(G.NumNodes()),                              
                                                    m_SearchFrontier(G.NumNodes()),
                                                    m_GCosts(G.NumNodes(), 0.0),
                                                    m_FCosts(G.NumNodes(), 0.0),
                                                    m_iSource(source),
                                                    m_iTarget(target)
    { 
        //create the PQ   
        m_pPQ =new indexedPriorityQLow<float>(m_FCosts, m_Graph.NumNodes());

        //put the source node on the queue
        m_pPQ->insert(m_iSource);
    }

    ~Graph_SearchAStar_TS(){delete m_pPQ;}

    //When called, this method pops the next node off the PQ and examines all
    //its edges. The method returns an enumerated value (target_found,
    //target_not_found, search_incomplete) indicating the status of the search
    int cycleOnce();

    //returns the vector of edges that the algorithm has examined
    std::vector<const Edge*> getSPT()const{return m_ShortestPathTree;}

    //returns a vector of node indexes that comprise the shortest path
    //from the source to the target
    std::list<int> getPathToTarget()const;

    //returns the path as a list of PathEdges
    std::list<PathEdge> getPathAsPathEdges()const;

    //returns the total cost to the target
    float getCostToTarget()const{return m_GCosts[m_iTarget];}
};

//-----------------------------------------------------------------------------
template <class graph_type, class heuristic>
int Graph_SearchAStar_TS<graph_type, heuristic>::cycleOnce()
{
    //if the PQ is empty the target has not been found
    if (m_pPQ->empty())
    {
        return target_not_found;
    }

    //get lowest cost node from the queue
    int NextClosestNode = m_pPQ->pop();

    //put the node on the SPT
    m_ShortestPathTree[NextClosestNode] = m_SearchFrontier[NextClosestNode];

    //if the target has been found exit
    if (NextClosestNode == m_iTarget)
    {
        return target_found;
    }

    //now to test all the edges attached to this node
    graph_type::ConstEdgeIterator ConstEdgeItr(m_Graph, NextClosestNode);
    for (const Edge* pE=ConstEdgeItr.begin();!ConstEdgeItr.end();pE=ConstEdgeItr.next())
    {
        //calculate the heuristic cost from this node to the target (H)                       
        float HCost = heuristic::calculate(m_Graph, m_iTarget, pE->to()); 

        //calculate the 'real' cost to this node from the source (G)
        float GCost = m_GCosts[NextClosestNode] + pE->cost();

        //if the node has not been added to the frontier, add it and update
        //the G and F costs
        if (m_SearchFrontier[pE->to()] == NULL)
        {
            m_FCosts[pE->to()] = GCost + HCost;
            m_GCosts[pE->to()] = GCost;

            m_pPQ->insert(pE->to());

            m_SearchFrontier[pE->to()] = pE;
        }

        //if this node is already on the frontier but the cost to get here
        //is cheaper than has been found previously, update the node
        //costs and frontier accordingly.
        else if ((GCost < m_GCosts[pE->to()]) && (m_ShortestPathTree[pE->to()]==NULL))
        {
            m_FCosts[pE->to()] = GCost + HCost;
            m_GCosts[pE->to()] = GCost;

            m_pPQ->ChangePriority(pE->to());

            m_SearchFrontier[pE->to()] = pE;
        }
    }
  
    //there are still nodes to explore
    return search_incomplete;
}

//-----------------------------------------------------------------------------
template <class graph_type, class heuristic>
std::list<int> Graph_SearchAStar_TS<graph_type, heuristic>::getPathToTarget()const
{
    std::list<int> path;

    //just return an empty path if no target or no path found
    if (m_iTarget < 0)  return path;    

    int nd = m_iTarget;

    path.push_back(nd);
    
    while ((nd != m_iSource) && (m_ShortestPathTree[nd] != 0))
    {
        nd = m_ShortestPathTree[nd]->from();

        path.push_front(nd);
    }

    return path;
} 


//-------------------------- getPathAsPathEdges -------------------------------
//
//  returns the path as a list of PathEdges
//-----------------------------------------------------------------------------
template <class graph_type, class heuristic>
std::list<PathEdge> Graph_SearchAStar_TS<graph_type, heuristic>::getPathAsPathEdges()const
{
    std::list<PathEdge> path;

    //just return an empty path if no target or no path found
    if (m_iTarget < 0)  return path;    

    int nd = m_iTarget;
    
    while ((nd != m_iSource) && (m_ShortestPathTree[nd] != 0))
    {
        path.push_front(PathEdge(  m_Graph.GetNode(m_ShortestPathTree[nd]->from()).pos(),
                                                 m_Graph.GetNode(m_ShortestPathTree[nd]->to()).pos(),
                                                 m_ShortestPathTree[nd]->flags(),
                                                 m_ShortestPathTree[nd]->getIntersectingEntityID()));

        nd = m_ShortestPathTree[nd]->from();
    }

    return path;
}

//-------------------------- Graph_SearchDijkstras_TS -------------------------
//
//  Dijkstra's algorithm class modified to spread a search over multiple
//  update-steps
//-----------------------------------------------------------------------------
template <class graph_type, class termination_condition>
class Graph_SearchDijkstras_TS : public Graph_SearchTimeSliced<typename graph_type::EdgeType>
{
private:
    //create typedefs for the node and edge types used by the graph
    typedef typename graph_type::EdgeType Edge;
    typedef typename graph_type::NodeType Node;

private:

    const graph_type& m_Graph;

    //indexed into my node. Contains the accumulative cost to that node
    std::vector<float> m_CostToThisNode; 

    std::vector<const Edge*> m_ShortestPathTree;
    std::vector<const Edge*> m_SearchFrontier;

    int m_iSource;
    int m_iTarget;

    //create an indexed priority queue of nodes. The nodes with the
    //lowest overall F cost (G+H) are positioned at the front.
    indexedPriorityQLow<float>* m_pPQ;

 

public:
    Graph_SearchDijkstras_TS(const graph_type&  G,
                                                        int source,
                                                        int target):Graph_SearchTimeSliced<Edge>(Dijkstra),
                                                        m_Graph(G),
                                                        m_ShortestPathTree(G.NumNodes()),                              
                                                        m_SearchFrontier(G.NumNodes()),
                                                        m_CostToThisNode(G.NumNodes(), 0.0),
                                                        m_iSource(source),
                                                        m_iTarget(target)
    { 
        //create the PQ         ,
        m_pPQ =new indexedPriorityQLow<float>(m_CostToThisNode, m_Graph.NumNodes());

        //put the source node on the queue
        m_pPQ->insert(m_iSource);
    }

    //let the search class take care of tidying up memory (the wary amongst
    //you may prefer to use std::auto_ptr or similar to replace the pointer
    //to the termination condition)
    ~Graph_SearchDijkstras_TS(){delete m_pPQ;}

    //When called, this method pops the next node off the PQ and examines all
    //its edges. The method returns an enumerated value (target_found,
    //target_not_found, search_incomplete) indicating the status of the search
    int cycleOnce();

    //returns the vector of edges that the algorithm has examined
    std::vector<const Edge*> getSPT()const{return m_ShortestPathTree;}

    //returns a vector of node indexes that comprise the shortest path
    //from the source to the target
    std::list<int> getPathToTarget()const;

    //returns the path as a list of PathEdges
    std::list<PathEdge> getPathAsPathEdges()const;

    //returns the total cost to the target
    float getCostToTarget()const{return m_CostToThisNode[m_iTarget];}
};

//-----------------------------------------------------------------------------
template <class graph_type, class termination_condition>
int Graph_SearchDijkstras_TS<graph_type, termination_condition>::cycleOnce()
{
    //if the PQ is empty the target has not been found
    if (m_pPQ->empty())
    {
        return target_not_found;
    }

    //get lowest cost node from the queue
    int NextClosestNode = m_pPQ->pop();

    //move this node from the frontier to the spanning tree
    m_ShortestPathTree[NextClosestNode] = m_SearchFrontier[NextClosestNode];

    //if the target has been found exit
    if (termination_condition::isSatisfied(m_Graph, m_iTarget, NextClosestNode))
    {
        //make a note of the node index that has satisfied the condition. This
        //is so we can work backwards from the index to extract the path from
        //the shortest path tree.
        m_iTarget = NextClosestNode;

        return target_found;
    }

    //now to test all the edges attached to this node
    graph_type::ConstEdgeIterator ConstEdgeItr(m_Graph, NextClosestNode);
    for (const Edge* pE=ConstEdgeItr.begin();!ConstEdgeItr.end();pE=ConstEdgeItr.next())
    {
        //the total cost to the node this edge points to is the cost to the
        //current node plus the cost of the edge connecting them.
        float NewCost = m_CostToThisNode[NextClosestNode] + pE->cost();

        //if this edge has never been on the frontier make a note of the cost
        //to get to the node it points to, then add the edge to the frontier
        //and the destination node to the PQ.
        if (m_SearchFrontier[pE->to()] == 0)
        {
            m_CostToThisNode[pE->to()] = NewCost;

            m_pPQ->insert(pE->to());

            m_SearchFrontier[pE->to()] = pE;
        }

        //else test to see if the cost to reach the destination node via the
        //current node is cheaper than the cheapest cost found so far. If
        //this path is cheaper, we assign the new cost to the destination
        //node, update its entry in the PQ to reflect the change and add the
        //edge to the frontier
        else if ( (NewCost < m_CostToThisNode[pE->to()]) &&
        (m_ShortestPathTree[pE->to()] == 0) )
        {
            m_CostToThisNode[pE->to()] = NewCost;

            //because the cost is less than it was previously, the PQ must be
            //re-sorted to account for this.
            m_pPQ->ChangePriority(pE->to());

            m_SearchFrontier[pE->to()] = pE;
        }
    }
  
    //there are still nodes to explore
    return search_incomplete;
}

//-----------------------------------------------------------------------------
template <class graph_type, class termination_condition>
std::list<int> Graph_SearchDijkstras_TS<graph_type, termination_condition>::getPathToTarget()const
{
    std::list<int> path;

    //just return an empty path if no target or no path found
    if (m_iTarget < 0)  return path;    

    int nd = m_iTarget;

    path.push_back(nd);

    while ((nd != m_iSource) && (m_ShortestPathTree[nd] != 0))
    {
        nd = m_ShortestPathTree[nd]->from();

        path.push_front(nd);
    }

    return path;
} 


//-------------------------- getPathAsPathEdges -------------------------------
//
//  returns the path as a list of PathEdges
//-----------------------------------------------------------------------------
template <class graph_type, class termination_condition>
std::list<PathEdge> Graph_SearchDijkstras_TS<graph_type, termination_condition>::getPathAsPathEdges()const
{
    std::list<PathEdge> path;

    //just return an empty path if no target or no path found
    if (m_iTarget < 0)  return path;    

    int nd = m_iTarget;

    while ((nd != m_iSource) && (m_ShortestPathTree[nd] != 0))
    {
        path.push_front(PathEdge( m_Graph.GetNode(m_ShortestPathTree[nd]->from()).pos(),
                                                 m_Graph.GetNode(m_ShortestPathTree[nd]->to()).pos(),
                                                 m_ShortestPathTree[nd]->flags(),
                                                 m_ShortestPathTree[nd]->getIntersectingEntityID()));

        nd = m_ShortestPathTree[nd]->from();
    }

    return path;
}

#endif