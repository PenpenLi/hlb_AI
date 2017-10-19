#ifndef SteeringBehaviors_Soccer_H
#define SteeringBehaviors_Soccer_H
#pragma warning (disable:4786)
//------------------------------------------------------------------------
//
//  Name:   SteeringBehaviors_Soccer.h
//
//  Desc:   class to encapsulate steering behaviors for a soccer player
//
//------------------------------------------------------------------------
#include <vector>
#include <string>
#include "common/2D/Vector2D.h"

class PlayerBase;
class SoccerPitch;
class SoccerBall;

//---------------------------- class details -----------------------------

class SteeringBehaviors_Soccer
{
private:
    enum behavior_type
    {
        behavior_none            = 0x0000,
        behavior_seek            = 0x0001,
        behavior_arrive          = 0x0002,
        behavior_separation   = 0x0004,
        behavior_pursuit         = 0x0008,
        behavior_interpose     = 0x0010
    };
    
    //Arrive makes use of these to determine how quickly a vehicle
    //should decelerate to its target
    enum Deceleration
    {
        slow = 3,
        normal = 2,
        fast = 1
    };
  
    PlayerBase* m_pPlayer;                                                  

    SoccerBall* m_pBall;

    //the steering force created by the combined effect of all
    //the selected behaviors
    Vector2D m_vSteeringForce;

    //the current target (usually the ball or predicted ball position)
    Vector2D m_vTarget;

    //the distance the player tries to interpose from the target
    float m_dInterposeDist;

    //multipliers. 
    float m_dMultSeparation;

    //how far it can 'see'
    float m_dViewDistance;
    
    //binary flags to indicate whether or not a behavior should be active
    int m_iFlags;
    
    //used by group behaviors to tag neighbours
    bool m_bTagged;
  


    //this behavior moves the agent towards a target position
    Vector2D seek(Vector2D target);

    //this behavior is similar to seek but it attempts to arrive 
    //at the target with a zero velocity
    Vector2D arrive(Vector2D target, Deceleration decel);

    //This behavior predicts where its prey will be and seeks
    //to that location
    Vector2D pursuit(const SoccerBall* ball);

    Vector2D separation();

    //this attempts to steer the agent to a position between the opponent
    //and the object
    Vector2D interpose(const SoccerBall* ball,
                                     Vector2D pos,
                                     float DistFromTarget);


    //finds any neighbours within the view radius
    void findNeighbours();


    //this function tests if a specific bit of m_iFlags is set
    bool On(behavior_type bt){return (m_iFlags & bt) == bt;}

    bool accumulateForce(Vector2D &sf, Vector2D ForceToAdd);

    Vector2D sumForces();

    //a vertex buffer to contain the feelers rqd for dribbling
    std::vector<Vector2D> m_Antenna;


public:
    SteeringBehaviors_Soccer(PlayerBase* agent,
                                                        SoccerPitch* world,
                                                        SoccerBall* ball);

    virtual ~SteeringBehaviors_Soccer(){}

 
    Vector2D calculate();

    //calculates the component of the steering force that is parallel
    //with the vehicle heading
    float forwardComponent();

    //calculates the component of the steering force that is perpendicuar
    //with the vehicle heading
    float sideComponent();

    Vector2D getForce()const{return m_vSteeringForce;}



    Vector2D  getTarget()const{return m_vTarget;}
    void setTarget(const Vector2D t){m_vTarget = t;}

    float getInterposeDistance()const{return m_dInterposeDist;}
    void setInterposeDistance(float d){m_dInterposeDist = d;}

    bool isTagged()const{return m_bTagged;}
    void tag(){m_bTagged = true;}
    void unTag(){m_bTagged = false;}
  

    void seekOn(){m_iFlags |= behavior_seek;}
    void arriveOn(){m_iFlags |= behavior_arrive;}
    void pursuitOn(){m_iFlags |= behavior_pursuit;}
    void separationOn(){m_iFlags |= behavior_separation;}
    void interposeOn(float d){m_iFlags |= behavior_interpose; m_dInterposeDist = d;}

  
    void seekOff()  {if(On(behavior_seek))   m_iFlags ^=behavior_seek;}
    void arriveOff(){if(On(behavior_arrive)) m_iFlags ^=behavior_arrive;}
    void pursuitOff(){if(On(behavior_pursuit)) m_iFlags ^=behavior_pursuit;}
    void separationOff(){if(On(behavior_separation)) m_iFlags ^=behavior_separation;}
    void interposeOff(){if(On(behavior_interpose)) m_iFlags ^=behavior_interpose;}


    bool isSeekOn(){return On(behavior_seek);}
    bool isArriveOn(){return On(behavior_arrive);}
    bool isPursuitOn(){return On(behavior_pursuit);}
    bool isSeparationOn(){return On(behavior_separation);}
    bool isInterposeOn(){return On(behavior_interpose);}

};




#endif