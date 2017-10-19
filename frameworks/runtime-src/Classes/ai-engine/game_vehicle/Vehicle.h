#ifndef VEHICLE_H
#define VEHICLE_H

#include "common/game/MovingEntity.h"
#include "common/misc/Smoother.h"

//#include <vector>
//#include <list>
//#include <string>

class GameWorldVehicle;
class SteeringBehavior;



class Vehicle : public MovingEntity
{
public:
    Vehicle(GameWorldVehicle* world,
                   Vector2D position,               
                   Vector2D velocity,
                   float rotation,
                   float mass,
                   float max_force,
                   float max_speed,
                   float max_turn_rate);

    ~Vehicle();
    
    //updates the vehicle's position and orientation
    void update(float dt);
    
    SteeringBehavior* const getSteering()const{return m_pSteering;}
    
    GameWorldVehicle* const getWorld()const{return m_pWorld;} 
    
    Vector2D getSmoothedHeading()const{return m_vSmoothedHeading;}

    bool isSmoothingOn()const{return m_bSmoothingOn;}
    void smoothingOn(){m_bSmoothingOn = true;}
    void smoothingOff(){m_bSmoothingOn = false;}
    
    float getTimeElapsed()const{return m_dTimeElapsed;}




private:
    void render();
    
    //a pointer to the world data. So a vehicle can access any obstacle, path, wall or agent data
    GameWorldVehicle* m_pWorld;

    //the steering behavior class
    SteeringBehavior* m_pSteering;

    //some steering behaviors give jerky looking movement. The
    //following members are used to smooth the vehicle's heading
    Smoother<Vector2D>*  m_pHeadingSmoother;
    
    //this vector represents the average of the vehicle's heading vector smoothed over the last few frames
    Vector2D m_vSmoothedHeading;

    //when true, smoothing is active
    bool m_bSmoothingOn;

    //keeps a track of the most recent update time. (some of the steering behaviors make use of this - see Wander)
    float m_dTimeElapsed;
    
    Vehicle& operator=(const Vehicle&);
    
    pNode m_ui;
};



#endif