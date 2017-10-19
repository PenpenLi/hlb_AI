#ifndef SPARSEGRAPH_H
#define SPARSEGRAPH_H
#pragma warning (disable:4786)
//------------------------------------------------------------------------
//
//  Name:   SparseGraph.h
//
//  Desc:   Graph class using the adjacency list representation.
//
//
//------------------------------------------------------------------------
#include <vector>
#include <list>
#include <cassert>
#include <string>

#include "common/2D/Vector2D.h"
#include "common/misc/UtilsEx.h" 



template <class node_type, class edge_type>   
class SparseGraph                                 
{
public:
    //enable easy client access to the edge and node types used in the graph
    typedef edge_type EdgeType;
    typedef node_type NodeType;

    //a couple more typedefs to save my fingers and to help with the formatting
    //of the code on the printed page
    typedef std::vector<node_type>  NodeVector;
    typedef std::list<edge_type> EdgeList;
    typedef std::vector<EdgeList> EdgeListVector;

private:
    //the nodes that comprise this graph
    NodeVector m_Nodes;

    //a vector of adjacency edge lists. (each node index keys into the 
    //list of edges associated with that node)
    EdgeListVector m_Edges;

    //is this a directed graph?
    bool m_bDigraph;

    //the index of the next node to be added
    int m_iNextNodeIndex;
  
    //returns true if an edge is not already present in the graph. Used
    //when adding edges to make sure no duplicates are created.
    bool isUniqueEdge(int from, int to)const;

    //iterates through all the edges in the graph and removes any that point
    //to an invalidated node
    void cullInvalidEdges();

  
public:
    //ctor
    SparseGraph(bool digraph): m_iNextNodeIndex(0), m_bDigraph(digraph){}

    //returns the node at the given index
    const NodeType& getNode(int idx)const;

    //non const version
    NodeType& getNode(int idx);

    //const method for obtaining a reference to an edge
    const EdgeType& getEdge(int from, int to)const;

    //non const version
    EdgeType& getEdge(int from, int to);
    
    //retrieves the next free node index
    int getNextFreeNodeIndex()const{return m_iNextNodeIndex;}
  
    //adds a node to the graph and returns its index
    int addNode(node_type node);

    //removes a node by setting its index to -1
    void removeNode(int node);

    //Use this to add an edge to the graph. The method will ensure that the
    //edge passed as a parameter is valid before adding it to the graph. If the
    //graph is a digraph then a similar edge connecting the nodes in the opposite
    //direction will be automatically added.
    void addEdge(EdgeType edge);

    //removes the edge connecting from and to from the graph (if present). If
    //a digraph then the edge connecting the nodes in the opposite direction 
    //will also be removed.
    void removeEdge(int from, int to);

  //sets the cost of an edge
  void setEdgeCost(int from, int to, float cost);

    //returns the number of active + inactive nodes present in the graph
    int getNumNodes()const{return m_Nodes.size();}
  
    //returns the number of active nodes present in the graph (this method's
    //performance can be improved greatly by caching the value)
    int getNumActiveNodes()const
    {
        int count = 0;

        for (unsigned int n=0; n<m_Nodes.size(); ++n) if (m_Nodes[n].Index() != -1) ++count;

        return count;
    }

    //returns the total number of edges present in the graph
    int getNumEdges()const
    {
        int tot = 0;

        for (EdgeListVector::const_iterator curEdge = m_Edges.begin(); curEdge != m_Edges.end(); ++curEdge)
        {
            tot += curEdge->size();
        }

        return tot;
    }

    //returns true if the graph is directed
    bool isDigraph()const{return m_bDigraph;}

    //returns true if the graph contains no nodes
    bool isEmpty()const{return m_Nodes.empty();}

    //returns true if a node with the given index is present in the graph
    bool isNodePresent(int nd)const;

    //returns true if an edge connecting the nodes 'to' and 'from'
    //is present in the graph
    bool isEdgePresent(int from, int to)const;

    //methods for loading and saving graphs from an open file stream or from
    //a file name 
    bool save(const char* FileName)const;
    bool save(std::ofstream& stream)const;
  
    bool load(const char* FileName);
    bool load(std::ifstream& stream);

    //clears the graph ready for new node insertions
    void clear()
    {
        m_iNextNodeIndex = 0; 
        m_Nodes.clear(); 
        m_Edges.clear();
    }

