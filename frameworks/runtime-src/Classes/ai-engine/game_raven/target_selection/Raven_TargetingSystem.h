#ifndef TARGETING_SELECTION_H
#define TARGETING_SELECTION_H
#pragma warning (disable:4786)
//-----------------------------------------------------------------------------
//
//  Name:   Raven_TargetingSystem.h
//
//
//  Desc:   class to select a target from the opponents currently in a bot's
//          perceptive memory.
//-----------------------------------------------------------------------------
#include "common/2D/Vector2D.h"
#include <list>


class Raven_Bot;


class Raven_TargetingSystem
{
public:
    Raven_TargetingSystem(Raven_Bot* owner);

    //each time this method is called the opponents in the owner's sensory 
    //memory are examined and the closest  is assigned to m_pCurrentTarget.
    //if there are no opponents that have had their memory records updated
    //within the memory span of the owner then the current target is set
    //to null
    void update();

    //returns true if there is a currently assigned target
    bool isTargetPresent()const{return m_pCurrentTarget != nullptr;}

    //returns true if the target is within the field of view of the owner
    bool isTargetWithinFOV()const;

    //returns true if there is unobstructed line of sight between the target
    //and the owner
    bool isTargetShootable()const;

    //returns the position the target was last seen. Throws an exception if
    //there is no target currently assigned
    Vector2D getLastRecordedPosition()const;

    //returns the amount of time the target has been in the field of view
    float getTimeTargetHasBeenVisible()const;

    //returns the amount of time the target has been out of view
    float getTimeTargetHasBeenOutOfView()const;

    //returns a pointer to the target. null if no target current.
    Raven_Bot* getTarget()const{return m_pCurrentTarget;}

    //sets the target pointer to null
    void clearTarget(){m_pCurrentTarget = nullptr;}

private:
    //the owner of this system
    Raven_Bot* m_pOwner;

    //the current target (this will be null if there is no target assigned)
    Raven_Bot* m_pCurrentTarget;
};


#endif
