#include "Goal_HuntTarget.h"
#include "Goal_Explore.h"
#include "Goal_MoveToPosition.h"
#include "..\Raven_Bot.h"
#include "..\Raven_SteeringBehaviors.h"


//---------------------------- Initialize -------------------------------------
//-----------------------------------------------------------------------------  
void Goal_HuntTarget::activate()
{
    m_iStatus = active;

    //if this goal is reactivated then there may be some existing subgoals that
    //must be removed
    removeAllSubgoals();

    //it is possible for the target to die whilst this goal is active so we
    //must test to make sure the bot always has an active target
    if (m_pOwner->getTargetSys()->isTargetPresent())
    {
        //grab a local copy of the last recorded position (LRP) of the target
        const Vector2D lrp = m_pOwner->getTargetSys()->getLastRecordedPosition();

        //if the bot has reached the LRP and it still hasn't found the target
        //it starts to search by using the explore goal to move to random
        //map locations
        if (lrp.isZero() || m_pOwner->isAtPosition(lrp))
        {
            addSubgoal(new Goal_Explore(m_pOwner));
        }
        //else move to the LRP
        else
        {
            addSubgoal(new Goal_MoveToPosition(m_pOwner, lrp));
        }
    }
    //if their is no active target then this goal can be removed from the queue
    else
    {
        m_iStatus = completed;
    }
}

//------------------------------ process --------------------------------------
//-----------------------------------------------------------------------------
int Goal_HuntTarget::process()
{
    //if status is inactive, call activate()
    activateIfInactive();

    m_iStatus = processSubgoals();

    //if target is in view this goal is satisfied
    if (m_pOwner->getTargetSys()->isTargetWithinFOV())
    {
        m_iStatus = completed;
    }

    return m_iStatus;
}
