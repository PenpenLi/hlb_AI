#ifndef HEALTH_GIVER_H
#define HEALTH_GIVER_H
#pragma warning (disable:4786)
//-----------------------------------------------------------------------------
//
//  Name:     Trigger_HealthGiver.h
//
//  Desc:     If a bot runs over an instance of this class its health is
//            increased. 
//
//-----------------------------------------------------------------------------
#include "common/triggers/Trigger_Respawning.h"
#include "common/triggers/TriggerRegion.h"
#include "../misc/Raven_Bot.h"



class Trigger_HealthGiver : public Trigger_Respawning<Raven_Bot>
{
public:
    Trigger_HealthGiver(Vector2D pos);

    //if triggered, the bot's health will be incremented
    void tryCheck(Raven_Bot* pBot);

private:    
    //the amount of health an entity receives when it runs over this trigger
    int m_iHealthGiven;
};



#endif