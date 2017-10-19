#include "Raven_PathPlanner.h"
#include "common/message/MessageDispatcher.h"
#include "common/misc/CellSpacePartition.h"
#include "common/misc/UtilsEx.h"
#include "common/misc/LogDebug.h"
#include "game_raven/ParaConfigRaven.h"
#include "game_raven/Raven_Messages.h"
#include "common/navigation/PathManager.h"
#include <cassert>


Raven_PathPlanner::Raven_PathPlanner(Raven_Bot* owner):m_pOwner(owner),
                                                                    m_NavGraph(m_pOwner->getWorld()->getMap()->getNavGraph()),
                                                                    m_pCurrentSearch(NULL)
{
}

Raven_PathPlanner::~Raven_PathPlanner()
{
    getReadyForNewSearch();
}

//------------------------------ getReadyForNewSearch -----------------------------------
//
//  called by the search manager when a search has been terminated to free
//  up the memory used when an instance of the search was created
//-----------------------------------------------------------------------------
void Raven_PathPlanner::getReadyForNewSearch()
{
    //unregister any existing search with the path manager
    m_pOwner->getWorld()->getPathManager()->unRegisterPlan(this);

    //clean up memory used by any existing search
    if (m_pCurrentSearch)
    {
        delete m_pCurrentSearch;  
        m_pCurrentSearch = nullptr;
    }
}

//---------------------------- getCostToNode ----------------------------------
//
//  returns the cost to travel from the bot's current position to a specific 
 // graph node. This method makes use of the pre-calculated lookup table
//-----------------------------------------------------------------------------
float Raven_PathPlanner::getCostToNode(unsigned int NodeIdx)const
{
    //find the closest visible node to the bots position
    int nd = getClosestNodeToPosition(m_pOwner->getPos());

    //add the cost to this node
    float cost =Vec2Distance(m_pOwner->getPos(),m_NavGraph.getNode(nd).getPos());

    //add the cost to the target node and return
    return cost + m_pOwner->getWorld()->getMap()->calculateCostToTravelBetweenNodes(nd, NodeIdx);
}

//------------------------ getCostToClosestItem ---------------------------
//
//  returns the cost to the closest instance of the giver type. This method
//  makes use of the pre-calculated lookup table. Returns -1 if no active
//  trigger found
//-----------------------------------------------------------------------------
float Raven_PathPlanner::getCostToClosestItem(unsigned int GiverType)const
{
    //find the closest visible node to the bots position
    int nd = getClosestNodeToPosition(m_pOwner->getPos());

    //if no closest node found return failure
    if (nd < 0) return -1;

    float ClosestSoFar = FloatMax;

    //iterate through all the triggers to find the closest *active* trigger of type GiverType
    const Raven_Map::TriggerSystem::TriggerList& triggers = m_pOwner->getWorld()->getMap()->getTriggers();

    Raven_Map::TriggerSystem::TriggerList::const_iterator it;
    for (it = triggers.begin(); it != triggers.end(); ++it)
    {
        if ( ((*it)->getEntityType() == GiverType) && (*it)->isActive())
        {
            float cost = m_pOwner->getWorld()->getMap()->calculateCostToTravelBetweenNodes(nd,(*it)->graphNodeIndex());

            if (cost < ClosestSoFar)
            {
                ClosestSoFar = cost;
            }
        }
    }

    //return a negative value if no active trigger of the type found
    if (isEqual(ClosestSoFar, FloatMax))
    {
        return -1;
    }

    return ClosestSoFar;
}


//----------------------------- getPath ------------------------------------
//
//  called by an agent after it has been notified that a search has terminated
//  successfully. The method extracts the path from m_pCurrentSearch, adds
//  additional edges appropriate to the search type and returns it as a list of
//  PathEdges.
//-----------------------------------------------------------------------------
Raven_PathPlanner::Path Raven_PathPlanner::getPath()
{
    assert (m_pCurrentSearch && "<Raven_PathPlanner::GetPathAsNodes>: no current search");

    Path path =  m_pCurrentSearch->getPathAsPathEdges();

    int closest = getClosestNodeToPosition(m_pOwner->getPos());

    path.push_front(PathEdge(m_pOwner->getPos(),getNodePosition(closest),NavGraphEdge::normal));
    
    //if the bot requested a path to a location then an edge leading to the
    //destination must be added
    if (m_pCurrentSearch->getType() == Graph_SearchTimeSliced<EdgeType>::AStar)
    {   
        path.push_back(PathEdge(path.back().getDestination(),m_vDestinationPos,NavGraphEdge::normal));
    }

    //smooth paths if required
    if (para_path_smooth_quick)
    {
        smoothPathEdgesQuick(path);
    }

    if (para_path_smooth_precise)
    {
        smoothPathEdgesPrecise(path);
    }

    return path;
}

