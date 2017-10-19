
#include "GameConfig.h"
#include "SteeringBehaviors.h"
#include "Vehicle.h"
#include "common/game/Wall.h"
#include "common/2D/Transformations.h"
#include "common/2D/Geometry.h"
#include "common/misc/UtilsEx.h"
#include "common/misc/CellSpacePartition.h"
#include "VehicleSteeringConfig.h"
#include "GameWorldVehicle.h"
#include "Obstacle.h"
#include <cassert>


using std::string;
using std::vector;


//------------------------- ctor -----------------------------------------
//
//------------------------------------------------------------------------
SteeringBehavior::SteeringBehavior(Vehicle* agent):
                                                    m_pVehicle(agent),
                                                    m_iFlags(0),
                                                    m_dDBoxLength(GetWeight(Para_MinDetectionBoxLength)),
                                                    m_dWeightCohesion(GetWeight(Para_Weight_Cohesion)),
                                                    m_dWeightAlignment(GetWeight(Para_Weight_Alignment)),
                                                    m_dWeightSeparation(GetWeight(Para_Weight_Separation)),
                                                    m_dWeightObstacleAvoidance(GetWeight(Para_Weight_ObstacleAvoidance)),
                                                    m_dWeightWander(GetWeight(Para_Weight_Wander)),
                                                    m_dWeightWallAvoidance(GetWeight(Para_Weight_WallAvoidance)),
                                                    m_dWeightSeek(GetWeight(Para_Weight_Seek)),
                                                    m_dWeightFlee(GetWeight(Para_Weight_Flee)),
                                                    m_dWeightArrive(GetWeight(Para_Weight_Arrive)),
                                                    m_dWeightPursuit(GetWeight(Para_Weight_Pursuit)),
                                                    m_dWeightOffsetPursuit(GetWeight(Para_Weight_OffsetPursuit)),
                                                    m_dWeightInterpose(GetWeight(Para_Weight_Interpose)),
                                                    m_dWeightHide(GetWeight(Para_Weight_Hide)),
                                                    m_dWeightEvade(GetWeight(Para_Weight_Evade)),
                                                    m_dWeightFollowPath(GetWeight(Para_Weight_FollowPath)),                                                    
                                                    m_dViewDistance(Para_ViewDistance),
                                                    m_dWallDetectionFeelerLength(Para_WallDetectionFeelerLength),
                                                    m_Feelers(3),
                                                    m_Deceleration(normal),
                                                    m_pTargetAgent1(nullptr),
                                                    m_pTargetAgent2(nullptr),
                                                    m_dWanderDistance(Para_WanderDist),
                                                    m_dWanderJitter(Para_WanderJitterPerSec),
                                                    m_dWanderRadius(Para_WanderRadius),
                                                    m_dWaypointSeekDistSq(Para_WaypointSeekDist*Para_WaypointSeekDist),
                                                    m_SummingMethod(prioritized)
{
    //stuff for the wander behavior
    float theta = RandFloat_0_1() * _PI_*2;

    //create a vector to a target position on the wander circle
    m_vWanderTarget = Vector2D(m_dWanderRadius * std::cos(theta), m_dWanderRadius * std::sin(theta));

    //create a Path
    m_pPath = new Path(30, 0, 0, Win_Width, Win_Height, true);
    m_pPath->loopOn();
}

SteeringBehavior::~SteeringBehavior()
{
    delete m_pPath;
}




//----------------------- Calculate --------------------------------------
//
//  calculates the accumulated steering force according to the method set
//  in m_SummingMethod
//------------------------------------------------------------------------
Vector2D SteeringBehavior::calculate()
{ 
    //reset the steering force
    m_vSteeringForce.zero();

    //use space partitioning to calculate the neighbours of this vehicle
    //if switched on. If not, use the standard tagging system
    if (!m_pVehicle->getWorld()->isSpacePartitioningOn())
    {
        //tag neighbors if any of the following 3 group behaviors are switched on
        if (On(behavior_separation) || On(behavior_allignment) || On(behavior_cohesion))
        {
            m_pVehicle->getWorld()->tagNeighborsWithinViewRange(m_pVehicle, m_dViewDistance);
        }
    }
    else
    {
        //calculate neighbours in cell-space if any of the following 3 group
        //behaviors are switched on
        if (On(behavior_separation) || On(behavior_allignment) || On(behavior_cohesion))
        {
            m_pVehicle->getWorld()->getCellSpace()->calculateNeighbors(m_pVehicle->getPos(), m_dViewDistance);
        }
    }

    switch (m_SummingMethod)
    {
        case weighted_average:
            m_vSteeringForce = calculateWeightedSum(); 
            break;

        case prioritized:
            m_vSteeringForce = calculatePrioritized(); 
            break;

        case dithered:
            m_vSteeringForce = calculateDithered();
            break;

        default:
            m_vSteeringForce = Vector2D(0,0); 

    }//end switch

    return m_vSteeringForce;
}

//------------------------- ForwardComponent -----------------------------
//
//  returns the forward oomponent of the steering force
//------------------------------------------------------------------------
float SteeringBehavior::forwardComponent()
{
    return m_pVehicle->getHeading().dot(m_vSteeringForce);
}

//--------------------------- SideComponent ------------------------------
//  returns the side component of the steering force
//------------------------------------------------------------------------
float SteeringBehavior::sideComponent()
{
    return m_pVehicle->getSide().dot(m_vSteeringForce);
}


