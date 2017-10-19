#ifndef GOAL_FIND_TARGET_H
#define GOAL_FIND_TARGET_H
#pragma warning (disable:4786)
//-----------------------------------------------------------------------------
//
//  Name:   Goal_HuntTarget.h
//
//
//  Desc:   Causes a bot to search for its current target. Exits when target
//          is in view
//-----------------------------------------------------------------------------
#include "common/goals/GoalComposite.h"
#include "Raven_Goal_Types.h"
#include "../Raven_Bot.h"


class Goal_HuntTarget : public GoalComposite<Raven_Bot>
{
private:
    //this value is set to true if the last visible position of the target
    //bot has been searched without success
    bool m_bLVPTried;

public:

    Goal_HuntTarget(Raven_Bot* pBot):GoalComposite<Raven_Bot>(pBot, goal_hunt_target),
                                                                  m_bLVPTried(false)
    {
    }

    //the usual suspects
    void activate();
    
    int  process();
    
    void terminate(){}
};





#endif
