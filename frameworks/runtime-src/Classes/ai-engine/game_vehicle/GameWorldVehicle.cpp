
#include "GameConfig.h"
#include "GameWorldVehicle.h"
#include "common/misc/UtilsEx.h"
#include "common/misc/LogDebug.h"
#include "common/game/CommonFunction.h"
#include "common/game/EngineInterface.h"
#include "common/game/Wall.h"
#include "VehicleSteeringConfig.h"
#include "SteeringBehaviors.h"
#include "Obstacle.h"


//using namespace cocos2d;
#include "common/message/Telegram.h"


GameWorldVehicle::GameWorldVehicle(Vector2D winSize, bool isCellSpaceOn):
                                                m_WinSize(winSize),
                                                m_bCellSpaceOn(isCellSpaceOn),
                                                m_pCellSpace(nullptr),
                                                m_vCrosshair(Vector2D(winSize.x/2.0, winSize.y/2.0))
{
    int totalNum = 100;


    if (isCellSpaceOn)
    {
        m_pCellSpace = new CellSpacePartition<Vehicle*>(winSize, Cell_Num_X, Cell_Num_Y, 200);
    }

    for (int i=0; i<totalNum; ++i)
    {
        //determine a random starting position
        Vector2D pos = Vector2D(winSize.x * RandFloat_0_1(), winSize.y * RandFloat_0_1());
        Vehicle* pVehicle = new Vehicle( this,
                                                            pos,                 //initial position
                                                            Vector2D(0,0), //velocity
                                                            RandFloat_0_1()*_PI_*2, //start rotation
                                                            Para_VehicleMass,      //mass
                                                            Para_MaxForce,     //max force
                                                            Para_MaxSpeed,     //max velocity
                                                            _PI_);     //max turn rate


        m_Vehicles.push_back(pVehicle);
        
        pVehicle->getSteering()->flockingOn();
        
        pVehicle->smoothingOn();
        
        //setup the spatial subdivision class
        if (m_pCellSpace)
        {
            m_pCellSpace->addEntity(pVehicle);
        }
    }


#if 0 //test evade 
    loadImgToNode(m_Vehicles[totalNum-1], "game_vehicle/vehicle_1.png");
    m_Vehicles[totalNum-1]->getSteering()->flockingOff();
    m_Vehicles[totalNum-1]->getSteering()->wanderOn();
    m_Vehicles[totalNum-1]->setMaxSpeed(70);

    for (int i=0; i<totalNum-1; ++i)
    {
        m_Vehicles[i]->getSteering()->evadeOn(m_Vehicles[totalNum-1]);
    }
#endif 

#if 0 //test wall
    createWalls();
    for (int i=0; i<totalNum; ++i)
    {
        m_Vehicles[i]->getSteering()->wallAvoidanceOn();
    }
#endif

#if 0 //test obstacle 
    Obstacle *ob = new Obstacle(m_WinSize.x/2, m_WinSize.y/2);
    addChildToNode(this, ob);
    m_Obstacles.push_back(ob);
    for (int i=0; i<totalNum; ++i)
    {
        m_Vehicles[i]->getSteering()->obstacleAvoidanceOn();
    }    
#endif 

#if 0 //test pursuit
    for (int i=0; i<totalNum; ++i)
    {
        m_Vehicles[i]->getSteering()->flockingOff();
        if (i == 0)
            m_Vehicles[i]->getSteering()->wanderOn();
        else 
        {
            //m_Vehicles[i]->getSteering()->pursuitOn(m_Vehicles[0]);
        }
    } 
#endif 

#if 0 //test flee
    setCrosshair(Vector2D(m_WinSize.x/2, m_WinSize.y/2));
    for (int i=0; i<totalNum; ++i)
    {
        m_Vehicles[i]->getSteering()->flockingOff();
        m_Vehicles[i]->getSteering()->fleeOn();
    } 
#endif 

}

GameWorldVehicle::~GameWorldVehicle()
{
    for (unsigned int a=0; a<m_Vehicles.size(); ++a)
    {
        delete m_Vehicles[a];
    }
    
    for (unsigned int ob=0; ob<m_Obstacles.size(); ++ob)
    {
        delete m_Obstacles[ob];
    }

    for (unsigned int i=0; i<m_Obstacles.size(); ++i)
    {
        delete m_Walls[i];
    }

    
    if (m_pCellSpace != nullptr)
    {
        delete m_pCellSpace;
    }
}

void GameWorldVehicle::onEnter()
{
    AILOG("GameWorldVehicle::onEnter");  

    BaseNode::onEnter();
    enableScheduleUpdate(this);
    /*
    BaseNode::onEnter();
    scheduleUpdate(); 
    */
}

void GameWorldVehicle::update(float dt)
{
    //AILOG("GameWorldVehicle::update %f", dt);
    for (unsigned int i = 0; i < m_Vehicles.size(); ++i)
    {
        m_Vehicles[i]->update(dt);
    }
}

GameWorldVehicle* GameWorldVehicle::create(int width, int height, bool isCellSpaceOn)
{
    AILOG("GameWorldVehicle::create");
    GameWorldVehicle* ret = new (std::nothrow)GameWorldVehicle(Vector2D(width, height), isCellSpaceOn);

    return ret;    
}

void GameWorldVehicle::createWalls()
{
    //create the walls  
    float gap = 20.0f;
    float CornerSize = 0.2f;
    float vDist = m_WinSize.x - 2*gap;
    float hDist = m_WinSize.y - 2*gap;

    const int NumWallVerts = 8;

    Vector2D vert[NumWallVerts] = {Vector2D(hDist*CornerSize+gap, gap),
                                   Vector2D(m_WinSize.x-gap-hDist*CornerSize, gap),
                                   Vector2D(m_WinSize.x-gap, gap+vDist*CornerSize),
                                   Vector2D(m_WinSize.x-gap, m_WinSize.y-gap-vDist*CornerSize),
                                         
                                   Vector2D(m_WinSize.x-gap-hDist*CornerSize, m_WinSize.y-gap),
                                   Vector2D(hDist*CornerSize+gap, m_WinSize.y-gap),
                                   Vector2D(gap, m_WinSize.y-gap-vDist*CornerSize),
                                   Vector2D(gap, gap+vDist*CornerSize)};
  
  for (int i=0; i<NumWallVerts; ++i)
  {
        Wall *p = nullptr;
        if (i<NumWallVerts-1)
        {
            p = new Wall(vert[i], vert[i+1]);
        }
        else 
        {
            p = new Wall(vert[i], vert[0]);
        }
        if (p)
        {
            m_Walls.push_back(p);
        }
  }
}

void GameWorldVehicle::removeEntity(Vehicle *entity)
{
    std::vector<Vehicle*>::iterator it;
    for (it = m_Vehicles.begin(); it != m_Vehicles.end(); )
    {
        if (*it == entity)
        {
            it = m_Vehicles.erase(it);
        }
        else 
        {
            ++it;
        }
    }

    //remove from cell space partition
    if (m_bCellSpaceOn)
    {
        getCellSpace()->removeEntity(entity);
    }
}

void GameWorldVehicle::tagNeighborsWithinViewRange(BaseEntity* pVehicle, float radius)
{
    tagNeighbors(pVehicle, m_Vehicles, radius);
}

void GameWorldVehicle::tagObstaclesWithinViewRange(BaseEntity* pVehicle, float radius)
{
    tagNeighbors(pVehicle, m_Obstacles, radius);
}


