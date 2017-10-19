#ifndef GOAL_MOVE_POS_H
#define GOAL_MOVE_POS_H
#pragma warning (disable:4786)

#include "common/goals/GoalComposite.h"
#include "common/2D/Vector2D.h"
#include "game_raven/Raven_Bot.h"
#include "Raven_Goal_Types.h"



class Goal_MoveToPosition : public GoalComposite<Raven_Bot>
{
public:
    Goal_MoveToPosition(Raven_Bot* pBot, Vector2D pos):
                                                                GoalComposite<Raven_Bot>(pBot,goal_move_to_position),
                                                                m_vDestination(pos)
    {
    }

    //the usual suspects
    void activate();
    
    int  process();
    
    void terminate(){}

    //this goal is able to accept messages
    bool handleMessage(const Telegram& msg);

private:
    //the position the bot wants to reach
    Vector2D m_vDestination;    
};


#endif
