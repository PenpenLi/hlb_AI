
#include "GameConfig.h"
#include "common/game/EngineInterface.h"
#include "common/game/CommonFunction.h"
#include "VehicleSteeringConfig.h"
#include "Vehicle.h"
#include "SteeringBehaviors.h"
#include "GameWorldVehicle.h"

Vehicle::Vehicle(GameWorldVehicle* world,
               Vector2D position, 
               Vector2D velocity,
               float rotation,
               float mass,
               float max_force,
               float max_speed,
               float max_turn_rate): MovingEntity(velocity,
                                                                        Vector2D(std::sin(rotation),-std::cos(rotation)),
                                                                        mass,
                                                                        max_turn_rate,
                                                                        max_speed,
                                                                        max_force),
                                                                        m_pWorld(world),
                                                                        m_vSmoothedHeading(Vector2D(0,0)),
                                                                        m_bSmoothingOn(false),
                                                                        m_dTimeElapsed(0.0f),
                                                                        m_ui(nullptr)
{ 
    m_vPosition = position;
    
    //set up the steering behavior class
    m_pSteering = new SteeringBehavior(this); 
    
    //set up the smoother
    m_pHeadingSmoother = new Smoother<Vector2D>(10, Vector2D(0.0, 0.0)); 
}


Vehicle::~Vehicle()
{
    getWorld()->removeEntity(this);

    delete m_pSteering;
    delete m_pHeadingSmoother;
    
    if (m_ui) 
    {
        removeChild(m_ui);
    }
}

//------------------------------ Update ----------------------------------
//
//  Updates the vehicle's position from a series of steering behaviors
//------------------------------------------------------------------------
void Vehicle::update(float time_elapsed)
{
    Vector2D SteeringForce;

    Vector2D oldPos = getPos();
    
    //update the time elapsed
    m_dTimeElapsed = time_elapsed;

    //calculate the combined force from each steering behavior in the vehicle's list
    SteeringForce = m_pSteering->calculate();
    
    //Acceleration = Force/Mass
    Vector2D acceleration = SteeringForce /m_dMass;
    
    //update velocity
    m_vVelocity += acceleration * time_elapsed; 

    //make sure vehicle does not exceed maximum velocity
    m_vVelocity.truncate(m_dMaxSpeed);
    
    //update the heading if the vehicle has a non zero velocity
    if (m_vVelocity.lengthSq() > 0.00000001)
    {
        m_vHeading = Vec2Normalize(m_vVelocity);
        m_vSide = m_vHeading.getPerp();
    }
    
    //update the position
    m_vPosition += m_vVelocity * time_elapsed;
    wrapAround(m_vPosition, Win_Width, Win_Height);

    //invoidOverlap(this, this->getWorld()->getVehicles());
    
    //update the vehicle's current cell if space partitioning is turned on
    if (getWorld()->isSpacePartitioningOn())
    {
        getWorld()->getCellSpace()->updateEntity(this, oldPos);
    }
    
    if (isSmoothingOn())
    {
        m_vSmoothedHeading = m_pHeadingSmoother->update(getHeading());
    }

    
    render();
    
}

void Vehicle::render()
{
    if (nullptr == m_ui) // init UI
    {
        auto node = loadImgToNode(getUINode(), "game_vehicle/vehicle.png");
        setSize(getNodeSize(node));
        m_ui = (pNode)node;
    }

    if (m_ui) //update UI
    {
        setNodePos(m_ui, m_vPosition);
    }
}

