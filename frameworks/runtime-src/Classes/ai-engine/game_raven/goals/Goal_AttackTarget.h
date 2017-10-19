#ifndef GOAL_ATTACKTARGET_H
#define GOAL_ATTACKTARGET_H
#pragma warning (disable:4786)
//-----------------------------------------------------------------------------
//
//  Name:   Goal_AttackTarget.h
//
//
//  Desc:   
//
//-----------------------------------------------------------------------------
#include "common/goals/GoalComposite.h"
#include "Raven_Goal_Types.h"
#include "../Raven_Bot.h"





class Goal_AttackTarget : public GoalComposite<Raven_Bot>
{
public:
    Goal_AttackTarget(Raven_Bot* pOwner):GoalComposite<Raven_Bot>(pOwner, goal_attack_target)
    {
    }

    void activate();

    int  process();

    void terminate(){m_iStatus = completed;}
};


#endif
