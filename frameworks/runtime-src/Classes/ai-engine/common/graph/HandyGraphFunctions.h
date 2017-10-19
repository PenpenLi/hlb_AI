#ifndef GRAPH_FUNCS
#define GRAPH_FUNCS
//-----------------------------------------------------------------------------
//
//  Name:   HandyGraphFunctions.h
//
//
//  Desc:   As the name implies, some useful functions you can use with your graphs. 

//          For the function templates, make sure your graph interface complies
//          with the SparseGraph class
//-----------------------------------------------------------------------------
#include "../misc/UtilsEx.h"
#include "../navigation/GraphAlgorithms.h"
#include "../navigation/AStarHeuristicPolicies.h"



//--------------------------- isValidNeighbour -----------------------------
//
//  returns true if x,y is a valid position in the map
//------------------------------------------------------------------------
bool isValidNeighbour(int x, int y, int NumCellsX, int NumCellsY)
{
    return !((x < 0) || (x >= NumCellsX) || (y < 0) || (y >= NumCellsY));
}
  
//------------ GraphHelper_AddAllNeighboursToGridNode ------------------
//
//  use to add he eight neighboring edges of a graph node that 
//  is positioned in a grid layout
//------------------------------------------------------------------------
template <class graph_type>
void addAllNeighboursToGridNode(graph_type& graph,
                                                                                    int row,
                                                                                    int col,
                                                                                    int NumCellsX,
                                                                                    int NumCellsY)
{   
    for (int i=-1; i<2; ++i)
    {
        for (int j=-1; j<2; ++j)
        {
            int nodeX = col+j;
            int nodeY = row+i;

            //skip if equal to this node
            if ( (i == 0) && (j==0) ) continue;

            //check to see if this is a valid neighbour
            if (isValidNeighbour(nodeX, nodeY, NumCellsX, NumCellsY))
            {
                //calculate the distance to this node
                Vector2D PosNode      = graph.getNode(row*NumCellsX+col).getPos();
                Vector2D PosNeighbour = graph.getNode(nodeY*NumCellsX+nodeX).getPos();

                float dist = PosNode.distance(PosNeighbour);

                //this neighbour is okay so it can be added
                graph_type::EdgeType NewEdge(row*NumCellsX+col, nodeY*NumCellsX+nodeX, dist);
                graph.addEdge(NewEdge);

                //if graph is not a diagraph then an edge needs to be added going
                //in the other direction
                if (!graph.isDigraph())
                {
                    graph_type::EdgeType NewEdge(nodeY*NumCellsX+nodeX, row*NumCellsX+col, dist);
                    graph.addEdge(NewEdge);
                }
            }
        }
    }
}


//--------------------------- GraphHelper_CreateGrid --------------------------
//
//  creates a graph based on a grid layout. This function requires the 
//  dimensions of the environment and the number of cells required horizontally
//  and vertically 
//-----------------------------------------------------------------------------
template <class graph_type>
void createGrid(graph_type& graph,
                                             int cySize,
                                             int cxSize,
                                             int NumCellsY,
                                             int NumCellsX)
{ 
    //need some temporaries to help calculate each node center
    float CellWidth  = (float)cySize / (float)NumCellsX;
    float CellHeight = (float)cxSize / (float)NumCellsY;

    float midX = CellWidth/2;
    float midY = CellHeight/2;

  
    //first create all the nodes
    for (int row=0; row<NumCellsY; ++row)
    {
        for (int col=0; col<NumCellsX; ++col)
        {
            graph.addNode(NavGraphNode<>(graph.getNextFreeNodeIndex(),
                                                                Vector2D(midX + (col*CellWidth),
                                                                midY + (row*CellHeight))));
        }
    }
    
    //now to calculate the edges. (A position in a 2d array [x][y] is the
    //same as [y*NumCellsX + x] in a 1d array). Each cell has up to eight
    //neighbours.
    for (int row=0; row<NumCellsY; ++row)
    {
        for (int col=0; col<NumCellsX; ++col)
        {
            addAllNeighboursToGridNode(graph, row, col, NumCellsX, NumCellsY);
        }
    }
}  


//--------------------------- WeightNavGraphNodeEdges -------------------------
//
//  Given a cost value and an index to a valid node this function examines 
//  all a node's edges, calculates their length, and multiplies
//  the value with the weight. Useful for setting terrain costs.
//------------------------------------------------------------------------
template <class graph_type>
void weightNavGraphNodeEdges(graph_type& graph, int node, float weight)
{
    //make sure the node is present
    assert(node < graph.getNumNodes());

    //set the cost for each edge
    graph_type::ConstEdgeIterator ConstEdgeItr(graph, node);
    for (const graph_type::EdgeType* pE=ConstEdgeItr.begin();!ConstEdgeItr.end();pE=ConstEdgeItr.next())
    {
        //calculate the distance between nodes
        float dist = Vec2Distance(graph.getNode(pE->from()).getPos(), graph.getNode(pE->to()).getPos());

        //set the cost of this edge
        graph.setEdgeCost(pE->from(), pE->to(), dist * weight);

        //if not a digraph, set the cost of the parallel edge to be the same
        if (!graph.isDigraph())
        {      
            graph.setEdgeCost(pE->to(), pE->from(), dist * weight);
        }
    }
}