//--------------------------- SmoothPathEdgesQuick ----------------------------
//
//  smooths a path by removing extraneous edges.
//-----------------------------------------------------------------------------
void Raven_PathPlanner::smoothPathEdgesQuick(Path& path)
{
    //create a couple of iterators and point them at the front of the path
    Path::iterator e1(path.begin()), e2(path.begin());

    //increment e2 so it points to the edge following e1.
    ++e2;

    //while e2 is not the last edge in the path, step through the edges checking
    //to see if the agent can move without obstruction from the source node of
    //e1 to the destination node of e2. If the agent can move between those 
    //positions then the two edges are replaced with a single edge.
    while (e2 != path.end())
    {
        //check for obstruction, adjust and remove the edges accordingly
        if ( (e2->Behavior() == EdgeType::normal) &&
                    m_pOwner->canWalkBetween(e1->Source(), e2->Destination()) )
        {
            e1->SetDestination(e2->Destination());
            e2 = path.erase(e2);
        }
        else
        {
            e1 = e2;
            ++e2;
        }
    }
}


//----------------------- SmoothPathEdgesPrecise ---------------------------------
//
//  smooths a path by removing extraneous edges.
//-----------------------------------------------------------------------------
void Raven_PathPlanner::smoothPathEdgesPrecise(Path& path)
{
    //create a couple of iterators
    Path::iterator e1, e2;

    //point e1 to the beginning of the path
    e1 = path.begin();
    
    while (e1 != path.end())
    {
        //point e2 to the edge immediately following e1
        e2 = e1; 
        ++e2;

        //while e2 is not the last edge in the path, step through the edges
        //checking to see if the agent can move without obstruction from the 
        //source node of e1 to the destination node of e2. If the agent can move
        //between those positions then the any edges between e1 and e2 are
        //replaced with a single edge.
        while (e2 != path.end())
        {
            //check for obstruction, adjust and remove the edges accordingly
            if ( (e2->Behavior() == EdgeType::normal) &&
                            m_pOwner->canWalkBetween(e1->getSource(), e2->getDestination()))
            {
                e1->setDestination(e2->getDestination());
                e2 = path.erase(++e1, ++e2);
                e1 = e2;
                --e1;
            }
            else
            {
                ++e2;
            }
        }

        ++e1;
    }
}



//---------------------------- cycleOnce --------------------------------------
//
//  the path manager calls this to iterate once though the search cycle
//  of the currently assigned search algorithm.
//-----------------------------------------------------------------------------
int Raven_PathPlanner::cycleOnce()const
{
    assert (m_pCurrentSearch && "<Raven_PathPlanner::cycleOnce>: No search object instantiated");

    int result = m_pCurrentSearch->cycleOnce();

    //let the bot know of the failure to find a path
    if (result == target_not_found)
    {
        MessageDispatcher::instance()->dispatchMsg( 0, 
                                                                            -1, 
                                                                            m_pOwner->getID(),
                                                                            Msg_NoPathAvailable, 
                                                                            NULL);
    }
    //let the bot know a path has been found
    else if (result == target_found)
    {
        //if the search was for an item type then the final node in the path will
        //represent a giver trigger. Consequently, it's worth passing the pointer
        //to the trigger in the extra info field of the message. (The pointer
        //will just be NULL if no trigger)
        void* pTrigger = m_NavGraph.getNode(m_pCurrentSearch->getPathToTarget().back()).getExtraInfo();
        MessageDispatcher::instance()->dispatchMsg( 0, 
                                                                            -1, 
                                                                            m_pOwner->getID(),
                                                                            Msg_PathReady, 
                                                                            pTrigger);        
    }

    return result;
}

//------------------------ getClosestNodeToPosition ---------------------------
//
//  returns the index of the closest visible graph node to the given position
//-----------------------------------------------------------------------------
int Raven_PathPlanner::getClosestNodeToPosition(Vector2D pos)const
{
    float ClosestSoFar = FloatMax;
    int ClosestNode  = no_closest_node_found;

    //when the cell space is queried this the the range searched for neighboring
    //graph nodes. This value is inversely proportional to the density of a 
    //navigation graph (less dense = bigger values)
    const float range = m_pOwner->getWorld()->getMap()->getCellSpaceNeighborhoodRange();

    //calculate the graph nodes that are neighboring this position
    m_pOwner->getWorld()->getMap()->getCellSpace()->calculateNeighbors(pos, range);

    //iterate through the neighbors and sum up all the position vectors
    for (NodeType* pN = m_pOwner->getWorld()->getMap()->getCellSpace()->begin();
                 !m_pOwner->getWorld()->getMap()->getCellSpace()->end();     
                 pN = m_pOwner->getWorld()->getMap()->getCellSpace()->next())
    {
        //if the path between this node and pos is unobstructed calculate the
        //distance
        if (m_pOwner->canWalkBetween(pos, pN->getPos()))
        {
            float dist = Vec2DistanceSq(pos, pN->getPos());
            //keep a record of the closest so far
            if (dist < ClosestSoFar)
            {
                ClosestSoFar = dist;
                ClosestNode  = pN->getIndex();
            }
        }
    }
   
    return ClosestNode;
}

