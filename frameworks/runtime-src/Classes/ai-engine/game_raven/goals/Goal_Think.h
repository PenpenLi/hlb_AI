#ifndef GOAL_THINK_H
#define GOAL_THINK_H
#pragma warning (disable:4786)
//-----------------------------------------------------------------------------
//
//  Name:   Goal_Think .h
//
//
//  Desc:   class to arbitrate between a collection of high level goals, and
//          to process those goals.
//-----------------------------------------------------------------------------
#include <vector>
#include <string>
#include "common/2D/Vector2D.h"
#include "common/goals/GoalComposite.h"
#include "../misc/Raven_Bot.h"
#include "Evaluator.h"



class Goal_Think : public GoalComposite<Raven_Bot>
{
public:
    Goal_Think(Raven_Bot* pBot);
    ~Goal_Think();

    //this method iterates through each goal evaluator and selects the one
    //that has the highest score as the current goal
    void arbitrate();

    //returns true if the given goal is not at the front of the subgoal list
    bool notPresent(unsigned int GoalType)const;

    //the usual suspects
    int  process();
    void activate();
    void terminate(){}
  
    //top level goal types
    void addGoal_MoveToPosition(Vector2D pos);
    void addGoal_GetItem(unsigned int ItemType);
    void addGoal_Explore();
    void addGoal_AttackTarget();

    //this adds the MoveToPosition goal to the *back* of the subgoal list.
    void queueGoal_MoveToPosition(Vector2D pos);
    
private:
    typedef std::vector<Evaluator*> GoalEvaluators;

    GoalEvaluators  m_Evaluators;
};


#endif