#include "ParaConfigSoccer.h"
#include "SteeringBehaviors_Soccer.h"
#include "PlayerBase.h"
#include "SoccerTeam.h"
#include "common/misc/Autolist.h"
#include "SoccerBall.h"
#include <algorithm> //max , min 

using std::string;
using std::vector;

//------------------------- ctor -----------------------------------------
//
//------------------------------------------------------------------------
SteeringBehaviors_Soccer::SteeringBehaviors_Soccer(PlayerBase* agent,
                                                                SoccerPitch* world,
                                                                SoccerBall* ball):  m_pPlayer(agent),
                                                                                            m_iFlags(0),
                                                                                            m_dMultSeparation(Para_SeparationCoefficient),
                                                                                            m_bTagged(false),
                                                                                            m_dViewDistance(Para_ViewDistance),
                                                                                            m_pBall(ball),
                                                                                            m_dInterposeDist(0.0),
                                                                                            m_Antenna(5,Vector2D())
{
}

//--------------------- accumulateForce ----------------------------------
//
//  This function calculates how much of its max steering force the 
//  vehicle has left to apply and then applies that amount of the
//  force to add.
//------------------------------------------------------------------------
bool SteeringBehaviors_Soccer::accumulateForce(Vector2D &sf, Vector2D ForceToAdd)
{
    //first calculate how much steering force we have left to use
    float MagnitudeSoFar = sf.length();

    float magnitudeRemaining = m_pPlayer->getMaxForce() - MagnitudeSoFar;

    //return false if there is no more force left to use
    if (magnitudeRemaining <= 0.0) return false;

    //calculate the magnitude of the force we want to add
    float MagnitudeToAdd = ForceToAdd.length();
  
    //now calculate how much of the force we can really add  
    if (MagnitudeToAdd > magnitudeRemaining)
    {
        MagnitudeToAdd = magnitudeRemaining;
    }
    
    //add it to the steering force
    sf += (Vec2Normalize(ForceToAdd) * MagnitudeToAdd); 
  
    return true;
}

//---------------------- Calculate ---------------------------------------
//
//  calculates the overall steering force based on the currently active
//  steering behaviors. 
//------------------------------------------------------------------------
Vector2D SteeringBehaviors_Soccer::calculate()
{                                                                         
    //reset the force
    m_vSteeringForce.zero();

    //this will hold the value of each individual steering force
    m_vSteeringForce = sumForces();

    //make sure the force doesn't exceed the vehicles maximum allowable
    m_vSteeringForce.truncate(m_pPlayer->getMaxForce());

    return m_vSteeringForce;
}

