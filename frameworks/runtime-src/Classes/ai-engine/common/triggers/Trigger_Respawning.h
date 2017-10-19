#ifndef Trigger_Respawning_H
#define Trigger_Respawning_H
#pragma warning (disable:4786)
//-----------------------------------------------------------------------------
//
//  Name:     Trigger_Respawning.h
//
//  Desc:     base class to create a trigger that is capable of respawning
//            after a period of inactivity
//
//-----------------------------------------------------------------------------
#include "Trigger.h"
#include <chrono>

template <class entity_type>
class Trigger_Respawning : public Trigger<entity_type>
{
protected:
    unsigned int activePeriodMs; //millisecond
    
    std::chrono::steady_clock::time_point m_nextActiveTime;

    //sets the trigger to be inactive for m_iNumUpdatesBetweenRespawns 
    //update-steps
    void deactivate()
    {
        setActive(false);
        m_nextActiveTime = std::chrono::steady_clock::now() + std::chrono::milliseconds(activePeriodMs);
    }

public:
    Trigger_Respawning()
    {
        activePeriodMs = 5000;
        m_nextActiveTime = std::chrono::steady_clock::now() + std::chrono::milliseconds(activePeriodMs);
    }

    virtual ~Trigger_Respawning(){}

    //to be implemented by child classes
    virtual void tryCheck(entity_type*) = 0;

    //this is called each game-tick to update the trigger's internal state
    virtual void update(float dt)
    {
        if ( !isActive() && std::chrono::steady_clock::now() > m_nextActiveTime)
        {
            setActive(true);
        }
    }

    void setRespawnDelay(unsigned int milliseconds) {activePeriodMs = milliseconds;}
};


#endif