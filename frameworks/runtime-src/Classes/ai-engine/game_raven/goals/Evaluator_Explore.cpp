#include "Evaluator_Explore.h"
#include "../navigation/Raven_PathPlanner.h"
#include "Raven_Feature.h"
#include "Goal_Think.h"
#include "Raven_Goal_Types.h"




//---------------- CalculateDesirability -------------------------------------
//-----------------------------------------------------------------------------
float Evaluator_Explore::calculateDesirability(Raven_Bot* pBot)
{
    float Desirability = 0.05f;

    Desirability *= m_dCharacterBias;

    return Desirability;
}

//----------------------------- SetGoal ---------------------------------------
//-----------------------------------------------------------------------------
void Evaluator_Explore::setGoal(Raven_Bot* pBot)
{
    pBot->getBrain()->addGoal_Explore();
}