    void removeEdges()
    {
        for (EdgeListVector::iterator it = m_Edges.begin(); it != m_Edges.end(); ++it)
        {
            it->clear();
        }
    }

  
    //non const class used to iterate through all the edges connected to a specific node. 
    class EdgeIterator
    {
    private: 
        typename EdgeList::iterator curEdge;

        SparseGraph<node_type, edge_type>& G;

        const int NodeIndex;


      public:
        EdgeIterator(SparseGraph<node_type, edge_type>& graph,int node): G(graph), NodeIndex(node)
        {
            /* we don't need to check for an invalid node index since if the node is
            invalid there will be no associated edges
            */

            curEdge = G.m_Edges[NodeIndex].begin();
        }

        EdgeType* begin()
        {        
            curEdge = G.m_Edges[NodeIndex].begin();

            return &(*curEdge);
        }

        EdgeType* next()
        {
            ++curEdge;

            if (end()) return NULL;

            return &(*curEdge);
        }

        //return true if we are at the end of the edge list
        bool end()
        {
            return (curEdge == G.m_Edges[NodeIndex].end());
        }
    };


    friend class EdgeIterator;

    //const class used to iterate through all the edges connected to a specific node. 
    class ConstEdgeIterator
    {
    private:                                                                
        typename EdgeList::const_iterator curEdge;

        const SparseGraph<node_type, edge_type>& G;

        const int NodeIndex;


    public:
        ConstEdgeIterator(const SparseGraph<node_type, edge_type>& graph,int node): G(graph),NodeIndex(node)
        {
            /* we don't need to check for an invalid node index since if the node is
            invalid there will be no associated edges
            */
            curEdge = G.m_Edges[NodeIndex].begin();
        }

        const EdgeType* begin()
        {        
            curEdge = G.m_Edges[NodeIndex].begin();

            if (end()) return NULL;

            return &(*curEdge);
        }

        const EdgeType* next()
        {
            ++curEdge;

            if(end())
            {
                return NULL;
            }
            else
            {
                return &(*curEdge);
            }
        }

        //return true if we are at the end of the edge list
        bool end()
        {
            return (curEdge == G.m_Edges[NodeIndex].end());
        }
    };

    friend class ConstEdgeIterator;


    //non const class used to iterate through the nodes in the graph
    class NodeIterator
    {
    private:
        typename NodeVector::iterator curNode;

        SparseGraph<node_type, edge_type>& G;

        //if a graph node is removed, it is not removed from the 
        //vector of nodes (because that would mean changing all the indices of 
        //all the nodes that have a higher index). This method takes a node
        //iterator as a parameter and assigns the next valid element to it.
        void getNextValidNode(typename NodeVector::iterator& it)
        {
            if ( curNode == G.m_Nodes.end() || it->Index() != -1) return;

            while ( (it->Index() == -1) )
            {
                ++it;

                if (curNode == G.m_Nodes.end()) break;
            }
        }


    public:
        NodeIterator(SparseGraph<node_type, edge_type> &graph):G(graph)
        {
            curNode = G.m_Nodes.begin();
        }


        node_type* begin()
        {      
            curNode = G.m_Nodes.begin();

            getNextValidNode(curNode);

            return &(*curNode);
        }

        node_type* next()
        {
            ++curNode;

            if (end()) return NULL;

            getNextValidNode(curNode);

            return &(*curNode);
        }

        bool end()
        {
            return (curNode == G.m_Nodes.end());
        }
    };

     
    friend class NodeIterator;


    //const class used to iterate through the nodes in the graph
    class ConstNodeIterator
    {
    private:
        typename NodeVector::const_iterator curNode;

        const SparseGraph<node_type, edge_type>& G;

        //if a graph node is removed or switched off, it is not removed from the 
        //vector of nodes (because that would mean changing all the indices of 
        //all the nodes that have a higher index. This method takes a node
        //iterator as a parameter and assigns the next valid element to it.
        void getNextValidNode(typename NodeVector::const_iterator& it)
        {
            if ( curNode == G.m_Nodes.end() || it->Index() != -1) return;

            while ( (it->Index() == -1) )
            {
                ++it;

                if (curNode == G.m_Nodes.end()) break;
            }
        }

    public:
        ConstNodeIterator(const SparseGraph<node_type, edge_type> &graph):G(graph)
        {
            curNode = G.m_Nodes.begin();
        }


        const node_type* begin()
        {      
            curNode = G.m_Nodes.begin();

            getNextValidNode(curNode);

            return &(*curNode);
        }

