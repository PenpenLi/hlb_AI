#ifndef TRIGGER_LIMITEDLIFETIME_H
#define TRIGGER_LIMITEDLIFETIME_H
#pragma warning (disable:4786)
//-----------------------------------------------------------------------------
//
//  Name:     Trigger_LimitedLifetime.h
//
//
//  Desc:     defines a trigger that only remains in the game for a specified
//            number of update steps
//
//-----------------------------------------------------------------------------
#include "Trigger.h"
#include <chrono>

template <class entity_type>
class Trigger_LimitedLifetime : public Trigger<entity_type>
{
public:
    Trigger_LimitedLifetime(int lifetime)
    {
        m_deadTime = std::chrono::steady_clock::now() + std::chrono::seconds(lifetime);
    }

    virtual ~Trigger_LimitedLifetime(){}

    //children of this class should always make sure this is called from within
    //their own update method
    virtual void update(float dt)
    {
        //if the lifetime counter expires set this trigger to be removed from
        //the game
        if (std::chrono::steady_clock::now() >= m_deadTime)
        {
            setToBeRemovedFromGame();
        }
    }

    //to be implemented by child classes
    virtual void tryCheck(entity_type*) = 0;

protected:
    //the lifetime of this trigger in seconds
    std::chrono::steady_clock::time_point m_deadTime;
};




#endif