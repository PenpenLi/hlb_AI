#ifndef WEAPON_GIVER_H
#define WEAPON_GIVER_H
#pragma warning (disable:4786)
//-----------------------------------------------------------------------------
//
//  Name:     Trigger_WeaponGiver.h
//
//
//  Desc:     This trigger 'gives' the triggering bot a weapon of the
//            specified type 
//
//-----------------------------------------------------------------------------
#include "common/triggers/Trigger_Respawning.h"
#include "../misc/Raven_Bot.h"


class Trigger_WeaponGiver : public Trigger_Respawning<Raven_Bot>
{
public:
    //this type of trigger is created when reading a map file
    Trigger_WeaponGiver(int entityType, Vector2D pos);

    //if triggered, this trigger will call the PickupWeapon method of the
    //bot. PickupWeapon will instantiate a weapon of the appropriate type.
    void tryCheck(Raven_Bot* pBot);
};




#endif