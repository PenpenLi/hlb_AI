#include "Raven_SteeringBehaviors.h"
#include "Raven_Bot.h"
#include "common/game/Wall.h"
#include "common/2D/Transformations.h"
#include "common/2D/Geometry.h"
#include "GameWorldRaven.h"
#include "Raven_Map.h"
#include <cassert>


using std::string;
using std::vector;


//------------------------- ctor -----------------------------------------
//
//------------------------------------------------------------------------
Raven_SteeringBehaviors::Raven_SteeringBehaviors(GameWorldRaven* world, Raven_Bot* agent):
                                                                                            m_pWorld(world),
                                                                                            m_pRaven_Bot(agent),
                                                                                            m_iFlags(0),
                                                                                            m_dWeightSeparation(Para_SeparationWeight),
                                                                                            m_dWeightWander(Para_WanderWeight),
                                                                                            m_dWeightWallAvoidance(Para_WallAvoidanceWeight),
                                                                                            m_dViewDistance(Para_ViewDistance),
                                                                                            m_dWallDetectionFeelerLength(Para_WallDetectionFeelerLength),
                                                                                            m_Feelers(3),
                                                                                            m_Deceleration(normal),
                                                                                            m_pTargetAgent1(NULL),
                                                                                            m_pTargetAgent2(NULL),
                                                                                            m_dWanderDistance(WanderDist),
                                                                                            m_dWanderJitter(WanderJitterPerSec),
                                                                                            m_dWanderRadius(WanderRad),
                                                                                            m_dWeightSeek(Para_SeekWeight),
                                                                                            m_dWeightArrive(Para_ArriveWeight),
                                                                                            m_bCellSpaceOn(false),
                                                                                            m_SummingMethod(prioritized)
{
    //stuff for the wander behavior
    float theta = RandFloat_0_1() * 2*_PI_;

    //create a vector to a target position on the wander circle
    m_vWanderTarget = Vector2D(m_dWanderRadius * cos(theta), m_dWanderRadius * sin(theta)); 
}

//---------------------------------dtor ----------------------------------
Raven_SteeringBehaviors::~Raven_SteeringBehaviors(){}


/////////////////////////////////////////////////////////////////////////////// CALCULATE METHODS 


//----------------------- calculate --------------------------------------
//
//  calculates the accumulated steering force according to the method set
//  in m_SummingMethod
//------------------------------------------------------------------------
Vector2D Raven_SteeringBehaviors::calculate()
{ 
    //reset the steering force
    m_vSteeringForce.Zero();

    //tag neighbors if any of the following 3 group behaviors are switched on
    if (On(behavior_separation))
    {
        m_pWorld->tagRaven_BotsWithinViewRange(m_pRaven_Bot, m_dViewDistance);
    }

    m_vSteeringForce = calculatePrioritized();

    return m_vSteeringForce;
}

//------------------------- ForwardComponent -----------------------------
//
//  returns the forward oomponent of the steering force
//------------------------------------------------------------------------
float Raven_SteeringBehaviors::getForwardComponent()
{
    return m_pRaven_Bot->getHeading().dot(m_vSteeringForce);
}

//--------------------------- SideComponent ------------------------------
//  returns the side component of the steering force
//------------------------------------------------------------------------
float Raven_SteeringBehaviors::getSideComponent()
{
    return m_pRaven_Bot->getSide().dot(m_vSteeringForce);
}


