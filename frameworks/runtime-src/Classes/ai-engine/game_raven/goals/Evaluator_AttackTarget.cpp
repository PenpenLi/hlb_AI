#include "Evaluator_AttackTarget.h"
#include "Goal_Think.h"
#include "Raven_Goal_Types.h"
#include "../Raven_WeaponSystem.h"
#include "Raven_Feature.h"



//------------------ calculateDesirability ------------------------------------
//
//  returns a value between 0 and 1 that indicates the Rating of a bot (the
//  higher the score, the stronger the bot).
//-----------------------------------------------------------------------------
float Evaluator_AttackTarget::calculateDesirability(Raven_Bot* pBot)
{
    float Desirability = 0.0f;

    //only do the calculation if there is a target present
    if (pBot->getTargetSys()->isTargetPresent()) 
    {
        const float Tweaker = 1.0f;

        Desirability = Tweaker * Raven_Feature::getHealth(pBot) * Raven_Feature::getTotalWeaponStrength(pBot);

        //bias the value according to the personality of the bot
        Desirability *= m_dCharacterBias;
    }

    return Desirability;
}

//----------------------------- setGoal ---------------------------------------
//-----------------------------------------------------------------------------
void Evaluator_AttackTarget::setGoal(Raven_Bot* pBot)
{
    pBot->getBrain()->addGoal_AttackTarget(); 
}
