#include "Raven_Feature.h"
#include "../Raven_Bot.h"
#include "../navigation/Raven_PathPlanner.h"
#include "../armory/Weapon.h"
#include "../Raven_WeaponSystem.h"

//-----------------------------------------------------------------------------
float Raven_Feature::getDistanceToItem(Raven_Bot* pBot, int ItemType)
{
    //determine the distance to the closest instance of the item type
    float getDistanceToItem = pBot->getPathPlanner()->getCostToClosestItem(ItemType);

    //if the previous method returns a negative value then there is no item of
    //the specified type present in the game world at this time.
    if (getDistanceToItem < 0 ) return 1;

    //these values represent cutoffs. Any distance over MaxDistance results in
    //a value of 0, and value below MinDistance results in a value of 1
    const float MaxDistance = 500.0f;
    const float MinDistance = 50.0f;

    clamp(getDistanceToItem, MinDistance, MaxDistance);

    return getDistanceToItem / MaxDistance;
}


//----------------------- getMaxRoundsBotCanCarryForWeapon --------------------
//
//  helper function to tidy up IndividualWeapon method
//  returns the maximum rounds of ammo a bot can carry for the given weapon
//-----------------------------------------------------------------------------
float Raven_Feature::getMaxRoundsBotCanCarryForWeapon(int WeaponType)
{
    switch(WeaponType)
    {
        case type_rail_gun:
            return Para_RailGun_MaxRoundsCarried;

        case type_rocket_launcher:
            return Para_RocketLauncher_MaxRoundsCarried;

        case type_shotgun:
            return Para_ShotGun_MaxRoundsCarried;

        default:
            throw std::runtime_error("trying to calculate  of unknown weapon");

    }//end switch
}


//----------------------- getIndividualWeaponStrength ----------------------
//-----------------------------------------------------------------------------
float Raven_Feature::getIndividualWeaponStrength(Raven_Bot* pBot, int WeaponType)
{
    //grab a pointer to the gun (if the bot owns an instance)
    Weapon* wp = pBot->getWeaponSys()->getWeaponFromInventory(WeaponType);
    if (wp)
    {
        return wp->getNumRoundsRemaining() /getMaxRoundsBotCanCarryForWeapon(WeaponType);
    }
    else
    {
        return 0.0;
    }
}

//--------------------- getTotalWeaponStrength --------------
//-----------------------------------------------------------------------------
float Raven_Feature::getTotalWeaponStrength(Raven_Bot* pBot)
{
    const float MaxRoundsForShotgun = getMaxRoundsBotCanCarryForWeapon(type_shotgun);
    const float MaxRoundsForRailgun = getMaxRoundsBotCanCarryForWeapon(type_rail_gun);
    const float MaxRoundsForRocketLauncher = getMaxRoundsBotCanCarryForWeapon(type_rocket_launcher);
    const float TotalRoundsCarryable = MaxRoundsForShotgun + MaxRoundsForRailgun + MaxRoundsForRocketLauncher;

    float NumSlugs = (float)pBot->getWeaponSys()->getAmmoRemainingForWeapon(type_rail_gun);
    float NumCartridges = (float)pBot->getWeaponSys()->getAmmoRemainingForWeapon(type_shotgun);
    float NumRockets = (float)pBot->getWeaponSys()->getAmmoRemainingForWeapon(type_rocket_launcher);

    //the value of the tweaker (must be in the range 0-1) indicates how much
    //desirability value is returned even if a bot has not picked up any weapons.
    //(it basically adds in an amount for a bot's persistent weapon -- the blaster)
    const float Tweaker = 0.1;

    return Tweaker + (1-Tweaker)*(NumSlugs + NumCartridges + NumRockets)/TotalRoundsCarryable;
}

//------------------------------- getHealthScore ---------------------------------
//
//-----------------------------------------------------------------------------
float Raven_Feature::getHealth(Raven_Bot* pBot)
{
    return (float)pBot->getHealth() / (float)pBot->getMaxHealth();
}