//--------------------------- requestPathToPosition ------------------------------
//
//  Given a target, this method first determines if nodes can be reached from 
//  the  bot's current position and the target position. If either end point
//  is unreachable the method returns false. 
//
//  If nodes are reachable from both positions then an instance of the time-
//  sliced A* search is created and registered with the search manager. the
//  method then returns true.
//        
//-----------------------------------------------------------------------------
bool Raven_PathPlanner::requestPathToPosition(Vector2D TargetPos)
{ 
    getReadyForNewSearch();

    //make a note of the target position.
    m_vDestinationPos = TargetPos;

    //if the target is walkable from the bot's position a path does not need to
    //be calculated, the bot can go straight to the position by ARRIVING at
    //the current waypoint
    if (m_pOwner->canWalkTo(TargetPos))
    { 
        return true;
    }
  
    //find the closest visible node to the bots position
    int ClosestNodeToBot = getClosestNodeToPosition(m_pOwner->getPos());

    //remove the destination node from the list and return false if no visible
    //node found. This will occur if the navgraph is badly designed or if the bot
    //has managed to get itself *inside* the geometry (surrounded by walls),
    //or an obstacle.
    if (ClosestNodeToBot == no_closest_node_found)
    { 
        AILOG("No closest node to bot found!");
        return false; 
    }

    AILOG("Closest node to bot is %d", ClosestNodeToBot);

    //find the closest visible node to the target position
    int ClosestNodeToTarget = getClosestNodeToPosition(TargetPos);
  
    //return false if there is a problem locating a visible node from the target.
    //This sort of thing occurs much more frequently than the above. For
    //example, if the user clicks inside an area bounded by walls or inside an
    //object.
    if (ClosestNodeToTarget == no_closest_node_found)
    { 
        AILOG("No closest node to target (%d)", ClosestNodeToTarget);
        return false; 
    }

    AILOG("Closest node to target is  %d", ClosestNodeToTarget);

    //create an instance of a the distributed A* search class
    typedef Graph_SearchAStar_TS<Raven_Map::NavGraph, Heuristic_Euclid> AStar;
   
    m_pCurrentSearch = new AStar(m_NavGraph, ClosestNodeToBot, ClosestNodeToTarget);

    //and register the search with the path manager
    m_pOwner->getWorld()->getPathManager()->registerPlan(this);
    
    return true;
}


//------------------------------ RequestPathToItem -----------------------------
//
// Given an item type, this method determines the closest reachable graph node
// to the bot's position and then creates a instance of the time-sliced 
// Dijkstra's algorithm, which it registers with the search manager
//
//-----------------------------------------------------------------------------
bool Raven_PathPlanner::requestPathToItem(unsigned int ItemType)
{    
    //clear the waypoint list and delete any active search
    getReadyForNewSearch();

    //find the closest visible node to the bots position
    int ClosestNodeToBot = getClosestNodeToPosition(m_pOwner->getPos());

    //remove the destination node from the list and return false if no visible
    //node found. This will occur if the navgraph is badly designed or if the bot
    //has managed to get itself *inside* the geometry (surrounded by walls),
    //or an obstacle
    if (ClosestNodeToBot == no_closest_node_found)
    { 
        AILOG("No closest node to bot found!");
        return false; 
    }

    //create an instance of the search algorithm
    typedef FindActiveTrigger<Trigger<Raven_Bot> > t_con; 
    typedef Graph_SearchDijkstras_TS<Raven_Map::NavGraph, t_con> DijSearch;
  
    m_pCurrentSearch = new DijSearch(m_NavGraph, ClosestNodeToBot, ItemType);  

    //register the search with the path manager
    m_pOwner->getWorld()->getPathManager()->registerPlan(this);
    
    return true;
}

//------------------------------ getNodePosition ------------------------------
//
//  used to retrieve the position of a graph node from its index. (takes
//  into account the enumerations 'non_graph_source_node' and 
//  'non_graph_target_node'
//----------------------------------------------------------------------------- 
Vector2D Raven_PathPlanner::getNodePosition(int idx)const
{
    return m_NavGraph.getNode(idx).getPos();
}
  

