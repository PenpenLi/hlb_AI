#include "Raven_Map.h"
#include "Raven_Door.h"
#include "common/game/EntityManager.h"
#include "common/graph/HandyGraphFunctions.h"
#include "../triggers/Trigger_OnButtonSendMsg.h"
#include "../triggers/Trigger_HealthGiver.h"
#include "../triggers/Trigger_WeaponGiver.h"
#include "../triggers/Trigger_SoundNotify.h"

//----------------------------- ctor ------------------------------------------
//-----------------------------------------------------------------------------
Raven_Map::Raven_Map():m_pNavGraph(NULL),
                                            m_pSpacePartition(NULL),
                                            m_iSizeY(0),
                                            m_iSizeX(0),
                                            m_dCellSpaceNeighborhoodRange(0)
{
    
}

//------------------------------ dtor -----------------------------------------
//-----------------------------------------------------------------------------
Raven_Map::~Raven_Map()
{
    clear();
}

//---------------------------- clear ------------------------------------------
//
//  deletes all the current objects ready for a map load
//-----------------------------------------------------------------------------
void Raven_Map::clear()
{
    //delete the triggers
    m_TriggerSystem.clear();

    //delete the doors
    std::vector<Raven_Door*>::iterator curDoor = m_Doors.begin();
    for (curDoor; curDoor != m_Doors.end(); ++curDoor)
    {
        delete *curDoor;
    }
    m_Doors.clear();

    std::vector<Wall*>::iterator curWall = m_Walls.begin();
    for (curWall; curWall != m_Walls.end(); ++curWall)
    {
        delete *curWall;
    }
    m_Walls.clear();
    
    m_SpawnPoints.clear();

    //delete the navgraph
    if (m_pNavGraph)
    {
        delete m_pNavGraph;   
    }
    
    //delete the partioning info
    if (m_pSpacePartition)
    {
        delete m_pSpacePartition;
    }
}


Wall* Raven_Map::addWall(Vector2D from, Vector2D to)
{
    Wall* w = new Wall(from, to);

    m_Walls.push_back(w);

    return w;
}

//--------------------------- addDoor -----------------------------------------
//-----------------------------------------------------------------------------
void Raven_Map::addDoor(Vector2D left, Vector2D right)
{
    Raven_Door* pDoor = new Raven_Door(this, left, right);

    m_Doors.push_back(pDoor);

    //register the entity 
    EntityManager::instance()->addEntity(pDoor);
}

//--------------------------- addDoorTrigger ----------------------------------
//-----------------------------------------------------------------------------
void Raven_Map::addDoorTrigger(std::ifstream& in)
{
    Trigger_OnButtonSendMsg<Raven_Bot>* tr = new Trigger_OnButtonSendMsg<Raven_Bot>(in);

    m_TriggerSystem.registerTrigger(tr);

    //register the entity 
    EntityManager::instance()->addEntity(tr);
}


//---------------------------- addSpawnPoint ----------------------------------
//-----------------------------------------------------------------------------
void Raven_Map::addSpawnPoint(float x, float y)
{
    m_SpawnPoints.push_back(Vector2D(x,y));
}


//----------------------- AddHealth__Giver ----------------------------------
//-----------------------------------------------------------------------------
void Raven_Map::addHealth_Giver(std::ifstream& in)
{
    Trigger_HealthGiver* hg = new Trigger_HealthGiver(in);

    m_TriggerSystem.registerTrigger(hg);

    //let the corresponding navgraph node point to this object
    NavGraph::NodeType& node = m_pNavGraph->getNode(hg->graphNodeIndex());

    node.setExtraInfo(hg);

    //register the entity 
    EntityManager::instance()->addEntity(hg);
}

//----------------------- AddWeapon__Giver ----------------------------------
//-----------------------------------------------------------------------------
void Raven_Map::addWeapon_Giver(int type_of_weapon, std::ifstream& in)
{
    Trigger_WeaponGiver* wg = new Trigger_WeaponGiver(in);

    wg->setEntityType(type_of_weapon);

    //add it to the appropriate vectors
    m_TriggerSystem.registerTrigger(wg);

    //let the corresponding navgraph node point to this object
    NavGraph::NodeType& node = m_pNavGraph->getNode(wg->graphNodeIndex());

    node.setExtraInfo(wg);

    //register the entity 
    EntityManager::instance()->addEntity(wg);
}