//----------------------- createAllPairsTable ---------------------------------
//
// creates a lookup table encoding the shortest path info between each node
// in a graph to every other
//-----------------------------------------------------------------------------
template <class graph_type>
std::vector<std::vector<int> > createAllPairsTable(const graph_type& G)
{
    enum {no_path = -1};
  
    std::vector<int> row(G.getNumNodes(), no_path);

    std::vector<std::vector<int> > ShortestPaths(G.getNumNodes(), row);

    for (int source=0; source<G.getNumNodes(); ++source)
    {
        //calculate the SPT for this node
        Graph_SearchDijkstra<graph_type> search(G, source);

        std::vector<const graph_type::EdgeType*> spt = search.getSPT();

        //now we have the SPT it's easy to work backwards through it to find
        //the shortest paths from each node to this source node
        for (int target = 0; target<G.getNumNodes(); ++target)
        {
            //if the source node is the same as the target just set to target
            if (source == target)
            {
                ShortestPaths[source][target] = target;
            }
            else
            {
                int nd = target;

                while ((nd != source) && (spt[nd] != 0))
                {
                ShortestPaths[spt[nd]->from][target]= nd;

                nd = spt[nd]->from;
                }
            }
        }//next target node
    }//next source node

    return ShortestPaths;
}


//----------------------- createAllPairsCostsTable -------------------------------
//
//  creates a lookup table of the cost associated from traveling from one
//  node to every other
//-----------------------------------------------------------------------------
template <class graph_type>
std::vector<std::vector<float> > createAllPairsCostsTable(const graph_type& G)
{
    //create a two dimensional vector
    std::vector<float> row(G.getNumNodes(), 0.0);
    std::vector<std::vector<float> > PathCosts(G.getNumNodes(), row);

    for (int source=0; source<G.getNumNodes(); ++source)
    {
        //do the search
        Graph_SearchDijkstra<graph_type> search(G, source);

        //iterate through every node in the graph and grab the cost to travel to
        //that node
        for (int target = 0; target<G.getNumNodes(); ++target)
        {
            if (source != target)
            {
                PathCosts[source][target]= search.getCostToNode(target);
            }
        }//next target node

    }//next source node

    return PathCosts;
}

//---------------------- calculateAverageGraphEdgeLength ----------------------
//
//  determines the average length of the edges in a navgraph (using the 
//  distance between the source & target node positions (not the cost of the 
//  edge as represented in the graph, which may account for all sorts of 
//  other factors such as terrain type, gradients etc)
//------------------------------------------------------------------------------
template <class graph_type>
float calculateAverageGraphEdgeLength(const graph_type& G)
{
    float TotalLength = 0;
    int NumEdgesCounted = 0;

    graph_type::ConstNodeIterator NodeItr(G);
    const graph_type::NodeType* pN;
    for (pN = NodeItr.begin(); !NodeItr.end(); pN=NodeItr.next())
    {
        graph_type::ConstEdgeIterator EdgeItr(G, pN->Index());
        for (const graph_type::EdgeType* pE = EdgeItr.begin(); !EdgeItr.end(); pE=EdgeItr.next())
        {
            //increment edge counter
            ++NumEdgesCounted;

            //add length of edge to total length
            TotalLength += Vec2Distance(G.getNode(pE->from()).getPos(), G.getNode(pE->to()).getPos());
        }
    }

    return TotalLength / (float)NumEdgesCounted;
}

//----------------------------- GetCostliestGraphEdge -------------------
//
//  returns the cost of the costliest edge in the graph
//-----------------------------------------------------------------------------
template <class graph_type>
float GetCostliestGraphEdge(const graph_type& G)
{
    float greatest = MinDouble;

    graph_type::ConstNodeIterator NodeItr(G);
    const graph_type::NodeType* pN;
    for (pN = NodeItr.begin(); !NodeItr.end(); pN=NodeItr.next())
    {
        graph_type::ConstEdgeIterator EdgeItr(G, pN->Index());
        for (const graph_type::EdgeType* pE = EdgeItr.begin(); !EdgeItr.end(); pE=EdgeItr.next())
        {
            if (pE->Cost() > greatest)greatest = pE->getCost();
        }
    }

    return greatest;
}

#endif
