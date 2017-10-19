#include "Goal_Think.h"
#include <list>
#include "misc/Cgdi.h"
#include "../Raven_ObjectEnumerations.h"
#include "misc/utils.h"
#include "../lua/Raven_Scriptor.h"

#include "Goal_MoveToPosition.h"
#include "Goal_Explore.h"
#include "Goal_GetItem.h"
#include "Goal_Wander.h"
#include "Raven_Goal_Types.h"
#include "Goal_AttackTarget.h"


#include "Evaluator_GetWeapon.h"
#include "Evaluator_GetHealth.h"
#include "Evaluator_Explore.h"
#include "Evaluator_AttackTarget.h"


Goal_Think::Goal_Think(Raven_Bot* pBot):GoalComposite<Raven_Bot>(pBot, goal_think)
{
  
    //these biases could be loaded in from a script on a per bot basis
    //but for now we'll just give them some random values
    const float LowRangeOfBias = 0.5f;
    const float HighRangeOfBias = 1.5f;

    float HealthBias = RandFloatInRange(LowRangeOfBias, HighRangeOfBias);
    float ShotgunBias = RandFloatInRange(LowRangeOfBias, HighRangeOfBias);
    float RocketLauncherBias = RandFloatInRange(LowRangeOfBias, HighRangeOfBias);
    float RailgunBias = RandFloatInRange(LowRangeOfBias, HighRangeOfBias);
    float ExploreBias = RandFloatInRange(LowRangeOfBias, HighRangeOfBias);
    float AttackBias = RandFloatInRange(LowRangeOfBias, HighRangeOfBias);

    //create the evaluator objects
    m_Evaluators.push_back(new Evaluator_GetHealth(HealthBias));
    m_Evaluators.push_back(new Evaluator_Explore(ExploreBias));
    m_Evaluators.push_back(new Evaluator_AttackTarget(AttackBias));
    m_Evaluators.push_back(new Evaluator_GetWeapon(ShotgunBias,type_shotgun));
    m_Evaluators.push_back(new Evaluator_GetWeapon(RailgunBias,type_rail_gun));
    m_Evaluators.push_back(new Evaluator_GetWeapon(RocketLauncherBias,type_rocket_launcher));
}

//----------------------------- dtor ------------------------------------------
//-----------------------------------------------------------------------------
Goal_Think::~Goal_Think()
{
    GoalEvaluators::iterator curDes = m_Evaluators.begin();
    for (curDes; curDes != m_Evaluators.end(); ++curDes)
    {
        delete *curDes;
    }
}

//------------------------------- Activate ------------------------------------
//-----------------------------------------------------------------------------
void Goal_Think::activate()
{
    if (!m_pOwner->isPossessed())
    {
        arbitrate();
    }

    m_iStatus = active;
}

//------------------------------ Process --------------------------------------
//
//  processes the subgoals
//-----------------------------------------------------------------------------
int Goal_Think::process()
{
    activateIfInactive();

    int SubgoalStatus = processSubgoals();

    if (SubgoalStatus == completed || SubgoalStatus == failed)
    {
        if (!m_pOwner->isPossessed())
        {
            m_iStatus = inactive;
        }
    }

    return m_iStatus;
}

//----------------------------- Update ----------------------------------------
// 
//  this method iterates through each goal option to determine which one has
//  the highest desirability.
//-----------------------------------------------------------------------------
void Goal_Think::arbitrate()
{
    float best = 0;
    Evaluator* MostDesirable = nullptr;

    //iterate through all the evaluators to see which produces the highest score
    GoalEvaluators::iterator curDes = m_Evaluators.begin();
    for (curDes; curDes != m_Evaluators.end(); ++curDes)
    {
        float desirabilty = (*curDes)->calculateDesirability(m_pOwner);

        if (desirabilty >= best)
        {
            best = desirabilty;
            MostDesirable = *curDes;
        }
    }

    assert(MostDesirable && "<Goal_Think::arbitrate>: no evaluator selected");

    MostDesirable->setGoal(m_pOwner);
}


//---------------------------- notPresent --------------------------------------
//
//  returns true if the goal type passed as a parameter is the same as this
//  goal or any of its subgoals
//-----------------------------------------------------------------------------
bool Goal_Think::notPresent(unsigned int GoalType)const
{
    if (!m_SubGoals.empty())
    {
        return m_SubGoals.front()->getType() != GoalType;
    }

    return true;
}

void Goal_Think::addGoal_MoveToPosition(Vector2D pos)
{
    addSubgoal( new Goal_MoveToPosition(m_pOwner, pos));
}

void Goal_Think::addGoal_Explore()
{
    if (notPresent(goal_explore))
    {
        removeAllSubgoals();
        addSubgoal( new Goal_Explore(m_pOwner));
    }
}

void Goal_Think::addGoal_GetItem(unsigned int ItemType)
{
    if (notPresent(ItemTypeToGoalType(ItemType)))
    {
        removeAllSubgoals();
        addSubgoal( new Goal_GetItem(m_pOwner, ItemType));
    }
}

void Goal_Think::addGoal_AttackTarget()
{
    if (notPresent(goal_attack_target))
    {
        removeAllSubgoals();
        addSubgoal( new Goal_AttackTarget(m_pOwner));
    }
}

//-------------------------- Queue Goals --------------------------------------
//-----------------------------------------------------------------------------
void Goal_Think::QueueGoal_MoveToPosition(Vector2D pos)
{
    m_SubGoals.push_back(new Goal_MoveToPosition(m_pOwner, pos));
}


