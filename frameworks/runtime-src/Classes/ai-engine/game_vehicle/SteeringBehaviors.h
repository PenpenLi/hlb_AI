#ifndef STEERINGBEHAVIORS_H
#define STEERINGBEHAVIORS_H

#include <vector>
#include <string>
#include <list>

#include "common/2D/Vector2D.h"
#include "common/game/Path.h"


class Vehicle;
class BaseEntity;
class Wall;
class Obstacle;
//--------------------------- Constants ----------------------------------
                                     



//------------------------------------------------------------------------

class SteeringBehavior
{
public:
    enum summing_method
    {
        weighted_average, 
        prioritized, 
        dithered
    };
    
    enum behavior_type
    {
        behavior_none               = 0x00000,
        behavior_seek               = 0x00002,
        behavior_flee               = 0x00004,
        behavior_arrive             = 0x00008,
        behavior_wander             = 0x00010,
        behavior_cohesion           = 0x00020,
        behavior_separation         = 0x00040,
        behavior_allignment         = 0x00080,
        behavior_obstacle_avoidance = 0x00100,
        behavior_wall_avoidance     = 0x00200,
        behavior_follow_path        = 0x00400,
        behavior_pursuit            = 0x00800,
        behavior_evade              = 0x01000,
        behavior_interpose          = 0x02000,
        behavior_hide               = 0x04000,
        behavior_flock              = 0x08000,
        behavior_offset_pursuit     = 0x10000,
    };

    SteeringBehavior(Vehicle* agent);
    virtual ~SteeringBehavior();

    //calculates and sums the steering forces from any active behaviors
    Vector2D calculate();

    //calculates the component of the steering force that is parallel with the vehicle heading
    float forwardComponent();

    //calculates the component of the steering force that is perpendicuar with the vehicle heading
    float sideComponent();

    
    void setTargetAgent1(Vehicle* Agent){m_pTargetAgent1 = Agent;}
    void setTargetAgent2(Vehicle* Agent){m_pTargetAgent2 = Agent;}

    void setPath(std::list<Vector2D> new_path){m_pPath->setPath(new_path);}
    
    Vector2D force()const{return m_vSteeringForce;}

    void SetSummingMethod(summing_method sm){m_SummingMethod = sm;}


    void fleeOn(){m_iFlags |= behavior_flee;}
    void fleeOff()  {if(On(behavior_flee))   m_iFlags ^= behavior_flee;}

    void seekOn(){m_iFlags |= behavior_seek;}
    void seekOff()  {if(On(behavior_seek))   m_iFlags ^= behavior_seek;}

    void arriveOn(){m_iFlags |= behavior_arrive;}
    void arriveOff(){if(On(behavior_arrive)) m_iFlags ^= behavior_arrive;}

    void wanderOn(){m_iFlags |= behavior_wander;}
    void wanderOff(){if(On(behavior_wander)) m_iFlags ^= behavior_wander;}

    void pursuitOn(Vehicle* v){m_iFlags |= behavior_pursuit; m_pTargetAgent1 = v;}
    void pursuitOff(){if(On(behavior_pursuit)) m_iFlags ^= behavior_pursuit;}

    void evadeOn(Vehicle* v){m_iFlags |= behavior_evade; m_pTargetAgent1 = v;}
    void evadeOff(){if(On(behavior_evade)) m_iFlags ^= behavior_evade;}

    void cohesionOn(){m_iFlags |= behavior_cohesion;}
    void cohesionOff(){if(On(behavior_cohesion)) m_iFlags ^= behavior_cohesion;}

    void separationOn(){m_iFlags |= behavior_separation;}
    void separationOff(){if(On(behavior_separation)) m_iFlags ^= behavior_separation;}

    void alignmentOn(){m_iFlags |= behavior_allignment;}
    void alignmentOff(){if(On(behavior_allignment)) m_iFlags ^= behavior_allignment;}

    void obstacleAvoidanceOn(){m_iFlags |= behavior_obstacle_avoidance;}
    void obstacleAvoidanceOff(){if(On(behavior_obstacle_avoidance)) m_iFlags ^= behavior_obstacle_avoidance;}