        const node_type* next()
        {
            ++curNode;

            if (end())
            {
                return NULL;
            }
            else
            {
                getNextValidNode(curNode);

                return &(*curNode);
            }
        }

        bool end()
        {
            return (curNode == G.m_Nodes.end());
        }
    };

    friend class ConstNodeIterator;
};





//--------------------------- isNodePresent --------------------------------
//
//  returns true if a node with the given index is present in the graph
//--------------------------------------------------------------------------
template <class node_type, class edge_type>
bool SparseGraph<node_type, edge_type>::isNodePresent(int nd)const
{
    if ((nd >= (int)m_Nodes.size() || (m_Nodes[nd].Index() == -1)))
    {
        return false;
    }

    return true;
}

//--------------------------- isEdgePresent --------------------------------
//
//  returns true if an edge with the given from/to is present in the graph
//--------------------------------------------------------------------------
template <class node_type, class edge_type>
bool SparseGraph<node_type, edge_type>::isEdgePresent(int from, int to)const
{
    if (isNodePresent(from) && isNodePresent(from))
    {
        for (EdgeList::const_iterator curEdge = m_Edges[from].begin(); curEdge != m_Edges[from].end(); ++curEdge)
        {
            if (curEdge->to() == to) return true;
        }

        return false;
    }

    return false;
}

//------------------------------ GetNode -------------------------------------
//
//  const and non const methods for obtaining a reference to a specific node
//----------------------------------------------------------------------------
template <class node_type, class edge_type>
const node_type&  SparseGraph<node_type, edge_type>::getNode(int idx)const
{
    assert( (idx < (int)m_Nodes.size()) &&(idx >=0) && "<SparseGraph::GetNode>: invalid index");

    return m_Nodes[idx];
}

  //non const version
template <class node_type, class edge_type>
node_type&  SparseGraph<node_type, edge_type>::getNode(int idx)
{
    assert( (idx < (int)m_Nodes.size()) &&(idx >=0) &&"<SparseGraph::GetNode>: invalid index");
    
    return m_Nodes[idx];
}

//------------------------------ GetEdge -------------------------------------
//
//  const and non const methods for obtaining a reference to a specific edge
//----------------------------------------------------------------------------
template <class node_type, class edge_type>
const edge_type& SparseGraph<node_type, edge_type>::getEdge(int from, int to)const
{
    assert( (from < m_Nodes.size()) &&(from >=0) && m_Nodes[from].Index() != -1 &&
                "<SparseGraph::GetEdge>: invalid 'from' index");

    assert( (to < m_Nodes.size()) && (to >=0) && m_Nodes[to].Index() != -1 &&
                "<SparseGraph::GetEdge>: invalid 'to' index");

    for (EdgeList::const_iterator curEdge = m_Edges[from].begin(); curEdge != m_Edges[from].end(); ++curEdge)
    {
        if (curEdge->to() == to) return *curEdge;
    }

    assert (0 && "<SparseGraph::GetEdge>: edge does not exist");
}

//non const version
template <class node_type, class edge_type>
edge_type& SparseGraph<node_type, edge_type>::getEdge(int from, int to)
{
    assert( (from < m_Nodes.size()) && (from >=0) && m_Nodes[from].Index() != -1 &&
                "<SparseGraph::GetEdge>: invalid 'from' index");

    assert( (to < m_Nodes.size()) && (to >=0) && m_Nodes[to].Index() != -1 &&
                "<SparseGraph::GetEdge>: invalid 'to' index");

    for (EdgeList::iterator curEdge = m_Edges[from].begin(); curEdge != m_Edges[from].end(); ++curEdge)
    {
        if (curEdge->to() == to) return *curEdge;
    }

    assert (0 && "<SparseGraph::GetEdge>: edge does not exist");
}

//-------------------------- AddEdge ------------------------------------------
//
//  Use this to add an edge to the graph. The method will ensure that the
//  edge passed as a parameter is valid before adding it to the graph. If the
//  graph is a digraph then a similar edge connecting the nodes in the opposite
//  direction will be automatically added.
//-----------------------------------------------------------------------------
template <class node_type, class edge_type>
void SparseGraph<node_type, edge_type>::addEdge(EdgeType edge)
{
    //first make sure the from and to nodes exist within the graph 
    assert( (edge.from() < m_iNextNodeIndex) && (edge.to() < m_iNextNodeIndex) &&
                "<SparseGraph::AddEdge>: invalid node index");

    //make sure both nodes are active before adding the edge
    if ( (m_Nodes[edge.to()].Index() != -1) &&  (m_Nodes[edge.from()].Index() != -1))
    {
        //add the edge, first making sure it is unique
        if (isUniqueEdge(edge.from(), edge.to()))
        {
            m_Edges[edge.from()].push_back(edge);
        }

        //if the graph is undirected we must add another connection in the opposite
        //direction
        if (!m_bDigraph)
        {
            //check to make sure the edge is unique before adding
            if (isUniqueEdge(edge.to(), edge.from()))
            {
                EdgeType NewEdge = edge;

                NewEdge.setTo(edge.from());
                NewEdge.setFrom(edge.to());

                m_Edges[edge.to()].push_back(NewEdge);
            }
        }
    }
}


