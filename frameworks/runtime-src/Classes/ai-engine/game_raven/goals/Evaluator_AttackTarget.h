#ifndef RAVEN_ATTACK_GOAL_EVALUATOR
#define RAVEN_ATTACK_GOAL_EVALUATOR
#pragma warning (disable:4786)
//-----------------------------------------------------------------------------
//
//  Name:   Evaluator_AttackTarget.h
//
//
//  Desc:  class to calculate how desirable the goal of attacking the bot's
//         current target is
//-----------------------------------------------------------------------------

#include "Evaluator.h"
#include "../Raven_Bot.h"


class Evaluator_AttackTarget : public Evaluator
{ 
public:
    Evaluator_AttackTarget(float bias):Evaluator(bias){}

    float calculateDesirability(Raven_Bot* pBot);

    void setGoal(Raven_Bot* pEnt);
};



#endif