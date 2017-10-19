
#include "Trigger_WeaponGiver.h"

///////////////////////////////////////////////////////////////////////////////

Trigger_WeaponGiver::Trigger_WeaponGiver(int entityType, Vector2D pos)
{
    setEntityType(entityType);
    setPos(pos);
    
    addCircularTriggerRegion(getPos(), Para_DefaultGiverTriggerRange);
    setRespawnDelay(Para_Weapon_RespawnDelay);
}


void Trigger_WeaponGiver::tryCheck(Raven_Bot* pBot)
{
    if (this->isActive() && this->isTouchingTrigger(pBot->getPos(), pBot->getBoundingRadius()))
    {
        pBot->getWeaponSys()->addWeapon(getEntityType());

        deactivate();
    } 
}