//----------------------------- RemoveEdge ---------------------------------
template <class node_type, class edge_type>
void SparseGraph<node_type, edge_type>::removeEdge(int from, int to)
{
    assert ( (from < (int)m_Nodes.size()) && (to < (int)m_Nodes.size()) &&
                "<SparseGraph::RemoveEdge>:invalid node index");

    EdgeList::iterator curEdge;
  
    if (!m_bDigraph)
    {
        for (curEdge = m_Edges[to].begin(); curEdge != m_Edges[to].end(); ++curEdge)
        {
            if (curEdge->to() == from){curEdge = m_Edges[to].erase(curEdge);break;}
        }
    }

    for (curEdge = m_Edges[from].begin(); curEdge != m_Edges[from].end(); ++curEdge)
    {
        if (curEdge->to() == to){curEdge = m_Edges[from].erase(curEdge);break;}
    }
}

//-------------------------- AddNode -------------------------------------
//
//  Given a node this method first checks to see if the node has been added
//  previously but is now innactive. If it is, it is reactivated.
//
//  If the node has not been added previously, it is checked to make sure its
//  index matches the next node index before being added to the graph
//------------------------------------------------------------------------
template <class node_type, class edge_type>
int SparseGraph<node_type, edge_type>::addNode(node_type node)
{
    if (node.Index() < (int)m_Nodes.size())
    {
        //make sure the client is not trying to add a node with the same ID as
        //a currently active node
        assert (m_Nodes[node.Index()].Index() == -1 &&
                     "<SparseGraph::AddNode>: Attempting to add a node with a duplicate ID");

        m_Nodes[node.Index()] = node;

        return m_iNextNodeIndex;
    }
  
    else
    {
        //make sure the new node has been indexed correctly
        assert (node.Index() == m_iNextNodeIndex && "<SparseGraph::AddNode>:invalid index");

        m_Nodes.push_back(node);
        m_Edges.push_back(EdgeList());

        return m_iNextNodeIndex++;
    }
}

//----------------------- CullInvalidEdges ------------------------------------
//
//  iterates through all the edges in the graph and removes any that point
//  to an invalidated node
//-----------------------------------------------------------------------------
template <class node_type, class edge_type>
void SparseGraph<node_type, edge_type>::cullInvalidEdges()
{
    for (EdgeListVector::iterator curEdgeList = m_Edges.begin(); curEdgeList != m_Edges.end(); ++curEdgeList)
    {
        for (EdgeList::iterator curEdge = (*curEdgeList).begin(); curEdge != (*curEdgeList).end(); ++curEdge)
        {
            if (m_Nodes[curEdge->to()].Index() == -1 || 
            m_Nodes[curEdge->from()].Index() == -1)
            {
                curEdge = (*curEdgeList).erase(curEdge);
            }
        }
    }
}

  
//------------------------------- RemoveNode -----------------------------
//
//  Removes a node from the graph and removes any links to neighbouring nodes
//------------------------------------------------------------------------
template <class node_type, class edge_type>
void SparseGraph<node_type, edge_type>::removeNode(int node)                                   
{
    assert(node < (int)m_Nodes.size() && "<SparseGraph::RemoveNode>: invalid node index");

    //set this node's index to -1
    m_Nodes[node].setIndex(-1);

    //if the graph is not directed remove all edges leading to this node and then
    //clear the edges leading from the node
    if (!m_bDigraph)
    {    
        //visit each neighbour and erase any edges leading to this node
        for (EdgeList::iterator curEdge = m_Edges[node].begin(); curEdge != m_Edges[node].end(); ++curEdge)
        {
            for (EdgeList::iterator curE = m_Edges[curEdge->to()].begin(); curE != m_Edges[curEdge->to()].end(); ++curE)
            {
                if (curE->to() == node)
                {
                    curE = m_Edges[curEdge->to()].erase(curE);
                    break;
                }
            }
        }

        //finally, clear this node's edges
        m_Edges[node].clear();
    }
    //if a digraph remove the edges the slow way
    else
    {
        cullInvalidEdges();
    }
}