//------------------------- loadMap ------------------------------------
//
//  sets up the game environment from map file
//-----------------------------------------------------------------------------
bool Raven_Map::loadMap(const std::string& filename)
{  
    std::ifstream in(filename.c_str());
    if (!in)
    {
        AILOG("Bad Map Filename");
        return false;
    }

    clear();

    //first of all read and create the navgraph. This must be done before
    //the entities are read from the map file because many of the entities
    //will be linked to a graph node (the graph node will own a pointer
    //to an instance of the entity)
    m_pNavGraph = new NavGraph(false); //create SparseGraph 

    m_pNavGraph->load(in);

    //determine the average distance between graph nodes so that we can
    //partition them efficiently
    m_dCellSpaceNeighborhoodRange = calculateAverageGraphEdgeLength(*m_pNavGraph) + 1;




  //load in the map size and adjust the client window accordingly
  in >> m_iSizeX >> m_iSizeY;

    //partition the graph nodes
    partitionNavGraph();

  //now create the environment entities
  while (!in.eof())
  {   
    //get type of next map object
    int EntityType;
    
    in >> EntityType;
    
    //create the object
    switch(EntityType)
    {
    case type_wall:
 
        addWall(in); break;

    case type_sliding_door:
 
        addDoor(in); break;

    case type_door_trigger:
 
        addDoorTrigger(in); break;

   case type_spawn_point:
     
       addSpawnPoint(in); break;

   case type_health:
     
       addHealth_Giver(in); break;

   case type_shotgun:
     
       addWeapon_Giver(type_shotgun, in); break;

   case type_rail_gun:
     
       addWeapon_Giver(type_rail_gun, in); break;

   case type_rocket_launcher:
     
       addWeapon_Giver(type_rocket_launcher, in); break;

    default:
      
      throw std::runtime_error("<Map::load>: Attempting to load undefined object");

      return false;
      
    }//end switch
  }

    //calculate the cost lookup table
    m_PathCosts = createAllPairsCostsTable(*m_pNavGraph);

  return true;
}





//------------- calculateCostToTravelBetweenNodes -----------------------------
//
//  Uses the pre-calculated lookup table to determine the cost of traveling
//  from nd1 to nd2
//-----------------------------------------------------------------------------
float Raven_Map::calculateCostToTravelBetweenNodes(int nd1, int nd2)const
{
    assert (nd1>=0 && nd1<m_pNavGraph->getNumNodes() &&
                  nd2>=0 && nd2<m_pNavGraph->getNumNodes() &&
                  "<Raven_Map::CostBetweenNodes>: invalid index");

    return m_PathCosts[nd1][nd2];
}




//-------------------------- PartitionEnvironment -----------------------------
//-----------------------------------------------------------------------------
void Raven_Map::partitionNavGraph()
{
    if (m_pSpacePartition) delete m_pSpacePartition;

    m_pSpacePartition = new CellSpacePartition<NavGraph::NodeType*>(m_iSizeX,
                                                                                          m_iSizeY,
                                                                                          Para_NumCellsX,
                                                                                          Para_NumCellsY,
                                                                                          m_pNavGraph->getNumNodes());

    //add the graph nodes to the space partition
    NavGraph::NodeIterator NodeItr(*m_pNavGraph);
    for (NavGraph::NodeType* pN=NodeItr.begin();!NodeItr.end();pN=NodeItr.next())
    {
        m_pSpacePartition->addEntity(pN);
    }   
}

//---------------------------- addSoundTrigger --------------------------------
//
//  given the bot that has made a sound, this method adds a SoundMade trigger
//-----------------------------------------------------------------------------
void Raven_Map::addSoundTrigger(Raven_Bot* pSoundSource, float range)
{
    m_TriggerSystem.registerTrigger(new Trigger_SoundNotify(pSoundSource, range));
}

//----------------------- updateTriggerSystem ---------------------------------
//
//  givena container of entities in the world this method updates them against
//  all the triggers
//-----------------------------------------------------------------------------
void Raven_Map::updateTriggerSystem(std::list<Raven_Bot*>& bots)
{
    m_TriggerSystem.update(bots);
}

//------------------------- getRandomNodeLocation -----------------------------
//
//  returns the position of a graph node selected at random
//-----------------------------------------------------------------------------
Vector2D Raven_Map::getRandomNodeLocation()const
{
    NavGraph::ConstNodeIterator NodeItr(*m_pNavGraph);
    int RandIndex = RandIntInRange(0, m_pNavGraph->getNumActiveNodes()-1);
    const NavGraph::NodeType* pN = NodeItr.begin();
    while (--RandIndex > 0)
    {
        pN = NodeItr.next();
    }

    return pN->pos();
}



