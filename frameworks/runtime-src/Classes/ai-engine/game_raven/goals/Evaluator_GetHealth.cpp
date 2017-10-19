#include "Evaluator_GetHealth.h"
#include "Goal_Think.h"
#include "Raven_Goal_Types.h"
#include "Raven_Feature.h"


//---------------------- calculateDesirability -------------------------------------
//-----------------------------------------------------------------------------
float Evaluator_GetHealth::calculateDesirability(Raven_Bot* pBot)
{
    //first grab the distance to the closest instance of a health item
    float Distance = Raven_Feature::getDistanceToItem(pBot, type_health);

    //if the distance feature is rated with a value of 1 it means that the
    //item is either not present on the map or too far away to be worth 
    //considering, therefore the desirability is zero
    if (Distance == 1)
    {
        return 0;
    }
    else
    {
        //value used to tweak the desirability
        const float Tweaker = 0.2;

        //the desirability of finding a health item is proportional to the amount
        //of health remaining and inversely proportional to the distance from the
        //nearest instance of a health item.
        float Desirability = Tweaker * (1-Raven_Feature::Health(pBot)) / 
                        (Raven_Feature::getDistanceToItem(pBot, type_health));

        //ensure the value is in the range 0 to 1
        clamp(Desirability, 0, 1);

        //bias the value according to the personality of the bot
        Desirability *= m_dCharacterBias;

        return Desirability;
    }
}



//----------------------------- setGoal ---------------------------------------
//-----------------------------------------------------------------------------
void Evaluator_GetHealth::setGoal(Raven_Bot* pBot)
{
    pBot->getBrain()->addGoal_GetItem(type_health); 
}
