#include "Goal_DodgeSideToSide.h"
#include "Goal_SeekToPosition.h"
#include "../Raven_Bot.h"
#include "../Raven_SteeringBehaviors.h"
#include "../GameWorldRaven.h"
#include "common/message/Telegram.h"
#include "../RavenMessages.h"



//------------------------------- activate ------------------------------------
//-----------------------------------------------------------------------------
void Goal_DodgeSideToSide::activate()
{
    m_iStatus = active;

    m_pOwner->getSteering()->seekOn();

  
    if (m_bClockwise)
    {
        if (m_pOwner->canStepRight(m_vStrafeTarget))
        {
            m_pOwner->getSteering()->setTarget(m_vStrafeTarget);
        }
        else
        {
            m_bClockwise = !m_bClockwise;
            m_iStatus = inactive;
        }
    }
    else
    {
        if (m_pOwner->canStepLeft(m_vStrafeTarget))
        {
            m_pOwner->getSteering()->setTarget(m_vStrafeTarget);
        }
        else
        {
            // debug_con << "changing" << "";
            m_bClockwise = !m_bClockwise;
            m_iStatus = inactive;
        }
    }
}



//-------------------------- process ------------------------------------------
//-----------------------------------------------------------------------------
int Goal_DodgeSideToSide::process()
{
    //if status is inactive, call activate()
    activateIfInactive(); 

    //if target goes out of view terminate
    if (!m_pOwner->getTargetSys()->isTargetWithinFOV())
    {
        m_iStatus = completed;
    }

    //else if bot reaches the target position set status to inactive so the goal 
    //is reactivated on the next update-step
    else if (m_pOwner->isAtPosition(m_vStrafeTarget))
    {
        m_iStatus = inactive;
    }

    return m_iStatus;
}

//---------------------------- terminate --------------------------------------
//-----------------------------------------------------------------------------
void Goal_DodgeSideToSide::terminate()
{
    m_pOwner->getSteering()->seekOff();
}


