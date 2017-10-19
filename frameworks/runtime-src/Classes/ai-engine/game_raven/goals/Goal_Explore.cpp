#include "Goal_Explore.h"
#include "../Raven_Bot.h"
#include "../navigation/Raven_PathPlanner.h"
#include "../GameWorldRaven.h"
#include "../Raven_Map.h"
#include "../RavenMessages.h"
#include "common/message/Telegram.h"
#include "Goal_SeekToPosition.h"
#include "Goal_FollowPath.h"



//------------------------------ activate -------------------------------------
//-----------------------------------------------------------------------------
void Goal_Explore::activate()
{
    m_iStatus = active;

    //if this goal is reactivated then there may be some existing subgoals that
    //must be removed
    removeAllSubgoals();

    if (!m_bDestinationIsSet)
    {
        //grab a random location
        m_CurrentDestination = m_pOwner->getWorld()->getMap()->getRandomNodeLocation();

        m_bDestinationIsSet = true;
    }

    //and request a path to that position
    m_pOwner->getPathPlanner()->requestPathToPosition(m_CurrentDestination);

    //the bot may have to wait a few update cycles before a path is calculated
    //so for appearances sake it simple ARRIVES at the destination until a path
    //has been found
    addSubgoal(new Goal_SeekToPosition(m_pOwner, m_CurrentDestination));
}

//------------------------------ Process -------------------------------------
//-----------------------------------------------------------------------------
int Goal_Explore::process()
{
    //if status is inactive, call activate()
    activateIfInactive();

    //process the subgoals
    m_iStatus = processSubgoals();

    return m_iStatus;
}


//---------------------------- handleMessage ----------------------------------
//-----------------------------------------------------------------------------
bool Goal_Explore::handleMessage(const Telegram& msg)
{
    //first, pass the message down the goal hierarchy
    bool bHandled = forwardMessageToFrontMostSubgoal(msg);

    //if the msg was not handled, test to see if this goal can handle it
    if (bHandled == false)
    {
        switch(msg.m_msgId)
        {
        case Msg_PathReady:
            //clear any existing goals
            removeAllSubgoals();
            addSubgoal(new Goal_FollowPath(m_pOwner, m_pOwner->getPathPlanner()->getPath()));
            return true; //msg handled


        case Msg_NoPathAvailable:
            m_iStatus = failed;
            return true; //msg handled

        default: 
            return false;
        }
    }
    
    //handled by subgoals
    return true;
}




