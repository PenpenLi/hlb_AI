
#include "Trigger_HealthGiver.h"



///////////////////////////////////////////////////////////////////////////////
Trigger_HealthGiver::Trigger_HealthGiver(Vector2D pos):m_iHealthGiven(Para_Health_Given)
{
    setPos(pos);
    
    //create this trigger's region of fluence
    addCircularTriggerRegion(pos, Para_DefaultGiverTriggerRange);

    setRespawnDelay(Para_Health_RespawnDelay);
}


void Trigger_HealthGiver::tryCheck(Raven_Bot* pBot)
{
    if (isActive() && isTouchingTrigger(pBot->getPos(), pBot->getBoundingRadius()))
    {
        pBot->increaseHealth(m_iHealthGiven);

        deactivate();
    } 
}

