#pragma warning (disable:4786)
#ifndef FIELDPLAYER_H
#define FIELDPLAYER_H
//------------------------------------------------------------------------
//
//  Name:   FieldPlayer.h
//
//  Desc:   Derived from a PlayerBase, this class encapsulates a player
//          capable of moving around a soccer pitch, kicking, dribbling,
//          shooting etc
//
//  Author: Mat Buckland 2003 (fup@ai-junkie.com)
//
//------------------------------------------------------------------------
#include <vector>
#include <string>
#include <algorithm>
#include <cassert>

#include "common/2D/Vector2D.h"
#include "common/fsm/StateMachine.h"
#include "common/misc/Regulator.h"
#include "FieldPlayerStates.h"
#include "PlayerBase.h"


class SteeringBehaviors_Soccer;
class SoccerTeam;
class SoccerPitch;
class SoccerGoal;
struct Telegram;


class FieldPlayer : public PlayerBase
{
public:
    FieldPlayer( SoccerTeam* home_team,
                            int home_region,
                            State<FieldPlayer>* start_state,
                            Vector2D heading,
                            Vector2D velocity,
                            float mass,
                            float max_force,
                            float max_speed,
                            float max_turn_rate,
                            player_role role);   
  
    ~FieldPlayer();

    //call this to update the player's position and orientation
    void update(float dt);   
    
    bool handleMessage(const Telegram& msg);

    StateMachine<FieldPlayer>* getFSM()const{return m_pStateMachine;}
    
    bool isReadyForNextKick()const{return m_pKickLimiter->isReady();} 

private:
    //an instance of the state machine class
    StateMachine<FieldPlayer>* m_pStateMachine;

    //limits the number of kicks a player may take per second
    Regulator* m_pKickLimiter;

    pNode m_ui;
    void updatePortraitByHeading(Vector2D vHeading);
    void render();
};




#endif