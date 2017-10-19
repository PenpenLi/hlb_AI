#ifndef STEERINGBEHAVIORS_H
#define STEERINGBEHAVIORS_H
#pragma warning (disable:4786)
//------------------------------------------------------------------------
//
//  Name:   Raven_SteeringBehavior.h
//
//  Desc:   class to encapsulate steering behaviors for a Raven_Bot
//
//
//------------------------------------------------------------------------
#include <vector>
#include <string>
#include <list>
#include "common/2D/Vector2D.h"

class Raven_Bot;
class Wall;
class BaseEntity;
class GameWorldRaven;


//--------------------------- Constants ----------------------------------

//the radius of the constraining circle for the wander behavior
const float WanderRad = 1.2f;
//distance the wander circle is projected in front of the agent
const float WanderDist = 2.0f;
//the maximum amount of displacement along the circle each frame
const float WanderJitterPerSec = 40.0f;

                                          



//------------------------------------------------------------------------

class Raven_SteeringBehaviors
{
public:
    enum summing_method
    {
        weighted_average, 
        prioritized, 
        dithered
    };

private:
    enum behavior_type
    {
        behavior_none = 0x00000,
        behavior_seek = 0x00002,
        behavior_arrive = 0x00008,
        behavior_wander = 0x00010,
        behavior_separation = 0x00040,
        behavior_wall_avoidance = 0x00200,
    };

private:
  //a pointer to the owner of this instance
  Raven_Bot* m_pRaven_Bot; 
  
  //pointer to the world data
  GameWorldRaven* m_pWorld;
  
    //the steering force created by the combined effect of all
    //the selected behaviors
    Vector2D m_vSteeringForce;
 
    //these can be used to keep track of friends, pursuers, or prey
    Raven_Bot* m_pTargetAgent1;
    Raven_Bot* m_pTargetAgent2;

    //the current target
    Vector2D m_vTarget;


    //a vertex buffer to contain the feelers rqd for wall avoidance  
    std::vector<Vector2D> m_Feelers;

    //the length of the 'feeler/s' used in wall detection
    float m_dWallDetectionFeelerLength;


    //the current position on the wander circle the agent is
    //attempting to steer towards
    Vector2D m_vWanderTarget; 

    //explained above
    float m_dWanderJitter;
    float m_dWanderRadius;
    float m_dWanderDistance;


    //multipliers. These can be adjusted to effect strength of the  
    //appropriate behavior.
    float m_dWeightSeparation;
    float m_dWeightWander;
    float m_dWeightWallAvoidance;
    float m_dWeightSeek;
    float m_dWeightArrive;


    //how far the agent can 'see'
    float m_dViewDistance;

    //binary flags to indicate whether or not a behavior should be active
    int m_iFlags;

    //Arrive makes use of these to determine how quickly a Raven_Bot
    //should decelerate to its target
    enum Deceleration
    {
        slow = 3, 
        normal = 2, 
        fast = 1
    };

    //default
    Deceleration m_Deceleration;

    //is cell space partitioning to be used or not?
    bool m_bCellSpaceOn;

    //what type of method is used to sum any active behavior
    summing_method  m_SummingMethod;

    //this function tests if a specific bit of m_iFlags is set
    bool On(behavior_type bt){return (m_iFlags & bt) == bt;}

    bool accumulateForce(Vector2D &sf, Vector2D ForceToAdd);

    //creates the antenna utilized by the wall avoidance behavior
    void createFeelers();


   /* .......................................................

                    BEGIN BEHAVIOR DECLARATIONS

      .......................................................*/
    //this behavior moves the agent towards a target position
    Vector2D seek(const Vector2D &target);

    //this behavior is similar to seek but it attempts to arrive 
    //at the target with a zero velocity
    Vector2D arrive(const Vector2D& target, const Deceleration deceleration);

    //this behavior makes the agent wander about randomly
    Vector2D wander();

    //this returns a steering force which will keep the agent away from any
    //walls it may encounter
    Vector2D wallAvoidance(const std::vector<Wall*> &walls);

    Vector2D separation(const std::list<Raven_Bot*> &agents);


    /* .......................................................

                       END BEHAVIOR DECLARATIONS

      .......................................................*/

    //calculates and sums the steering forces from any active behaviors
    Vector2D calculatePrioritized();

public:
    Raven_SteeringBehaviors(GameWorldRaven* world, Raven_Bot* agent);

    virtual ~Raven_SteeringBehaviors();

    //calculates and sums the steering forces from any active behaviors
    Vector2D calculate();

    //calculates the component of the steering force that is parallel
    //with the Raven_Bot heading
    float getForwardComponent();

    //calculates the component of the steering force that is perpendicuar
    //with the Raven_Bot heading
    float getSideComponent();

    void setTarget(Vector2D t){m_vTarget = t;}
    Vector2D getTarget()const{return m_vTarget;}

    void setTargetAgent1(Raven_Bot* Agent){m_pTargetAgent1 = Agent;}
    void setTargetAgent2(Raven_Bot* Agent){m_pTargetAgent2 = Agent;}

    Vector2D getForce()const{return m_vSteeringForce;}

    void setSummingMethod(summing_method sm){m_SummingMethod = sm;}

    void seekOn(){m_iFlags |= behavior_seek;}
    void arriveOn(){m_iFlags |= behavior_arrive;}
    void wanderOn(){m_iFlags |= behavior_wander;}
    void separationOn(){m_iFlags |= behavior_separation;}
    void wallAvoidanceOn(){m_iFlags |= behavior_wall_avoidance;}

    void seekOff()  {if(On(behavior_seek))   m_iFlags ^=behavior_seek;}
    void arriveOff(){if(On(behavior_arrive)) m_iFlags ^=behavior_arrive;}
    void wanderOff(){if(On(behavior_wander)) m_iFlags ^=behavior_wander;}
    void separationOff(){if(On(behavior_separation)) m_iFlags ^=behavior_separation;}
    void wallAvoidanceOff(){if(On(behavior_wall_avoidance)) m_iFlags ^=behavior_wall_avoidance;}

    bool isSeekOn(){return On(behavior_seek);}
    bool isArriveOn(){return On(behavior_arrive);}
    bool isWanderOn(){return On(behavior_wander);}
    bool isSeparationOn(){return On(behavior_separation);}
    bool isWallAvoidanceOn(){return On(behavior_wall_avoidance);}

    const std::vector<Vector2D>& getFeelers()const{return m_Feelers;}
  
    float getWanderJitter()const{return m_dWanderJitter;}
    float getWanderDistance()const{return m_dWanderDistance;}
    float getWanderRadius()const{return m_dWanderRadius;}

    float getSeparationWeight()const{return m_dWeightSeparation;}

};

#endif