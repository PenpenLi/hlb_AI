#include "Evaluator_GetWeapon.h"
#include "../GameWorldRaven.h"
#include "../Raven_Map.h"
#include "Goal_Think.h"
#include "Raven_Goal_Types.h"
#include "Raven_Feature.h"




//------------------- calculateDesirability ---------------------------------
//-----------------------------------------------------------------------------
float Evaluator_GetWeapon::calculateDesirability(Raven_Bot* pBot)
{
    //grab the distance to the closest instance of the weapon type
    float Distance = Raven_Feature::getDistanceToItem(pBot, m_iWeaponType);

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
        const float Tweaker = 0.15f;

        float Health, WeaponStrength;

        Health = Raven_Feature::getHealth(pBot);

        WeaponStrength = Raven_Feature::getIndividualWeaponStrength(pBot,m_iWeaponType);

        float Desirability = (Tweaker * Health * (1-WeaponStrength)) / Distance;

        //ensure the value is in the range 0 to 1
        clamp(Desirability, 0, 1);

        Desirability *= m_dCharacterBias;

        return Desirability;
    }
}


//------------------------------ setGoal --------------------------------------
void Evaluator_GetWeapon::setGoal(Raven_Bot* pBot)
{
    pBot->getBrain()->addGoal_GetItem(m_iWeaponType); 
}

