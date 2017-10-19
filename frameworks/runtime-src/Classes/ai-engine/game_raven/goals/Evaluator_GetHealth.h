#ifndef RAVEN_HEALTH_EVALUATOR
#define RAVEN_HEALTH_EVALUATOR
#pragma warning (disable:4786)
//-----------------------------------------------------------------------------
//
//  Name:   Evaluator_GetHealth.h
//
//
//  Desc:   class to calculate how desirable the goal of fetching a health item
//          is
//-----------------------------------------------------------------------------

#include "Evaluator.h"
#include "../Raven_Bot.h"

class Evaluator_GetHealth : public Evaluator
{
public:
    Evaluator_GetHealth(float bias):Evaluator(bias){}

    float calculateDesirability(Raven_Bot* pBot);

    void setGoal(Raven_Bot* pEnt);
};

#endif
    
