#ifndef SOCCERTEAM_H
#define SOCCERTEAM_H
#pragma warning (disable:4786)

//------------------------------------------------------------------------
//
//  Name:   SoccerTeam.h
//
//  Desc:   class to define a team of soccer playing agents. A SoccerTeam
//          contains several field players and one goalkeeper. A SoccerTeam
//          is implemented as a finite state machine and has states for
//          attacking, defending, and KickOff.
//
//
//------------------------------------------------------------------------

#include <vector>
#include "common/game/BaseNode.h"
#include "common/game/Region.h"
#include "SupportSpotCalculator.h"
#include "common/fsm/StateMachine.h"

class SoccerGoal;
class PlayerBase;
class FieldPlayer;
class SoccerPitch;
class GoalKeeper;
class SupportSpotCalculator;


class SoccerTeam :public BaseNode 
{
public:
    enum team_color 
    {
        blue,
        red
    };

private:

    //an instance of the state machine class
    StateMachine<SoccerTeam>* m_pStateMachine;

    //the team must know its own color!
    team_color m_Color;

    //pointers to the team members
    std::vector<PlayerBase*> m_Players;

    //a pointer to the soccer pitch
    SoccerPitch* m_pPitch;

    //pointers to the goals
    SoccerGoal* m_pOpponentsGoal;
    SoccerGoal* m_pHomeGoal;

    //a pointer to the opposing team
    SoccerTeam* m_pOpponents;

    //pointers to 'key' players
    PlayerBase* m_pControllingPlayer;
    PlayerBase* m_pSupportingPlayer;
    PlayerBase* m_pReceivingPlayer;
    PlayerBase* m_pPlayerClosestToBall;

    //the squared distance the closest player is from the ball
    float m_dDistSqToBallOfClosestPlayer;

    //players use this to determine strategic positions on the playing field
    SupportSpotCalculator* m_pSupportSpotCalc;


    //creates all the players for this team
    void createPlayers();

    //called each frame. Sets m_pClosestPlayerToBall to point to the player
    //closest to the ball. 
    void calculateClosestPlayerToBall();


public:
    SoccerTeam(SoccerGoal* home_goal,
                             SoccerGoal* opponents_goal,
                             SoccerPitch* pitch,
                             team_color color);

    ~SoccerTeam();

    void update(float dt);

    //calling this changes the state of all field players to that of 
    //ReturnToHomeRegion. Mainly used when a goal keeper has possession
    void returnAllFieldPlayersToHome()const;

    //returns true if player has a clean shot at the goal and sets ShotTarget
    //to a normalized vector pointing in the direction the shot should be
    //made. Else returns false and sets heading to a zero vector
    bool canShoot(Vector2D BallPos, float power, Vector2D& ShotTarget = Vector2D()) const;

    //The best pass is considered to be the pass that cannot be intercepted 
    //by an opponent and that is as far forward of the receiver as possible  
    //If a pass is found, the receiver's address is returned in the 
    //reference, 'receiver' and the position the pass will be made to is 
    //returned in the  reference 'PassTarget'
    bool findPass( const PlayerBase*const passer,
                                PlayerBase*& receiver,
                                Vector2D& passTarget,
                                float power,
                                float minPassingDistance) const;

    //Three potential passes are calculated. One directly toward the receiver's
    //current position and two that are the tangents from the ball position
    //to the circle of radius 'range' from the receiver.
    //These passes are then tested to see if they can be intercepted by an
    //opponent and to make sure they terminate within the playing area. If
    //all the passes are invalidated the function returns false. Otherwise
    //the function returns the pass that takes the ball closest to the 
    //opponent's goal area.
    bool getBestPassToReceiver( const PlayerBase* const passer,
                                                            const PlayerBase* const receiver,
                                                            Vector2D& passTarget,
                                                            const float power)const;

    //test if a pass from positions 'from' to 'target' kicked with force 
    //'PassingForce'can be intercepted by an opposing player
    bool isPassSafeFromOpponent( Vector2D from,
                                                                Vector2D target,
                                                                const PlayerBase* const receiver,
                                                                const PlayerBase* const opp,
                                                                float passingForce)const;

    //tests a pass from position 'from' to position 'target' against each member
    //of the opposing team. Returns true if the pass can be made without
    //getting intercepted
    bool isPassSafeFromAllOpponents(Vector2D from,
                                                                    Vector2D target,
                                                                    const PlayerBase* const receiver,
                                                                    float passingForce)const;

    //returns true if there is an opponent within radius of position
    bool isOpponentWithinRadius(Vector2D pos, float rad);

    //this tests to see if a pass is possible between the requester and
    //the controlling player. If it is possible a message is sent to the
    //controlling player to pass the ball asap.
    void requestPass(FieldPlayer* requester)const;

    //calculates the best supporting position and finds the most appropriate
    //attacker to travel to the spot
    PlayerBase* determineBestSupportingAttacker();
  

    const std::vector<PlayerBase*>& getMembers()const {return m_Players;}  

    StateMachine<SoccerTeam>* getFSM()const {return m_pStateMachine;}

    SoccerGoal*const getHomeGoal()const {return m_pHomeGoal;}
    
    SoccerGoal*const getOpponentsGoal()const {return m_pOpponentsGoal;}

    SoccerPitch*const getPitch()const {return m_pPitch;}           

    SoccerTeam*const getOpponents()const {return m_pOpponents;}
    void setOpponents(SoccerTeam* opps) {m_pOpponents = opps;}

    team_color getTeamColor()const {return m_Color;}

    void setPlayerClosestToBall(PlayerBase* plyr) {m_pPlayerClosestToBall=plyr;}
    PlayerBase* getPlayerClosestToBall()const {return m_pPlayerClosestToBall;}
  
    float getClosestDistToBallSq()const {return m_dDistSqToBallOfClosestPlayer;}

    Vector2D getSupportSpot()const {return m_pSupportSpotCalc->getBestSupportingSpot();}

    PlayerBase* getSupportingPlayer()const {return m_pSupportingPlayer;}

    void setSupportingPlayer(PlayerBase* plyr) {m_pSupportingPlayer = plyr;}

    PlayerBase* getReceiver()const {return m_pReceivingPlayer;}
    void setReceiver(PlayerBase* plyr) {m_pReceivingPlayer = plyr;}

    PlayerBase* getControllingPlayer()const{return m_pControllingPlayer;}
    void setControllingPlayer(PlayerBase* plyr) 
    {
        m_pControllingPlayer = plyr;
        //rub it in the opponents faces!
        getOpponents()->lostControl();
    }

    bool  isInControl()const {if(m_pControllingPlayer) return true; else return false;}
    void  lostControl(){m_pControllingPlayer = NULL;}

    PlayerBase* getPlayerFromID(int id)const;
  

    void setPlayerHomeRegion(int plyr, int region)const;

    void determineBestSupportingPosition()const{m_pSupportSpotCalc->determineBestSupportingPosition();}

    void updateTargetsOfWaitingPlayers()const;

    //returns false if any of the team are not located within their home region
    bool isAllPlayersAtHome()const;
};

#endif