    void wallAvoidanceOn(){m_iFlags |= behavior_wall_avoidance;}
    void wallAvoidanceOff(){if(On(behavior_wall_avoidance)) m_iFlags ^= behavior_wall_avoidance;}

    void followPathOn(){m_iFlags |= behavior_follow_path;}
    void followPathOff(){if(On(behavior_follow_path)) m_iFlags ^= behavior_follow_path;}

    void interposeOn(Vehicle* v1, Vehicle* v2){m_iFlags |= behavior_interpose; m_pTargetAgent1 = v1; m_pTargetAgent2 = v2;}
    void interposeOff(){if(On(behavior_interpose)) m_iFlags ^= behavior_interpose;}

    void hideOn(Vehicle* v){m_iFlags |= behavior_hide; m_pTargetAgent1 = v;}
    void hideOff(){if(On(behavior_hide)) m_iFlags ^= behavior_hide;}

    void offsetPursuitOn(Vehicle* v1, const Vector2D offset){m_iFlags |= behavior_offset_pursuit; m_vOffset = offset; m_pTargetAgent1 = v1;}  
    void offsetPursuitOff(){if(On(behavior_offset_pursuit)) m_iFlags ^= behavior_offset_pursuit;}

    void flockingOn(){cohesionOn(); alignmentOn(); separationOn(); wanderOn();}
    void flockingOff(){cohesionOff(); alignmentOff(); separationOff(); wanderOff();}

    
    bool isFleeOn(){return On(behavior_flee);}
    bool isSeekOn(){return On(behavior_seek);}
    bool isArriveOn(){return On(behavior_arrive);}
    bool isWanderOn(){return On(behavior_wander);}
    bool isPursuitOn(){return On(behavior_pursuit);}
    bool isEvadeOn(){return On(behavior_evade);}
    bool isCohesionOn(){return On(behavior_cohesion);}
    bool isSeparationOn(){return On(behavior_separation);}
    bool isAlignmentOn(){return On(behavior_allignment);}
    bool isObstacleAvoidanceOn(){return On(behavior_obstacle_avoidance);}
    bool isWallAvoidanceOn(){return On(behavior_wall_avoidance);}
    bool isFollowPathOn(){return On(behavior_follow_path);}
    bool isInterposeOn(){return On(behavior_interpose);}
    bool isHideOn(){return On(behavior_hide);}
    bool isOffsetPursuitOn(){return On(behavior_offset_pursuit);}


private:
    //a pointer to the owner of this instance
    Vehicle* m_pVehicle;   
  
    //the steering force created by the combined effect of all the selected behaviors
    Vector2D m_vSteeringForce;
 
    //these can be used to keep track of friends, pursuers, or prey
    Vehicle* m_pTargetAgent1;
    Vehicle* m_pTargetAgent2;

    //the current target
    Vector2D m_vTarget;

    //length of the 'detection box' utilized in obstacle avoidance
    float m_dDBoxLength;

    //a vertex buffer to contain the feelers rqd for wall avoidance  
    std::vector<Vector2D> m_Feelers;
  
    //the length of the 'feeler/s' used in wall detection
    float m_dWallDetectionFeelerLength;


    //the current position on the wander circle the agent is attempting to steer towards
    Vector2D m_vWanderTarget; 

    //explained above
    float m_dWanderJitter;
    float m_dWanderRadius;
    float m_dWanderDistance;

    //multipliers. These can be adjusted to effect strength of the  
    //appropriate behavior. Useful to get flocking the way you require for example.
    float m_dWeightSeparation;
    float m_dWeightCohesion;
    float m_dWeightAlignment;
    float m_dWeightWander;
    float m_dWeightObstacleAvoidance;
    float m_dWeightWallAvoidance;
    float m_dWeightSeek;
    float m_dWeightFlee;
    float m_dWeightArrive;
    float m_dWeightPursuit;
    float m_dWeightOffsetPursuit;
    float m_dWeightInterpose;
    float m_dWeightHide;
    float m_dWeightEvade;
    float m_dWeightFollowPath;