//--------------------- accumulateForce ----------------------------------
//
//  This function calculates how much of its max steering force the 
//  vehicle has left to apply and then applies that amount of the
//  force to add.
//------------------------------------------------------------------------
bool SteeringBehavior::accumulateForce(Vector2D &RunningTot, Vector2D ForceToAdd)
{
    //calculate how much steering force the vehicle has used so far
    float MagnitudeSoFar = RunningTot.length();

    //calculate how much steering force remains to be used by this vehicle
    float MagnitudeRemaining = m_pVehicle->getMaxForce() - MagnitudeSoFar;

    //return false if there is no more force left to use
    if (MagnitudeRemaining <= 0.0) return false;

    //calculate the magnitude of the force we want to add
    float MagnitudeToAdd = ForceToAdd.length();

    //if the magnitude of the sum of ForceToAdd and the running total
    //does not exceed the maximum force available to this vehicle, just
    //add together. Otherwise add as much of the ForceToAdd vector is
    //possible without going over the max.
    if (MagnitudeToAdd < MagnitudeRemaining)
    {
        RunningTot += ForceToAdd;
    }
    else
    {
        //add it to the steering force
        RunningTot += (Vec2Normalize(ForceToAdd) * MagnitudeRemaining); 
    }

    return true;
}



//---------------------- calculatePrioritized ----------------------------
//
//  this method calls each active steering behavior in order of priority
//  and acumulates their forces until the max steering force magnitude
//  is reached, at which time the function returns the steering force 
//  accumulated to that  point
//------------------------------------------------------------------------
Vector2D SteeringBehavior::calculatePrioritized()
{       
    Vector2D force;
  
    if (On(behavior_wall_avoidance))
    {
        force = wallAvoidance(m_pVehicle->getWorld()->getWalls()) * m_dWeightWallAvoidance;
        
        if (!accumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
    }
   
    if (On(behavior_obstacle_avoidance))
    {
        force = obstacleAvoidance(m_pVehicle->getWorld()->getObstacles()) * m_dWeightObstacleAvoidance;

        if (!accumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
    }

    if (On(behavior_evade))
    {
        assert(m_pTargetAgent1 && "Evade target not assigned");

        force = evade(m_pTargetAgent1) * m_dWeightEvade;

        if (!accumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
    }

    if (On(behavior_flee))
    {
        force = flee(m_pVehicle->getWorld()->getCrosshair()) * m_dWeightFlee;

        if (!accumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
    }


    //these next three can be combined for flocking behavior (wander is
    //also a good behavior to add into this mix)
    if (!m_pVehicle->getWorld()->isSpacePartitioningOn())
    {
        if (On(behavior_separation))
        {
            force = separation(m_pVehicle->getWorld()->getVehicles()) * m_dWeightSeparation;

            if (!accumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
        }

        if (On(behavior_allignment))
        {
            force = alignment(m_pVehicle->getWorld()->getVehicles()) * m_dWeightAlignment;

            if (!accumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
        }

        if (On(behavior_cohesion))
        {
            force = cohesion(m_pVehicle->getWorld()->getVehicles()) * m_dWeightCohesion;

            if (!accumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
        }
    }
    else
    {

        if (On(behavior_separation))
        {
            force = separationEx(m_pVehicle->getWorld()->getVehicles()) * m_dWeightSeparation;

            if (!accumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
        }

        if (On(behavior_allignment))
        {
            force = alignmentEx(m_pVehicle->getWorld()->getVehicles()) * m_dWeightAlignment;

            if (!accumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
        }

        if (On(behavior_cohesion))
        {
            force = cohesionEx(m_pVehicle->getWorld()->getVehicles()) * m_dWeightCohesion;

            if (!accumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
        }
    }

    if (On(behavior_seek))
    {
        force = seek(m_pVehicle->getWorld()->getCrosshair()) * m_dWeightSeek;

        if (!accumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
    }


    if (On(behavior_arrive))
    {
        force = arrive(m_pVehicle->getWorld()->getCrosshair(), m_Deceleration) * m_dWeightArrive;

        if (!accumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
    }

    if (On(behavior_wander))
    {
        force = wander() * m_dWeightWander;

        if (!accumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
    }

    if (On(behavior_pursuit))
    {
        assert(m_pTargetAgent1 && "pursuit target not assigned");

        force = pursuit(m_pTargetAgent1) * m_dWeightPursuit;

        if (!accumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
    }

    if (On(behavior_offset_pursuit))
    {
        assert (m_pTargetAgent1 && "pursuit target not assigned");
        assert (!m_vOffset.isZero() && "No offset assigned");

        force = offsetPursuit(m_pTargetAgent1, m_vOffset);

        if (!accumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
    }

    if (On(behavior_interpose))
    {
        assert (m_pTargetAgent1 && m_pTargetAgent2 && "Interpose agents not assigned");

        force = interpose(m_pTargetAgent1, m_pTargetAgent2) * m_dWeightInterpose;

        if (!accumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
    }

    if (On(behavior_hide))
    {
        assert(m_pTargetAgent1 && "Hide target not assigned");

        force = hide(m_pTargetAgent1, m_pVehicle->getWorld()->getObstacles()) * m_dWeightHide;

        if (!accumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
    }

    if (On(behavior_follow_path))
    {
        force = followPath() * m_dWeightFollowPath;

        if (!accumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
    }

    return m_vSteeringForce;
}


//---------------------- calculateWeightedSum ----------------------------
//
//  this simply sums up all the active behaviors X their weights and 
//  truncates the result to the max available steering force before 
//  returning
//------------------------------------------------------------------------
Vector2D SteeringBehavior::calculateWeightedSum()
{        
    if (On(behavior_wall_avoidance))
    {
        m_vSteeringForce += wallAvoidance(m_pVehicle->getWorld()->getWalls()) *m_dWeightWallAvoidance;
    }
   
    if (On(behavior_obstacle_avoidance))
    {
        m_vSteeringForce += obstacleAvoidance(m_pVehicle->getWorld()->getObstacles()) * m_dWeightObstacleAvoidance;
    }

    if (On(behavior_evade))
    {
        assert(m_pTargetAgent1 && "Evade target not assigned");

        m_vSteeringForce += evade(m_pTargetAgent1) * m_dWeightEvade;
    }


    //these next three can be combined for flocking behavior (wander is
    //also a good behavior to add into this mix)
    if (!m_pVehicle->getWorld()->isSpacePartitioningOn())
    {
        if (On(behavior_separation))
        {
            m_vSteeringForce += separation(m_pVehicle->getWorld()->getVehicles()) * m_dWeightSeparation;
        }

        if (On(behavior_allignment))
        {
            m_vSteeringForce += alignment(m_pVehicle->getWorld()->getVehicles()) * m_dWeightAlignment;
        }

        if (On(behavior_cohesion))
        {
            m_vSteeringForce += cohesion(m_pVehicle->getWorld()->getVehicles()) * m_dWeightCohesion;
        }
    }
    else
    {
        if (On(behavior_separation))
        {
            m_vSteeringForce += separationEx(m_pVehicle->getWorld()->getVehicles()) * m_dWeightSeparation;
        }

        if (On(behavior_allignment))
        {
            m_vSteeringForce += alignmentEx(m_pVehicle->getWorld()->getVehicles()) * m_dWeightAlignment;
        }

        if (On(behavior_cohesion))
        {
            m_vSteeringForce += cohesionEx(m_pVehicle->getWorld()->getVehicles()) * m_dWeightCohesion;
        }
    }


    if (On(behavior_wander))
    {
        m_vSteeringForce += wander() * m_dWeightWander;
    }

    if (On(behavior_seek))
    {
        m_vSteeringForce += seek(m_pVehicle->getWorld()->getCrosshair()) * m_dWeightSeek;
    }

    if (On(behavior_flee))
    {
        m_vSteeringForce += flee(m_pVehicle->getWorld()->getCrosshair()) * m_dWeightFlee;
    }

    if (On(behavior_arrive))
    {
        m_vSteeringForce += arrive(m_pVehicle->getWorld()->getCrosshair(), m_Deceleration) * m_dWeightArrive;
    }

    if (On(behavior_pursuit))
    {
        assert(m_pTargetAgent1 && "pursuit target not assigned");

        m_vSteeringForce += pursuit(m_pTargetAgent1) * m_dWeightPursuit;
    }

    if (On(behavior_offset_pursuit))
    {
        assert (m_pTargetAgent1 && "pursuit target not assigned");
        assert (!m_vOffset.isZero() && "No offset assigned");

        m_vSteeringForce += offsetPursuit(m_pTargetAgent1, m_vOffset) * m_dWeightOffsetPursuit;
    }

    if (On(behavior_interpose))
    {
        assert (m_pTargetAgent1 && m_pTargetAgent2 && "Interpose agents not assigned");

        m_vSteeringForce += interpose(m_pTargetAgent1, m_pTargetAgent2) * m_dWeightInterpose;
    }

    if (On(behavior_hide))
    {
        assert(m_pTargetAgent1 && "Hide target not assigned");

        m_vSteeringForce += hide(m_pTargetAgent1, m_pVehicle->getWorld()->getObstacles()) * m_dWeightHide;
    }

    if (On(behavior_follow_path))
    {
        m_vSteeringForce += followPath() * m_dWeightFollowPath;
    }

    m_vSteeringForce.truncate(m_pVehicle->getMaxForce());

    return m_vSteeringForce;
}


//---------------------- calculateDithered ----------------------------
//
//  this method sums up the active behaviors by assigning a probabilty
//  of being calculated to each behavior. It then tests the first priority
//  to see if it should be calcukated this simulation-step. If so, it
//  calculates the steering force resulting from this behavior. If it is
//  more than zero it returns the force. If zero, or if the behavior is
//  skipped it continues onto the next priority, and so on.
//
//  NOTE: Not all of the behaviors have been implemented in this method,
//        just a few, so you get the general idea
//------------------------------------------------------------------------
Vector2D SteeringBehavior::calculateDithered()
{  
    //reset the steering force
    m_vSteeringForce.zero();

    if (On(behavior_wall_avoidance) && RandFloat_0_1() < Para_Dither_WallAvoidance)
    {
        m_vSteeringForce = wallAvoidance(m_pVehicle->getWorld()->getWalls()) *
            m_dWeightWallAvoidance /Para_Dither_WallAvoidance;

        if (!m_vSteeringForce.isZero())
        {
            m_vSteeringForce.truncate(m_pVehicle->getMaxForce()); 
            return m_vSteeringForce;
        }
    }
   
    if (On(behavior_obstacle_avoidance) && RandFloat_0_1() < Para_Dither_ObstacleAvoidance)
    {
        m_vSteeringForce += obstacleAvoidance(m_pVehicle->getWorld()->getObstacles()) * 
        m_dWeightObstacleAvoidance / Para_Dither_ObstacleAvoidance;

        if (!m_vSteeringForce.isZero())
        {
            m_vSteeringForce.truncate(m_pVehicle->getMaxForce()); 
            return m_vSteeringForce;
        }
    }

    if (!m_pVehicle->getWorld()->isSpacePartitioningOn())
    {
        if (On(behavior_separation) && RandFloat_0_1() < Para_Dither_Separation)
        {
            m_vSteeringForce += separation(m_pVehicle->getWorld()->getVehicles()) * 
                      m_dWeightSeparation / Para_Dither_Separation;

            if (!m_vSteeringForce.isZero())
            {
                m_vSteeringForce.truncate(m_pVehicle->getMaxForce()); 
                return m_vSteeringForce;
            }
        }
    }
    else
    {
        if (On(behavior_separation) && RandFloat_0_1() < Para_Dither_Separation)
        {
            m_vSteeringForce += separationEx(m_pVehicle->getWorld()->getVehicles()) * 
                        m_dWeightSeparation / Para_Dither_Separation;

            if (!m_vSteeringForce.isZero())
            {
                m_vSteeringForce.truncate(m_pVehicle->getMaxForce()); 
                return m_vSteeringForce;
            }
        }
    }


    if (On(behavior_flee) && RandFloat_0_1() < Para_Dither_Flee)
    {
        m_vSteeringForce += flee(m_pVehicle->getWorld()->getCrosshair()) * m_dWeightFlee / Para_Dither_Flee;

        if (!m_vSteeringForce.isZero())
        {
            m_vSteeringForce.truncate(m_pVehicle->getMaxForce()); 

            return m_vSteeringForce;
        }
    }

    if (On(behavior_evade) && RandFloat_0_1() < Para_Dither_Evade)
    {
        assert(m_pTargetAgent1 && "Evade target not assigned");

        m_vSteeringForce += evade(m_pTargetAgent1) * m_dWeightEvade / Para_Dither_Evade;

        if (!m_vSteeringForce.isZero())
        {
            m_vSteeringForce.truncate(m_pVehicle->getMaxForce()); 
            return m_vSteeringForce;
        }
    }


    if (!m_pVehicle->getWorld()->isSpacePartitioningOn())
    {
        if (On(behavior_allignment) && RandFloat_0_1() < Para_Dither_Alignment)
        {
            m_vSteeringForce += alignment(m_pVehicle->getWorld()->getVehicles()) *
                                             m_dWeightAlignment / Para_Dither_Alignment;

            if (!m_vSteeringForce.isZero())
            {
                m_vSteeringForce.truncate(m_pVehicle->getMaxForce()); 

                return m_vSteeringForce;
            }
        }

        if (On(behavior_cohesion) && RandFloat_0_1() < Para_Dither_Cohesion)
        {
            m_vSteeringForce += cohesion(m_pVehicle->getWorld()->getVehicles()) * 
                          m_dWeightCohesion / Para_Dither_Cohesion;

            if (!m_vSteeringForce.isZero())
            {
                m_vSteeringForce.truncate(m_pVehicle->getMaxForce()); 
                return m_vSteeringForce;
            }
        }
    }
    else
    {
        if (On(behavior_allignment) && RandFloat_0_1() < Para_Dither_Alignment)
        {
            m_vSteeringForce += alignmentEx(m_pVehicle->getWorld()->getVehicles()) *
                          m_dWeightAlignment / Para_Dither_Alignment;

            if (!m_vSteeringForce.isZero())
            {
                m_vSteeringForce.truncate(m_pVehicle->getMaxForce()); 
                return m_vSteeringForce;
            }
        }

        if (On(behavior_cohesion) && RandFloat_0_1() < Para_Dither_Cohesion)
        {
            m_vSteeringForce += cohesionEx(m_pVehicle->getWorld()->getVehicles()) *
                          m_dWeightCohesion / Para_Dither_Cohesion;

            if (!m_vSteeringForce.isZero())
            {
                m_vSteeringForce.truncate(m_pVehicle->getMaxForce()); 
                return m_vSteeringForce;
            }
        }
    }

    if (On(behavior_wander) && RandFloat_0_1() < Para_Dither_Wander)
    {
        m_vSteeringForce += wander() * m_dWeightWander / Para_Dither_Wander;
        
        if (!m_vSteeringForce.isZero())
        {
            m_vSteeringForce.truncate(m_pVehicle->getMaxForce()); 
            return m_vSteeringForce;
        }
    }

    if (On(behavior_seek) && RandFloat_0_1() < Para_Dither_Seek)
    {
        m_vSteeringForce += seek(m_pVehicle->getWorld()->getCrosshair()) * m_dWeightSeek / Para_Dither_Seek;
        
        if (!m_vSteeringForce.isZero())
        {
            m_vSteeringForce.truncate(m_pVehicle->getMaxForce()); 
            return m_vSteeringForce;
        }
    }

    if (On(behavior_arrive) && RandFloat_0_1() < Para_Dither_Arrive)
    {
        m_vSteeringForce += arrive(m_pVehicle->getWorld()->getCrosshair(), m_Deceleration) * 
                        m_dWeightArrive / Para_Dither_Arrive;

        if (!m_vSteeringForce.isZero())
        {
            m_vSteeringForce.truncate(m_pVehicle->getMaxForce()); 
            return m_vSteeringForce;
        }
    }
 
    return m_vSteeringForce;
}





//------------------------------- Seek -----------------------------------
//
//  Given a target, this behavior returns a steering force which will
//  direct the agent towards the target
//------------------------------------------------------------------------
Vector2D SteeringBehavior::seek(Vector2D targetPos)
{
    Vector2D DesiredVelocity = Vec2Normalize(targetPos - m_pVehicle->getPos()) * m_pVehicle->getMaxSpeed();

    return (DesiredVelocity - m_pVehicle->getVelocity());
}

//----------------------------- Flee -------------------------------------
//
//  Does the opposite of Seek
//------------------------------------------------------------------------
Vector2D SteeringBehavior::flee(Vector2D targetPos)
{
    //only flee if the target is within 'panic distance'. Work in distance
    //squared space.
    /* const float PanicDistanceSq = 100.0f * 100.0;
        if (Vec2DistanceSq(m_pVehicle->getPos(), target) > PanicDistanceSq)
        {
            return Vector2D(0,0);
        }
    */

    Vector2D DesiredVelocity = Vec2Normalize(m_pVehicle->getPos() - targetPos) * m_pVehicle->getMaxSpeed();

    return (DesiredVelocity - m_pVehicle->getVelocity());
}

//--------------------------- Arrive -------------------------------------
//
//  This behavior is similar to seek but it attempts to arrive at the
//  target with a zero velocity
//------------------------------------------------------------------------
Vector2D SteeringBehavior::arrive(Vector2D targetPos, Deceleration deceleration)
{
    Vector2D ToTarget = targetPos - m_pVehicle->getPos();

    //calculate the distance to the target
    float dist = ToTarget.length();

    if (dist > 0)
    {
        //because Deceleration is enumerated as an int, this value is required
        //to provide fine tweaking of the deceleration..
        const float DecelerationTweaker = 0.3f;

        //calculate the speed required to reach the target given the desired deceleration
        float speed =  dist / ((float)deceleration * DecelerationTweaker);     

        //make sure the velocity does not exceed the max
        speed = std::min(speed, m_pVehicle->getMaxSpeed());

        //from here proceed just like Seek except we don't need to normalize 
        //the ToTarget vector because we have already gone to the trouble
        //of calculating its length: dist. 
        Vector2D DesiredVelocity =  ToTarget * speed / dist;
        return (DesiredVelocity - m_pVehicle->getVelocity());
    }

    return Vector2D(0,0);
}

//------------------------------ Pursuit ---------------------------------
//
//  this behavior creates a force that steers the agent towards the 
//  evader
//------------------------------------------------------------------------
Vector2D SteeringBehavior::pursuit(Vehicle* evader)
{
    //if the evader is ahead and facing the agent then we can just seek
    //for the evader's current position.
    Vector2D ToEvader = evader->getPos() - m_pVehicle->getPos();
    float RelativeHeading = m_pVehicle->getHeading().dot(evader->getHeading());

    if ( (ToEvader.dot(m_pVehicle->getHeading()) > 0) &&  (RelativeHeading < -0.95))  //acos(0.95)=18 degs
    {
        return seek(evader->getPos());
    }

    //Not considered ahead so we predict where the evader will be.

    //the lookahead time is propotional to the distance between the evader
    //and the pursuer; and is inversely proportional to the sum of the
    //agent's velocities
    float LookAheadTime = ToEvader.length() / (m_pVehicle->getMaxSpeed() + evader->getSpeed());

    //now seek to the predicted future position of the evader
    return seek(evader->getPos() + evader->getVelocity() * LookAheadTime);
}


//----------------------------- Evade ------------------------------------
//
//  similar to pursuit except the agent Flees from the estimated future
//  position of the pursuer
//------------------------------------------------------------------------
Vector2D SteeringBehavior::evade(Vehicle* pursuer)
{
    /* Not necessary to include the check for facing direction this time */
    Vector2D ToPursuer = pursuer->getPos() - m_pVehicle->getPos();

    //uncomment the following two lines to have Evade only consider pursuers 
    //within a 'threat range'
    const float ThreatRange = 100.0f;
    if (ToPursuer.lengthSq() > ThreatRange * ThreatRange) return Vector2D();

    //the lookahead time is propotional to the distance between the pursuer
    //and the pursuer; and is inversely proportional to the sum of the
    //agents' velocities
    float LookAheadTime = ToPursuer.length() /(m_pVehicle->getMaxSpeed() + pursuer->getSpeed());

    //now flee away from predicted future position of the pursuer
    return flee(pursuer->getPos() + pursuer->getVelocity() * LookAheadTime);
}


//--------------------------- Wander -------------------------------------
//
//  This behavior makes the agent wander about randomly
//------------------------------------------------------------------------
Vector2D SteeringBehavior::wander()
{ 
    //this behavior is dependent on the update rate, so this line must
    //be included when using time independent framerate.
    float JitterThisTimeSlice = m_dWanderJitter * m_pVehicle->getTimeElapsed();

    //first, add a small random vector to the target's position
    m_vWanderTarget += Vector2D(RandFloat_minus1_1() * JitterThisTimeSlice,
                                                    RandFloat_minus1_1() * JitterThisTimeSlice);

    //reproject this new vector back on to a unit circle
    m_vWanderTarget.normalize();

    //increase the length of the vector to the same as the radius
    //of the wander circle
    m_vWanderTarget *= m_dWanderRadius;

    //move the target into a position WanderDist in front of the agent
    Vector2D target = m_vWanderTarget + Vector2D(m_dWanderDistance, 0);

    //project the target into world space
    Vector2D Target = PointToWorldSpace(target,
                                                                   m_pVehicle->getHeading(),
                                                                   m_pVehicle->getSide(), 
                                                                   m_pVehicle->getPos());

    //and steer towards it
    return Target - m_pVehicle->getPos(); 
}


//---------------------- obstacleAvoidance -------------------------------
//
//  Given a vector of CObstacles, this method returns a steering force
//  that will prevent the agent colliding with the closest obstacle
//------------------------------------------------------------------------
Vector2D SteeringBehavior::obstacleAvoidance(const std::vector<Obstacle *>& obstacles)
{
    //the detection box length is proportional to the agent's velocity
    m_dDBoxLength = Para_MinDetectionBoxLength + 
                        (m_pVehicle->getSpeed()/m_pVehicle->getMaxSpeed()) * Para_MinDetectionBoxLength;

    //tag all obstacles within range of the box for processing
    m_pVehicle->getWorld()->tagObstaclesWithinViewRange(m_pVehicle, m_dDBoxLength);

    //this will keep track of the closest intersecting obstacle (CIB)
    BaseEntity* ClosestIntersectingObstacle = NULL;

    //this will be used to track the distance to the CIB
    float DistToClosestIP = FloatMax;

    //this will record the transformed local coordinates of the CIB
    Vector2D LocalPosOfClosestObstacle;

    std::vector<Obstacle *>::const_iterator curOb = obstacles.begin();

    while(curOb != obstacles.end())
    {
        //if the obstacle has been tagged within range proceed
        if ((*curOb)->isTag())
        {
              //calculate this obstacle's position in local space
              Vector2D LocalPos = PointToLocalSpace((*curOb)->getPos(),
                                                     m_pVehicle->getHeading(),
                                                     m_pVehicle->getSide(),
                                                     m_pVehicle->getPos());

              //if the local position has a negative x value then it must lay
              //behind the agent. (in which case it can be ignored)
              if (LocalPos.x >= 0)
              {
                    //if the distance from the x axis to the object's position is less
                    //than its radius + half the width of the detection box then there
                    //is a potential intersection.
                    float ExpandedRadius = (*curOb)->getBoundingRadius() + m_pVehicle->getBoundingRadius();

                    if (fabs(LocalPos.y) < ExpandedRadius)
                    {
                        //now to do a line/circle intersection test. The center of the 
                        //circle is represented by (cX, cY). The intersection points are 
                        //given by the formula x = cX +/-sqrt(r^2-cY^2) for y=0. 
                        //We only need to look at the smallest positive value of x because
                        //that will be the closest point of intersection.
                        float cX = LocalPos.x;
                        float cY = LocalPos.y;

                        //we only need to calculate the sqrt part of the above equation once
                        float SqrtPart = std::sqrt(ExpandedRadius*ExpandedRadius - cY*cY);

                        float ip = cX - SqrtPart;

                        if (ip <= 0.0)
                        {
                            ip = cX + SqrtPart;
                        }

                        //test to see if this is the closest so far. If it is keep a
                        //record of the obstacle and its local coordinates
                        if (ip < DistToClosestIP)
                        {
                            DistToClosestIP = ip;
                            ClosestIntersectingObstacle = *curOb;
                            LocalPosOfClosestObstacle = LocalPos;
                        }         
                    }
              }
        }

        ++curOb;
    }

    //if we have found an intersecting obstacle, calculate a steering 
    //force away from it
    Vector2D SteeringForce;

    if (ClosestIntersectingObstacle)
    {
        //the closer the agent is to an object, the stronger the 
        //steering force should be
        float multiplier = 1.0 + (m_dDBoxLength - LocalPosOfClosestObstacle.x) /m_dDBoxLength;

        //calculate the lateral force
        SteeringForce.y = (ClosestIntersectingObstacle->getBoundingRadius() - LocalPosOfClosestObstacle.y)  * multiplier;   

        //apply a braking force proportional to the obstacles distance from the vehicle. 
        const float BrakingWeight = 0.2f;

        SteeringForce.x = (ClosestIntersectingObstacle->getBoundingRadius() - LocalPosOfClosestObstacle.x) * BrakingWeight;
    }

    //finally, convert the steering vector from local to world space
    return VectorToWorldSpace(SteeringForce,
                            m_pVehicle->getHeading(),
                            m_pVehicle->getSide());
}


//--------------------------- wallAvoidance --------------------------------
//
//  This returns a steering force that will keep the agent away from any
//  walls it may encounter
//------------------------------------------------------------------------
Vector2D SteeringBehavior::wallAvoidance(const std::vector<Wall *>& walls)
{
    //the feelers are contained in a std::vector, m_Feelers
    createFeelers();

    float DistToThisIP    = 0.0;
    float DistToClosestIP = FloatMax;

    //this will hold an index into the vector of walls
    int ClosestWall = -1;

    Vector2D SteeringForce,
                    point,         //used for storing temporary info
                    ClosestPoint;  //holds the closest intersection point

    //examine each feeler in turn
    for (unsigned int flr=0; flr<m_Feelers.size(); ++flr)
    {
        //run through each wall checking for any intersection points
        for (unsigned int w=0; w<walls.size(); ++w)
        {
            if (lineIntersection2D(m_pVehicle->getPos(),
                                                 m_Feelers[flr],
                                                 walls[w]->from(),
                                                 walls[w]->to(),
                                                 DistToThisIP,
                                                 point))
            {
                //is this the closest found so far? If so keep a record
                if (DistToThisIP < DistToClosestIP)
                {
                    DistToClosestIP = DistToThisIP;
                    ClosestWall = w;
                    ClosestPoint = point;
                }
            }
        }//next wall

        //if an intersection point has been detected, calculate a force  
        //that will direct the agent away
        if (ClosestWall >=0)
        {
            //calculate by what distance the projected position of the agent
            //will overshoot the wall
            Vector2D OverShoot = m_Feelers[flr] - ClosestPoint;

            //create a force in the direction of the wall normal, with a 
            //magnitude of the overshoot
            SteeringForce = walls[ClosestWall]->normal() * OverShoot.length();
        }
    }//next feeler

    return SteeringForce;
}

//------------------------------- createFeelers --------------------------
//
//  Creates the antenna utilized by wallAvoidance
//------------------------------------------------------------------------
void SteeringBehavior::createFeelers()
{
    //feeler pointing straight in front
    m_Feelers[0] = m_pVehicle->getPos() +  m_pVehicle->getHeading() * m_dWallDetectionFeelerLength;

    //feeler to left
    Vector2D temp = m_pVehicle->getHeading();
    Vec2DRotateAroundOrigin(temp, _PI_*0.5 * 3.5f);
    m_Feelers[1] = m_pVehicle->getPos() + temp * m_dWallDetectionFeelerLength/2.0f ;

    //feeler to right
    temp = m_pVehicle->getHeading();
    Vec2DRotateAroundOrigin(temp, _PI_*0.5 * 0.5f);
    m_Feelers[2] = m_pVehicle->getPos() + temp * m_dWallDetectionFeelerLength/2.0f;
}


//---------------------------- separation --------------------------------
//
// this calculates a force repelling from the other neighbors
//------------------------------------------------------------------------
Vector2D SteeringBehavior::separation(const vector<Vehicle*> &neighbors)
{  
    Vector2D SteeringForce;

    for (unsigned int a=0; a<neighbors.size(); ++a)
    {
        //make sure this agent isn't included in the calculations and that
        //the agent being examined is close enough. ***also make sure it doesn't
        //include the evade target ***
        if((neighbors[a] != m_pVehicle) && neighbors[a]->isTag() &&(neighbors[a] != m_pTargetAgent1))
        {
            Vector2D ToAgent = m_pVehicle->getPos() - neighbors[a]->getPos();

            //scale the force inversely proportional to the agents distance  
            //from its neighbor.
            SteeringForce += Vec2Normalize(ToAgent)/(ToAgent.length()+0.1);
        }
    }
    return SteeringForce;
}


//---------------------------- Alignment ---------------------------------
//
//  returns a force that attempts to align this agents heading with that
//  of its neighbors
//------------------------------------------------------------------------
Vector2D SteeringBehavior::alignment(const vector<Vehicle*>& neighbors)
{
    //used to record the average heading of the neighbors
    Vector2D AverageHeading;

    //used to count the number of vehicles in the neighborhood
    int NeighborCount = 0;

    //iterate through all the tagged vehicles and sum their heading vectors  
    for (unsigned int a=0; a<neighbors.size(); ++a)
    {
        //make sure *this* agent isn't included in the calculations and that
        //the agent being examined  is close enough ***also make sure it doesn't
        //include any evade target ***
        if((neighbors[a] != m_pVehicle) && neighbors[a]->isTag() && (neighbors[a] != m_pTargetAgent1))
        {
            AverageHeading += neighbors[a]->getHeading();
            ++NeighborCount;
        }
    }

    //if the neighborhood contained one or more vehicles, average their
    //heading vectors.
    if (NeighborCount > 0)
    {
        AverageHeading /= (float)NeighborCount;
        AverageHeading -= m_pVehicle->getHeading();
    }
    
    return AverageHeading;
}

//-------------------------------- Cohesion ------------------------------
//
//  returns a steering force that attempts to move the agent towards the
//  center of mass of the agents in its immediate area
//------------------------------------------------------------------------
Vector2D SteeringBehavior::cohesion(const vector<Vehicle*> &neighbors)
{
    //first find the center of mass of all the agents
    Vector2D CenterOfMass, SteeringForce;
    int NeighborCount = 0;

    //iterate through the neighbors and sum up all the position vectors
    for (unsigned int a=0; a<neighbors.size(); ++a)
    {
        //make sure *this* agent isn't included in the calculations and that
        //the agent being examined is close enough ***also make sure it doesn't
        //include the evade target ***
        if((neighbors[a] != m_pVehicle) && neighbors[a]->isTag() && (neighbors[a] != m_pTargetAgent1))
        {
            CenterOfMass += neighbors[a]->getPos();
            ++NeighborCount;
        }
    }

    if (NeighborCount > 0)
    {
        //the center of mass is the average of the sum of positions
        CenterOfMass /= (float)NeighborCount;
        //now seek towards that position
        SteeringForce = seek(CenterOfMass);
    }

    //the magnitude of cohesion is usually much larger than separation or
    //allignment so it usually helps to normalize it.
    return Vec2Normalize(SteeringForce);
}


/* NOTE: the next three behaviors are the same as the above three, except
          that they use a cell-space partition to find the neighbors
*/


//---------------------------- separation --------------------------------
//
// this calculates a force repelling from the other neighbors
//
//  USES SPACIAL PARTITIONING
//------------------------------------------------------------------------
Vector2D SteeringBehavior::separationEx(const vector<Vehicle*> &neighbors)
{  
    Vector2D SteeringForce;

    //iterate through the neighbors and sum up all the position vectors
    for (BaseEntity* pV = m_pVehicle->getWorld()->getCellSpace()->begin();
                     !m_pVehicle->getWorld()->getCellSpace()->end();     
                   pV = m_pVehicle->getWorld()->getCellSpace()->next())
    {    
        //make sure this agent isn't included in the calculations and that
        //the agent being examined is close enough
        if(pV != m_pVehicle)
        {
            Vector2D ToAgent = m_pVehicle->getPos() - pV->getPos();

            //scale the force inversely proportional to the agents distance from its neighbor.
            SteeringForce += Vec2Normalize(ToAgent)/(ToAgent.length()+0.1);
        }
    }

    return SteeringForce;
}
//---------------------------- Alignment ---------------------------------
//
//  returns a force that attempts to align this agents heading with that
//  of its neighbors
//
//  USES SPACIAL PARTITIONING
//------------------------------------------------------------------------
Vector2D SteeringBehavior::alignmentEx(const vector<Vehicle*> &neighbors)
{
    //This will record the average heading of the neighbors
    Vector2D AverageHeading;

    //This count the number of vehicles in the neighborhood
    float NeighborCount = 0.0;

    //iterate through the neighbors and sum up all the position vectors
    for (MovingEntity* pV = m_pVehicle->getWorld()->getCellSpace()->begin();
                         !m_pVehicle->getWorld()->getCellSpace()->end();     
                        pV = m_pVehicle->getWorld()->getCellSpace()->next())
    {
        //make sure *this* agent isn't included in the calculations and that
        //the agent being examined  is close enough
        if(pV != m_pVehicle)
        {
            AverageHeading += pV->getHeading();
            ++NeighborCount;
        }
    }

    //if the neighborhood contained one or more vehicles, average their
    //heading vectors.
    if (NeighborCount > 0.0)
    {
        AverageHeading /= NeighborCount;
        AverageHeading -= m_pVehicle->getHeading();
    }

    return AverageHeading;
}


//-------------------------------- Cohesion ------------------------------
//
//  returns a steering force that attempts to move the agent towards the
//  center of mass of the agents in its immediate area
//
//  USES SPACIAL PARTITIONING
//------------------------------------------------------------------------
Vector2D SteeringBehavior::cohesionEx(const vector<Vehicle*> &neighbors)
{
    //first find the center of mass of all the agents
    Vector2D CenterOfMass, SteeringForce;
    int NeighborCount = 0;

    //iterate through the neighbors and sum up all the position vectors
    for (BaseEntity* pV = m_pVehicle->getWorld()->getCellSpace()->begin();
                        !m_pVehicle->getWorld()->getCellSpace()->end();     
                        pV = m_pVehicle->getWorld()->getCellSpace()->next())
    {
        //make sure *this* agent isn't included in the calculations and that
        //the agent being examined is close enough
        if(pV != m_pVehicle)
        {
            CenterOfMass += pV->getPos();
            ++NeighborCount;
        }
    }

    if (NeighborCount > 0)
    {
        //the center of mass is the average of the sum of positions
        CenterOfMass /= (float)NeighborCount;

        //now seek towards that position
        SteeringForce = seek(CenterOfMass);
    }

    //the magnitude of cohesion is usually much larger than separation or
    //allignment so it usually helps to normalize it.
    return Vec2Normalize(SteeringForce);
}


//--------------------------- Interpose ----------------------------------
//
//  Given two agents, this method returns a force that attempts to 
//  position the vehicle between them
//------------------------------------------------------------------------
Vector2D SteeringBehavior::interpose(Vehicle* VehicleA, Vehicle* VehicleB)
{
    //first we need to figure out where the two agents are going to be at 
    //time T in the future. This is approximated by determining the time
    //taken to reach the mid way point at the current time at at max speed.
    Vector2D MidPoint = (VehicleA->getPos() + VehicleB->getPos()) / 2.0;

    float TimeToReachMidPoint = Vec2Distance(m_pVehicle->getPos(), MidPoint) /
                               m_pVehicle->getMaxSpeed();

    //now we have T, we assume that agent A and agent B will continue on a
    //straight trajectory and extrapolate to get their future positions
    Vector2D APos = VehicleA->getPos() + VehicleA->getVelocity() * TimeToReachMidPoint;
    Vector2D BPos = VehicleB->getPos() + VehicleB->getVelocity() * TimeToReachMidPoint;

    //calculate the mid point of these predicted positions
    MidPoint = (APos + BPos) / 2.0;

    //then steer to Arrive at it
    return arrive(MidPoint, fast);
}

//--------------------------- Hide ---------------------------------------
//
//------------------------------------------------------------------------
Vector2D SteeringBehavior::hide(Vehicle* hunter, const std::vector<Obstacle *>& obstacles)
{
    float DistToClosest = FloatMax;
    Vector2D BestHidingSpot;

    std::vector<Obstacle *>::const_iterator curOb = obstacles.begin();
    std::vector<Obstacle *>::const_iterator closest;

    while(curOb != obstacles.end())
    {
        //calculate the position of the hiding spot for this obstacle
        Vector2D HidingSpot = getHidingPosition((*curOb)->getPos(),
                                             (*curOb)->getBoundingRadius(),
                                              hunter->getPos());
            
        //work in distance-squared space to find the closest hiding
        //spot to the agent
        float dist = Vec2DistanceSq(HidingSpot, m_pVehicle->getPos());

        if (dist < DistToClosest)
        {
            DistToClosest = dist;
            BestHidingSpot = HidingSpot;
            closest = curOb;
        }  
        ++curOb;
    }//end while
  
    //if no suitable obstacles found then Evade the hunter
    if (DistToClosest == FloatMax)
    {
        return evade(hunter);
    }

    //else use Arrive on the hiding spot
    return arrive(BestHidingSpot, fast);
}

//------------------------- GetHidingPosition ----------------------------
//
//  Given the position of a hunter, and the position and radius of
//  an obstacle, this method calculates a position DistanceFromBoundary 
//  away from its bounding radius and directly opposite the hunter
//------------------------------------------------------------------------
Vector2D SteeringBehavior::getHidingPosition(const Vector2D& posOb, 
                                                                            const float radiusOb,
                                                                            const Vector2D& posHunter)
{
    //calculate how far away the agent is to be from the chosen obstacle's
    //bounding radius
    const float DistanceFromBoundary = 30.0;
    float DistAway = radiusOb + DistanceFromBoundary;

    //calculate the heading toward the object from the hunter
    Vector2D ToOb = Vec2Normalize(posOb - posHunter);

    //scale it to size and add to the obstacles position to get
    //the hiding spot.
    return (ToOb * DistAway) + posOb;
}


//------------------------------- FollowPath -----------------------------
//
//  Given a series of Vector2Ds, this method produces a force that will
//  move the agent along the waypoints in order. The agent uses the
// 'Seek' behavior to move to the next waypoint - unless it is the last
//  waypoint, in which case it 'Arrives'
//------------------------------------------------------------------------
Vector2D SteeringBehavior::followPath()
{ 
    //move to next target if close enough to current target (working in
    //distance squared space)
    if(Vec2DistanceSq(m_pPath->currentWaypoint(), m_pVehicle->getPos()) < m_dWaypointSeekDistSq)
    {
        m_pPath->setNextWaypoint();
    }
    
    if (!m_pPath->isFinished())
    {
        return seek(m_pPath->currentWaypoint());
    }

    else
    {
        return arrive(m_pPath->currentWaypoint(), normal);
    }
}

//------------------------- Offset Pursuit -------------------------------
//
//  Produces a steering force that keeps a vehicle at a specified offset
//  from a leader vehicle
//------------------------------------------------------------------------
Vector2D SteeringBehavior::offsetPursuit(Vehicle* leader, const Vector2D offset)
{
    //calculate the offset's position in world space
    Vector2D WorldOffsetPos = PointToWorldSpace(offset,
                                                  leader->getHeading(),
                                                  leader->getSide(),
                                                  leader->getPos());

    Vector2D ToOffset = WorldOffsetPos - m_pVehicle->getPos();

    //the lookahead time is propotional to the distance between the leader
    //and the pursuer; and is inversely proportional to the sum of both
    //agent's velocities
    float LookAheadTime = ToOffset.length() / 
                        (m_pVehicle->getMaxSpeed() + leader->getSpeed());

    //now Arrive at the predicted future position of the offset
    return arrive(WorldOffsetPos + leader->getVelocity() * LookAheadTime, fast);
}


