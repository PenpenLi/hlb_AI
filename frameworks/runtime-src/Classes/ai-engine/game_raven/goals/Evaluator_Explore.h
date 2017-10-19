#ifndef RAVEN_EXPLORE_GOAL_EVALUATOR
#define RAVEN_EXPLORE_GOAL_EVALUATOR
#pragma warning (disable:4786)
//-----------------------------------------------------------------------------
//
//  Name:   Evaluator_Explore.h
//
//
//  Desc:  class to calculate how desirable the goal of exploring is
//-----------------------------------------------------------------------------

#include "Evaluator.h"
#include "../Raven_Bot.h"


class Evaluator_Explore : public Evaluator
{ 
public:
    Evaluator_Explore(float bias):Evaluator(bias){}

    float calculateDesirability(Raven_Bot* pBot);

    void setGoal(Raven_Bot* pEnt);
};

#endif