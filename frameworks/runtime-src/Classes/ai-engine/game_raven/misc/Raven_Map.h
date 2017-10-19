#ifndef RAVEN_MAP_H
#define RAVEN_MAP_H
#pragma warning (disable:4786)
//-----------------------------------------------------------------------------
//
//  Name:   Raven_Map.h
//
//
//  Desc:   this class creates and stores all the entities that make up the
//          Raven game environment. (walls, bots, health etc)
//
//          It can read a Raven map editor file and recreate the necessary
//          geometry.
//-----------------------------------------------------------------------------
#include <vector>
#include <string>
#include <list>
#include "common/game/Wall.h"
#include "common/triggers/Trigger.h"
#include "common/triggers/TriggerSystem.h"
#include "common/graph/GraphEdgeTypes.h"
#include "common/graph/GraphNodeTypes.h"
#include "common/graph/SparseGraph.h"
#include "common/misc/cellSpacePartition.h"
#include "Raven_Bot.h"

class BaseEntity;
class Raven_Door;

class Raven_Map
{
public:
    typedef NavGraphNode<Trigger<Raven_Bot>*> GraphNode;
    typedef SparseGraph<GraphNode, NavGraphEdge> NavGraph;
    typedef CellSpacePartition<NavGraph::NodeType*> CellSpace;

    typedef Trigger<Raven_Bot> TriggerType;
    typedef TriggerSystem<TriggerType> TriggerSystem;
  
public:
    Raven_Map();  
    ~Raven_Map();

    //loads an environment from a file
    bool loadMap(const std::string& FileName); 

    //adds a wall and returns a pointer to that wall. (this method can be
    //used by objects such as doors to add walls to the environment)
    Wall* addWall(Vector2D from, Vector2D to);

    void addSoundTrigger(Raven_Bot* pSoundSource, float range);

    float calculateCostToTravelBetweenNodes(int nd1, int nd2)const;

    //returns the position of a graph node selected at random
    Vector2D getRandomNodeLocation()const;
 
    void updateTriggerSystem(std::list<Raven_Bot*>& bots);

    const Raven_Map::TriggerSystem::TriggerList& getTriggers()const{return m_TriggerSystem.getTriggers();}
    const std::vector<Wall*>& getWalls()const{return m_Walls;}
    NavGraph& getNavGraph()const{return *m_pNavGraph;}
    std::vector<Raven_Door*>& getDoors(){return m_Doors;}
    const std::vector<Vector2D>& getSpawnPoints()const{return m_SpawnPoints;}
    CellSpace* const getCellSpace()const{return m_pSpacePartition;}
    Vector2D getRandomSpawnPoint(){return m_SpawnPoints[RandIntInRange(0,m_SpawnPoints.size()-1)];}
    int getSizeX()const{return m_iSizeX;}
    int getSizeY()const{return m_iSizeY;}
    int getMaxDimension()const{return std::max(m_iSizeX, m_iSizeY);}
    float getCellSpaceNeighborhoodRange()const{return m_dCellSpaceNeighborhoodRange;}


private:

  //the walls that comprise the current map's architecture. 
  std::vector<Wall*> m_Walls;

  //trigger are objects that define a region of space. When a raven bot
  //enters that area, it 'triggers' an event. That event may be anything
  //from increasing a bot's health to opening a door or requesting a lift.
  TriggerSystem m_TriggerSystem;    

  //this holds a number of spawn positions. When a bot is instantiated
  //it will appear at a randomly selected point chosen from this vector
  std::vector<Vector2D> m_SpawnPoints;

  //a map may contain a number of sliding doors.
  std::vector<Raven_Door*> m_Doors;
 
  //this map's accompanying navigation graph
  NavGraph* m_pNavGraph;  

  //the graph nodes will be partitioned enabling fast lookup
  CellSpace* m_pSpacePartition;

  //the size of the search radius the cellspace partition uses when looking for 
  //neighbors 
  float m_dCellSpaceNeighborhoodRange;

  int m_iSizeX;
  int m_iSizeY;
  
  void partitionNavGraph();

    //this will hold a pre-calculated lookup table of the cost to travel from
    //one node to any other.
    std::vector<std::vector<float>> m_PathCosts;

    //stream constructors for loading from a file
    void addSpawnPoint(float x, float y);
    void addHealth_Giver(std::ifstream& in);
    void addWeapon_Giver(int type_of_weapon, std::ifstream& in);
    void addDoor(std::ifstream& in);
    void addDoorTrigger(std::ifstream& in);
    void clear();
    
};



#endif