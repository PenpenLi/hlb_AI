
#ifndef GAMEWORLD_H
#define GAMEWORLD_H 

#include "common/misc/CellSpacePartition.h"
#include "common/game/BaseNode.h"
#include "Vehicle.h"

//#include "2d/CCNode.h"
//using namespace cocos2d;

class Wall;
class Obstacle;

class GameWorldVehicle:public BaseNode
{
public:
    GameWorldVehicle(Vector2D winSize, bool isCellSpaceOn);
    ~GameWorldVehicle();
    void onEnter();
    void update(float dt);

    static GameWorldVehicle* create(int width, int height, bool isCellSpaceOn);
    
    void createWalls();
    const std::vector<Wall *>& getWalls() {return m_Walls;}                          
    CellSpacePartition<Vehicle *> *getCellSpace() {return m_pCellSpace;}
    const std::vector<Obstacle *>& getObstacles() {return m_Obstacles;}
    const std::vector<Vehicle*>& getVehicles() {return m_Vehicles;}
    
    void spacePartitioningOn() {m_bCellSpaceOn = true;};
    void spacePartitioningOff() {m_bCellSpaceOn = false;};
    bool isSpacePartitioningOn()const{return m_bCellSpaceOn;}
    void removeEntity(Vehicle *entity);
    void tagNeighborsWithinViewRange(BaseEntity* pVehicle, float radius);
    void tagObstaclesWithinViewRange(BaseEntity* pVehicle, float radius);

    //------------------------- Set Crosshair ------------------------------------
    //
    //  The user can set the position of the crosshair by right clicking the
    //  mouse. This method makes sure the click is not inside any enabled
    //  Obstacles and sets the position appropriately
    //------------------------------------------------------------------------
    void setCrosshair(Vector2D pos) { m_vCrosshair.x = pos.x; m_vCrosshair.y = pos.y;}   
    Vector2D getCrosshair() const {return m_vCrosshair;}
    
private:    
    //a container of all the moving entities
    std::vector<Vehicle*> m_Vehicles;
    
    //any obstacles
    std::vector<Obstacle*> m_Obstacles;
    
    //container containing any walls in the environment
    std::vector<Wall *> m_Walls;
    
    CellSpacePartition<Vehicle *>* m_pCellSpace;
  
    //flags to turn aids and obstacles etc on/off
    bool  m_bShowWalls;
    bool  m_bShowObstacles;

    bool m_bCellSpaceOn;

    Vector2D m_vCrosshair; //user set point
    Vector2D m_WinSize;
};

#endif 

