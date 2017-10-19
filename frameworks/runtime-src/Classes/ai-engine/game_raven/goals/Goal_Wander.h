#ifndef GOAL_WANDER_H
#define GOAL_WANDER_H
#pragma warning (disable:4786)
//-----------------------------------------------------------------------------
//
//  Name:   Goal_Wander.h
//
//  Desc:   Causes a bot to wander until terminated
//-----------------------------------------------------------------------------
#include "common/goals/Goal.h"
#include "Raven_Goal_Types.h"
#include "game_raven/Raven_Bot.h"


class Goal_Wander : public Goal<Raven_Bot>
{
public:
    Goal_Wander(Raven_Bot* pBot):Goal<Raven_Bot>(pBot,goal_wander)
    {
    }
    
    void activate();

    int process();

    void terminate();
};





#endif
