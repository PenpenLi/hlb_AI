
#include "Goal_Wander.h"
#include "Raven_SteeringBehaviors.h"


void Goal_Wander::activate()
{
    m_iStatus = active;

    m_pOwner->getSteering()->wanderOn();
}

int Goal_Wander::process()
{
    //if status is inactive, call Activate()
    activateIfInactive();

    return m_iStatus;
}

void Goal_Wander::terminate()
{
    m_pOwner->getSteering()->wanderOff();
}