//--------------------- accumulateForce ----------------------------------
//
//  This function calculates how much of its max steering force the 
//  vehicle has left to apply and then applies that amount of the
//  force to add.
//------------------------------------------------------------------------
bool Raven_SteeringBehaviors::accumulateForce(Vector2D &RunningTot,Vector2D ForceToAdd)
{  
    //calculate how much steering force the vehicle has used so far
    float MagnitudeSoFar = RunningTot.length();

    //calculate how much steering force remains to be used by this vehicle
    float MagnitudeRemaining = m_pRaven_Bot->getMaxForce() - MagnitudeSoFar;

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
        MagnitudeToAdd = MagnitudeRemaining;
        //add it to the steering force
        RunningTot += (Vec2Normalize(ForceToAdd) * MagnitudeToAdd); 
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
Vector2D Raven_SteeringBehaviors::calculatePrioritized()
{       
    Vector2D force;

    if (On(behavior_wall_avoidance))
    {
        force = wallAvoidance(m_pWorld->GetMap()->GetWalls()) * m_dWeightWallAvoidance;

        if (!accumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
    }

 
    //these next three can be combined for flocking behavior (wander is
    //also a good behavior to add into this mix)

    if (On(behavior_separation))
    {
        force = separation(m_pWorld->GetAllBots()) * m_dWeightSeparation;

        if (!accumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
    }


    if (On(behavior_seek))
    {
        force = seek(m_vTarget) * m_dWeightSeek;

        if (!accumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
    }


    if (On(behavior_arrive))
    {
        force = arrive(m_vTarget, m_Deceleration) * m_dWeightArrive;

        if (!accumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
    }

    if (On(behavior_wander))
    {
        force = wander() * m_dWeightWander;

        if (!accumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
    }


    return m_vSteeringForce;
}


/////////////////////////////////////////////////////////////////////////////// START OF BEHAVIORS

//------------------------------- Seek -----------------------------------
//
//  Given a target, this behavior returns a steering force which will
//  direct the agent towards the target
//------------------------------------------------------------------------
Vector2D Raven_SteeringBehaviors::seek(const Vector2D &target)
{
    Vector2D DesiredVelocity = Vec2Normalize(target - m_pRaven_Bot->getPos()) * m_pRaven_Bot->getMaxSpeed();

    return (DesiredVelocity - m_pRaven_Bot->getVelocity());
}


//--------------------------- Arrive -------------------------------------
//
//  This behavior is similar to seek but it attempts to arrive at the
//  target with a zero velocity
//------------------------------------------------------------------------
Vector2D Raven_SteeringBehaviors::arrive(const Vector2D& target, const Deceleration deceleration)
{
    Vector2D ToTarget = target - m_pRaven_Bot->getPos();

    //calculate the distance to the target
    float dist = ToTarget.length();

    if (dist > 0)
    {
        //because Deceleration is enumerated as an int, this value is required
        //to provide fine tweaking of the deceleration..
        const float DecelerationTweaker = 0.3f;

        //calculate the speed required to reach the target given the desired
        //deceleration
        float speed = dist /((float)deceleration * DecelerationTweaker);     

        //make sure the velocity does not exceed the max
        speed = std::min(speed, m_pRaven_Bot->getMaxSpeed());

        //from here proceed just like Seek except we don't need to normalize 
        //the ToTarget vector because we have already gone to the trouble
        //of calculating its length: dist. 
        Vector2D DesiredVelocity =  ToTarget * speed / dist;

        return (DesiredVelocity - m_pRaven_Bot->getVelocity());
    }

    return Vector2D(0,0);
}



//--------------------------- Wander -------------------------------------
//
//  This behavior makes the agent wander about randomly
//------------------------------------------------------------------------
Vector2D Raven_SteeringBehaviors::wander()
{ 
    //first, add a small random vector to the target's position
    m_vWanderTarget += Vector2D(RandFloat_minus1_1() * m_dWanderJitter,
                                                    RandFloat_minus1_1() * m_dWanderJitter);

    //reproject this new vector back on to a unit circle
    m_vWanderTarget.normalize();

    //increase the length of the vector to the same as the radius
    //of the wander circle
    m_vWanderTarget *= m_dWanderRadius;

    //move the target into a position WanderDist in front of the agent
    Vector2D target = m_vWanderTarget + Vector2D(m_dWanderDistance, 0);

    //project the target into world space
    Vector2D Target = PointToWorldSpace(target,
                                                                   m_pRaven_Bot->getHeading(),
                                                                   m_pRaven_Bot->getSide(), 
                                                                   m_pRaven_Bot->getPos());

    //and steer towards it
    return Target - m_pRaven_Bot->getPos(); 
}


//--------------------------- wallAvoidance --------------------------------
//
//  This returns a steering force that will keep the agent away from any
//  walls it may encounter
//------------------------------------------------------------------------
Vector2D Raven_SteeringBehaviors::wallAvoidance(const vector<Wall2D*> &walls)
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
            if (lineIntersection2D(m_pRaven_Bot->getPos(),
                                                 m_Feelers[flr],
                                                 walls[w]->From(),
                                                 walls[w]->To(),
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

//------------------------------- CreateFeelers --------------------------
//
//  Creates the antenna utilized by wallAvoidance
//------------------------------------------------------------------------
void Raven_SteeringBehaviors::createFeelers()
{
    //feeler pointing straight in front
    m_Feelers[0] = m_pRaven_Bot->getPos() + m_dWallDetectionFeelerLength *
                            m_pRaven_Bot->getHeading() * m_pRaven_Bot->getSpeed();

    //feeler to left
    Vector2D temp = m_pRaven_Bot->getHeading();
    Vec2DRotateAroundOrigin(temp, _PI_*0.5 * 3.5);
    m_Feelers[1] = m_pRaven_Bot->getPos() + m_dWallDetectionFeelerLength/2.0 * temp;

    //feeler to right
    temp = m_pRaven_Bot->getHeading();
    Vec2DRotateAroundOrigin(temp, _PI_*0.5 * 0.5);
    m_Feelers[2] = m_pRaven_Bot->getPos() + m_dWallDetectionFeelerLength/2.0 * temp;
}


//---------------------------- separation --------------------------------
//
// this calculates a force repelling from the other neighbors
//------------------------------------------------------------------------
Vector2D Raven_SteeringBehaviors::separation(const std::list<Raven_Bot*>& neighbors)
{  
    //iterate through all the neighbors and calculate the vector from the
    Vector2D SteeringForce;

    std::list<Raven_Bot*>::const_iterator it = neighbors.begin();
    for (it; it != neighbors.end(); ++it)
    {
        //make sure this agent isn't included in the calculations and that
        //the agent being examined is close enough. ***also make sure it doesn't
        //include the evade target ***
        if((*it != m_pRaven_Bot) && (*it)->isTagged() &&(*it != m_pTargetAgent1))
        {
            Vector2D ToAgent = m_pRaven_Bot->getPos() - (*it)->getPos();

            //scale the force inversely proportional to the agents distance  
            //from its neighbor.
            SteeringForce += Vec2Normalize(ToAgent)/ToAgent.length();
        }
    }

    return SteeringForce;
}


