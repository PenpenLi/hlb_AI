#ifndef GOALY_H
#define GOALY_H
//------------------------------------------------------------------------
//
//  Name:   GoalKeeper.h
//
//  Desc:   class to implement a goalkeeper agent
//
//
//------------------------------------------------------------------------
#include "common/2D/Vector2D.h"
#include "PlayerBase.h"
#include "common/fsm/StateMachine.h"

class PlayerBase;

class GoalKeeper : public PlayerBase
{
public:
    GoalKeeper(SoccerTeam* home_team,
                            int home_region,
                            State<GoalKeeper>* start_state,
                            Vector2D heading,
                            Vector2D velocity,
                            float mass,
                            float max_force,
                            float max_speed,
                            float max_turn_rate);

    ~GoalKeeper();

    //these must be implemented
    void update(float dt);
    bool handleMessage(const Telegram& msg);

    //returns true if the ball comes close enough for the keeper to 
    //consider intercepting
    bool isBallWithinRangeForIntercept()const;

    //returns true if the keeper has ventured too far away from the goalmouth
    bool isTooFarFromGoalMouth()const;

    //this method is called by the Intercept state to determine the spot
    //along the goalmouth which will act as one of the interpose targets
    //(the other is the ball).
    //the specific point at the goal line that the keeper is trying to cover
    //is flexible and can move depending on where the ball is on the field.
    //To achieve this we just scale the ball's y value by the ratio of the
    //goal width to playingfield width
    Vector2D getRearInterposeTarget()const;
    
    StateMachine<GoalKeeper>* getFSM()const{return m_pStateMachine;}
    
    Vector2D getLookAt()const{return m_vLookAt;}
    void setLookAt(Vector2D v){m_vLookAt=v;}

private:
    //an instance of the state machine class
    StateMachine<GoalKeeper>* m_pStateMachine;
  
    //this vector is updated to point towards the ball and is used when
    //rendering the goalkeeper (instead of the underlaying vehicle's heading)
    //to ensure he always appears to be watching the ball
    Vector2D m_vLookAt;    

    pNode m_ui;
    void updatePortraitByHeading(Vector2D vHeading);
    void render();    
};



#endif