//-------------------------- SetEdgeCost ---------------------------------
//
//  Sets the cost of a specific edge
//------------------------------------------------------------------------
template <class node_type, class edge_type>
void SparseGraph<node_type, edge_type>::setEdgeCost(int from, int to, float NewCost)
{
    //make sure the nodes given are valid
    assert( (from < (int)m_Nodes.size()) && (to < (int)m_Nodes.size()) &&
                "<SparseGraph::SetEdgeCost>: invalid index");

    //visit each neighbour and erase any edges leading to this node
    for (EdgeList::iterator curEdge = m_Edges[from].begin(); curEdge != m_Edges[from].end(); ++curEdge)
    {
        if (curEdge->to() == to)
        {
            curEdge->setCost(NewCost);
            break;
        }
    }
}

//-------------------------------- isUniqueEdge ----------------------------
//
//  returns true if the edge is not present in the graph. Used when adding
//  edges to prevent duplication
//------------------------------------------------------------------------
template <class node_type, class edge_type>
bool SparseGraph<node_type, edge_type>::isUniqueEdge(int from, int to)const
{
    for (EdgeList::const_iterator curEdge = m_Edges[from].begin();curEdge != m_Edges[from].end(); ++curEdge)
    {
        if (curEdge->to() == to)
        {
            return false;
        }
    }

    return true;
}

//-------------------------------- Save ---------------------------------------

template <class node_type, class edge_type>
bool SparseGraph<node_type, edge_type>::save(const char* FileName)const
{
    //open the file and make sure it's valid
    std::ofstream out(FileName);

    if (!out)
    {
        throw std::runtime_error("Cannot open file: " + std::string(FileName));
        return false;
    }

    return save(out);
}


//-------------------------------- Save ---------------------------------------
template <class node_type, class edge_type>
bool SparseGraph<node_type, edge_type>::save(std::ofstream& stream)const
{
    //save the number of nodes
    stream << m_Nodes.size() << std::endl;

    //iterate through the graph nodes and save them
    NodeVector::const_iterator curNode = m_Nodes.begin();
    for (curNode; curNode!=m_Nodes.end(); ++curNode)
    {
        stream << *curNode;
    }

    //save the number of edges
    stream << NumEdges() << std::endl;


    //iterate through the edges and save them
    for (unsigned int nodeIdx = 0; nodeIdx < m_Nodes.size(); ++nodeIdx)
    {
        for (EdgeList::const_iterator curEdge = m_Edges[nodeIdx].begin();
                curEdge!=m_Edges[nodeIdx].end(); ++curEdge)
        {
            stream << *curEdge;
        }  
    }

    return true;
}


//------------------------------- Load ----------------------------------------
//-----------------------------------------------------------------------------
template <class node_type, class edge_type>
bool SparseGraph<node_type, edge_type>::load(const char* FileName)
{
    //open file and make sure it's valid
    std::ifstream in(FileName);

    if (!in)
    {
        throw std::runtime_error("Cannot open file: " + std::string(FileName));
        return false;
    }

    return load(in);
}

//------------------------------- Load ----------------------------------------
//-----------------------------------------------------------------------------
template <class node_type, class edge_type>
bool SparseGraph<node_type, edge_type>::load(std::ifstream& stream)
{
    clear();

    //get the number of nodes and read them in
    int NumNodes, NumEdges;

    stream >> NumNodes;

    for (int n=0; n<NumNodes; ++n)
    {
        NodeType NewNode(stream);

        //when editing graphs it's possible to end up with a situation where some
        //of the nodes have been invalidated (their id's set to -1). Therefore
        //when a node of index -1 is encountered, it must still be added.
        if (NewNode.Index() != -1)
        {
            addNode(NewNode);
        }
        else
        {
            m_Nodes.push_back(NewNode);

            //make sure an edgelist is added for each node
            m_Edges.push_back(EdgeList());

            ++m_iNextNodeIndex;
        }
    }

    //now add the edges
    stream >> NumEdges;
    for (int e=0; e<NumEdges; ++e)
    {
        EdgeType NextEdge(stream);

        addEdge(NextEdge);
    }

    return true;
}

#endif
