#pragma warning (disable:4786)
#ifndef PLAYERBASE_H
#define PLAYERBASE_H
//------------------------------------------------------------------------
//
//  Name: PlayerBase.h
//
//  Desc: Definition of a soccer player base class. The player inherits
//        from the autolist class so that any player created will be 
//        automatically added to a list that is easily accesible by any
//        other game objects. (mainly used by the steering behaviors and
//        player state classes)
//
//------------------------------------------------------------------------
#include <vector>
#include <string>
#include <cassert>
#include "common/misc/AutoList.h"
#include "common/2D/Vector2D.h"
#include "common/game/MovingEntity.h"

class SoccerTeam;
class SoccerPitch;
class SoccerBall;
class SteeringBehaviors_Soccer;
class Region;

class PlayerBase : public MovingEntity, public AutoList<PlayerBase>
{
public:
    enum player_role
    {
        goal_keeper, 
        attacker, 
        defender
    };
    
    enum move_direction
    {
        move_up = 0, 
        move_down,
        move_left,
        move_right,
        move_max
    };
protected:

    //this player's role in the team
    player_role m_PlayerRole;

    //a pointer to this player's team
    SoccerTeam* m_pTeam;

    //the steering behaviors
    SteeringBehaviors_Soccer* m_pSteering;

    //the region that this player is assigned to.
    int m_homeRegion;

    //the region this player moves to before kickoff
    int m_defaultRegion;

    //the distance to the ball (in squared-space). This value is queried 
    //a lot so it's calculated once each time-step and stored here.
    float m_distSqToBall;

public:


    PlayerBase( SoccerTeam* team,
                             int home_region,
                             Vector2D heading,
                             Vector2D velocity,
                             float mass,
                             float max_force,
                             float max_speed,
                             float max_turn_rate,
                             player_role role);

    virtual ~PlayerBase();

    //returns true if there is an opponent within this player's comfort zone
    bool isThreatened()const;

    //rotates the player to face the ball or the player's current target
    void trackBall();
    
    void trackTarget();

    bool positionInFrontOfPlayer(Vector2D position)const;
    
    //this messages the player that is closest to the supporting spot to
    //change state to support the attacking player
    void findSupport() const;

    //returns true if the ball can be grabbed by the goalkeeper
    bool isBallWithinKeeperRange()const;

    //returns true if the ball is within kicking range
    bool isBallWithinKickingRange()const;

    //returns true if a ball comes within range of a receiver
    bool isBallWithinReceivingRange()const;

    //returns true if the player is located within the boundaries of his home region
    bool isInHomeRegion()const;

    //returns true if this player is ahead of the attacker
    bool isAheadOfAttacker()const;

    //returns true if a player is located at the designated support spot
    bool isAtSupportSpot()const;

    //returns true if the player is located at his steering target
    bool isAtTarget()const;

    //returns true if the player is the closest player in his team to
    //the ball
    bool isClosestTeamMemberToBall()const;

    //returns true if the point specified by 'position' is located in front of the player
    bool isPositionInFrontOfPlayer(Vector2D position)const;

    //returns true if the player is the closest player on the pitch to the ball
    bool isClosestPlayerOnPitchToBall()const;

    //returns true if this player is the controlling player
    bool isControllingPlayer()const;

    //returns true if the player is located in the designated 'hot region' --
    //the area close to the opponent's goal
    bool isInHotRegion()const;

    player_role getRole()const{return m_PlayerRole;}

    float getDistSqToBall()const{return m_distSqToBall;}
    
    void  setDistSqToBall(float val){m_distSqToBall = val;}

    //calculate distance to opponent's/home goal. Used frequently by the passing methods
    float getDistToOppGoal()const;
    
    float  getDistToHomeGoal()const;

    void setDefaultHomeRegion(){m_homeRegion = m_defaultRegion;}

    SoccerBall* const getBall()const;
    SoccerPitch* const getPitch()const;
    SteeringBehaviors_Soccer*const getSteering()const{return m_pSteering;}
    const Region* const getHomeRegion()const;
    void setHomeRegion(int NewRegion){m_homeRegion = NewRegion;}
    SoccerTeam*const getTeam()const{return m_pTeam;}
};

#endif