//-------------------------- sumForces -----------------------------------
//
//  this method calls each active steering behavior and acumulates their
//  forces until the max steering force magnitude is reached at which
//  time the function returns the steering force accumulated to that point
//------------------------------------------------------------------------
Vector2D SteeringBehaviors_Soccer::sumForces()
{
    Vector2D force;

    //the soccer players must always tag their neighbors
    findNeighbours();

    if (On(behavior_separation))
    {
        force += separation() * m_dMultSeparation;

        if (!accumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
    }    

    if (On(behavior_seek))
    {
        force += seek(m_vTarget);

        if (!accumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
    }

    if (On(behavior_arrive))
    {
        force += arrive(m_vTarget, fast);

        if (!accumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
    }

    if (On(behavior_pursuit))
    {
        force += pursuit(m_pBall);

        if (!accumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
    }

    if (On(behavior_interpose))
    {
        force += interpose(m_pBall, m_vTarget, m_dInterposeDist);

        if (!accumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
    }

    return m_vSteeringForce;
}

//------------------------- ForwardComponent -----------------------------
//
//  calculates the forward component of the steering force
//------------------------------------------------------------------------
float SteeringBehaviors_Soccer::forwardComponent()
{
    return m_pPlayer->getHeading().dot(m_vSteeringForce);
}

//--------------------------- SideComponent ------------------------------
//
//  //  calculates the side component of the steering force
//------------------------------------------------------------------------
float SteeringBehaviors_Soccer::sideComponent()
{
    return m_pPlayer->getSide().dot(m_vSteeringForce) * m_pPlayer->getMaxTurnRate();
}


//------------------------------- seek -----------------------------------
//
//  Given a target, this behavior returns a steering force which will
//  allign the agent with the target and move the agent in the desired
//  direction
//------------------------------------------------------------------------
Vector2D SteeringBehaviors_Soccer::seek(Vector2D target)
{
    Vector2D DesiredVelocity = Vec2Normalize(target - m_pPlayer->getPos())
                                                * m_pPlayer->getMaxSpeed();

    return (DesiredVelocity - m_pPlayer->getVelocity());
}


//--------------------------- arrive -------------------------------------
//
//  This behavior is similar to seek but it attempts to arrive at the
//  target with a zero velocity
//------------------------------------------------------------------------
Vector2D SteeringBehaviors_Soccer::arrive(Vector2D target, Deceleration deceleration)
{
    Vector2D ToTarget = target - m_pPlayer->getPos();

    //calculate the distance to the target
    float dist = ToTarget.length();

    if (dist > 0)
    {
        //because Deceleration is enumerated as an int, this value is required
        //to provide fine tweaking of the deceleration..
        const float DecelerationTweaker = 0.3f;

        //calculate the speed required to reach the target given the desired
        //deceleration
        float speed =  dist / ((float)deceleration * DecelerationTweaker);                    

        //make sure the velocity does not exceed the max
        speed = std::min(speed, m_pPlayer->getMaxSpeed());

        //from here proceed just like seek except we don't need to normalize 
        //the ToTarget vector because we have already gone to the trouble
        //of calculating its length: dist. 
        Vector2D DesiredVelocity =  ToTarget * speed / dist;

        return (DesiredVelocity - m_pPlayer->getVelocity());
    }

    return Vector2D(0,0);
}


//------------------------------ pursuit ---------------------------------
//
//  this behavior creates a force that steers the agent towards the 
//  ball
//------------------------------------------------------------------------
Vector2D SteeringBehaviors_Soccer::pursuit(const SoccerBall* ball)
{
    Vector2D ToBall = ball->getPos() - m_pPlayer->getPos();

    //the lookahead time is proportional to the distance between the ball
    //and the pursuer; 
    float LookAheadTime = 0.0;

    if (ball->getSpeed() != 0.0)
    {
        LookAheadTime = ToBall.length() / ball->getSpeed();
    }

    //calculate where the ball will be at this time in the future
    m_vTarget = ball->futurePosition(LookAheadTime);

    //now seek to the predicted future position of the ball
    return arrive(m_vTarget, fast);
}


//-------------------------- FindNeighbours ------------------------------
//
//  tags any vehicles within a predefined radius
//------------------------------------------------------------------------
void SteeringBehaviors_Soccer::findNeighbours()
{
    std::list<PlayerBase*>& AllPlayers = AutoList<PlayerBase>::getAllMembers();
    std::list<PlayerBase*>::iterator curPlyr;
    for (curPlyr = AllPlayers.begin(); curPlyr!=AllPlayers.end(); ++curPlyr)
    {
        //first clear any current tag
        (*curPlyr)->getSteering()->unTag();

        //work in distance squared to avoid sqrts
        Vector2D to = (*curPlyr)->getPos() - m_pPlayer->getPos();

        if (to.lengthSq() < (m_dViewDistance * m_dViewDistance))
        {
            (*curPlyr)->getSteering()->tag();
        }
    }//next
}


//---------------------------- Separation --------------------------------
//
// this calculates a force repelling from the other neighbors
//------------------------------------------------------------------------
Vector2D SteeringBehaviors_Soccer::separation()
{  
    //iterate through all the neighbors and calculate the vector from the
    Vector2D SteeringForce;

    std::list<PlayerBase*>& AllPlayers = AutoList<PlayerBase>::getAllMembers();
    std::list<PlayerBase*>::iterator curPlyr;
    for (curPlyr = AllPlayers.begin(); curPlyr!=AllPlayers.end(); ++curPlyr)
    {
        //make sure this agent isn't included in the calculations and that
        //the agent is close enough
        if((*curPlyr != m_pPlayer) && (*curPlyr)->getSteering()->isTagged())
        {
            Vector2D ToAgent = m_pPlayer->getPos() - (*curPlyr)->getPos();

            //scale the force inversely proportional to the agents distance  
            //from its neighbor.
            SteeringForce += Vec2Normalize(ToAgent)/ToAgent.length();
        }
    }

    return SteeringForce;
}

  
//--------------------------- interpose ----------------------------------
//
//  Given an opponent and an object position this method returns a 
//  force that attempts to position the agent between them
//------------------------------------------------------------------------
Vector2D SteeringBehaviors_Soccer::interpose( const SoccerBall* ball,
                                                                            Vector2D target,
                                                                            float DistFromTarget)
{
    return arrive(target + Vec2Normalize(ball->getPos() - target) * 
                            DistFromTarget, normal);
}