    //how far the agent can 'see'
    float m_dViewDistance;

    //pointer to any current path
    Path* m_pPath;

    //the distance (squared) a vehicle has to be from a path waypoint before
    //it starts seeking to the next waypoint
    float m_dWaypointSeekDistSq;

    //any offset used for formations or offset pursuit
    Vector2D m_vOffset;

    //binary flags to indicate whether or not a behavior should be active
    int m_iFlags;

  
    //Arrive makes use of these to determine how quickly a vehicle
    //should decelerate to its target
    enum Deceleration{
        slow = 3, 
        normal = 2, 
        fast = 1
    };

    //default
    Deceleration m_Deceleration;
    
    //what type of method is used to sum any active behavior
    summing_method  m_SummingMethod;


  //this function tests if a specific bit of m_iFlags is set
  bool On(behavior_type bt){return (m_iFlags & bt) == bt;}

  bool accumulateForce(Vector2D &sf, Vector2D forceToAdd);

  //creates the antenna utilized by the wall avoidance behavior
  void createFeelers();



   /* ......................................................................................................................

                                    BEGIN BEHAVIOR DECLARATIONS

      ......................................................................................................................*/
    //this behavior moves the agent towards a target position
    Vector2D seek(Vector2D targetPos);

    //this behavior returns a vector that moves the agent away from a target position
    Vector2D flee(Vector2D targetPos);

    //this behavior is similar to seek but it attempts to arrive 
    //at the target position with a zero velocity
    Vector2D arrive(Vector2D targetPos, Deceleration deceleration);

    //this behavior predicts where an agent will be in time T and seeks
    //towards that point to intercept it.
    Vector2D pursuit(Vehicle* agent);

    //this behavior maintains a position, in the direction of offset
    //from the target vehicle
    Vector2D offsetPursuit(Vehicle* leader, const Vector2D offset);

    //this behavior attempts to evade a pursuer
    Vector2D evade(Vehicle* agent);

    //this behavior makes the agent wander about randomly
    Vector2D wander();

    //this returns a steering force which will attempt to keep the agent 
    //away from any obstacles it may encounter
    Vector2D obstacleAvoidance(const std::vector<Obstacle *>& obstacles);

    //this returns a steering force which will keep the agent away from any
    //walls it may encounter
    Vector2D wallAvoidance(const std::vector<Wall *> &walls);

    //given a series of Vector2Ds, this method produces a force that will
    //move the agent along the waypoints in order
    Vector2D followPath();

    //this results in a steering force that attempts to steer the vehicle
    //to the center of the vector connecting two moving agents.
    Vector2D interpose(Vehicle* VehicleA, Vehicle* VehicleB);

    //given another agent position to hide from and a list of BaseGameEntitys this
    //method attempts to put an obstacle between itself and its opponent
    Vector2D hide(Vehicle* hunter, const std::vector<Obstacle *>& obstacles);


  // ---------------------------- Group Behaviors -------------------------- //
    Vector2D cohesion(const std::vector<Vehicle*> &agents);
    Vector2D separation(const std::vector<Vehicle*> &agents);
    Vector2D alignment(const std::vector<Vehicle*> &agents);

    //the following three are the same as above but they use cell-space
    //partitioning to find the neighbors
    Vector2D cohesionEx(const std::vector<Vehicle*> &agents);
    Vector2D separationEx(const std::vector<Vehicle*> &agents);
    Vector2D alignmentEx(const std::vector<Vehicle*> &agents);

   /* ......................................................................................................................

                                    END BEHAVIOR DECLARATIONS

      ......................................................................................................................*/


    //calculates and sums the steering forces from any active behaviors
    Vector2D calculateWeightedSum();
    Vector2D calculatePrioritized();
    Vector2D calculateDithered();

    //helper method for Hide. Returns a position located on the other
    //side of an obstacle to the pursuer
    Vector2D getHidingPosition(const Vector2D& posOb,
                                                        const float radiusOb,
                                                        const Vector2D& posHunter); 
};


#endif