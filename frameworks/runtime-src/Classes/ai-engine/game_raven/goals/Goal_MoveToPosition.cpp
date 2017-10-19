#include "Goal_MoveToPosition.h"
#include "Messaging/Telegram.h"
#include "game_raven/RavenMessages.h"
#include "Goal_SeekToPosition.h"
#include "Goal_FollowPath.h"



//------------------------------- activate ------------------------------------
//-----------------------------------------------------------------------------
void Goal_MoveToPosition::activate()
{
    m_iStatus = active;

    //make sure the subgoal list is clear.
    removeAllSubgoals();

    //requests a path to the target position from the path planner. Because, for
    //demonstration purposes, the Raven path planner uses time-slicing when 
    //processing the path requests the bot may have to wait a few update cycles
    //before a path is calculated. Consequently, for appearances sake, it just
    //seeks directly to the target position whilst it's awaiting notification
    //that the path planning request has succeeded/failed
    if (m_pOwner->getPathPlanner()->requestPathToPosition(m_vDestination))
    {
        addSubgoal(new Goal_SeekToPosition(m_pOwner, m_vDestination));
    }
}

//------------------------------ Process --------------------------------------
//-----------------------------------------------------------------------------
int Goal_MoveToPosition::process()
{
    //if status is inactive, call activate()
    activateIfInactive();
    
    //process the subgoals
    m_iStatus = processSubgoals();

    //if any of the subgoals have failed then this goal re-plans
    reactivateIfFailed();

    return m_iStatus;
}

//---------------------------- HandleMessage ----------------------------------
//-----------------------------------------------------------------------------
bool Goal_MoveToPosition::handleMessage(const Telegram& msg)
{
    //first, pass the message down the goal hierarchy
    bool bHandled = forwardMessageToFrontMostSubgoal(msg);

    //if the msg was not handled, test to see if this goal can handle it
    if (bHandled == false)
    {
        switch(msg.m_msgId)
        {
            case Msg_PathReady:
            {
                //clear any existing goals
                removeAllSubgoals();
                addSubgoal(new Goal_FollowPath(m_pOwner,m_pOwner->getPathPlanner()->getPath()));

                return true; //msg handled
            }

            case Msg_NoPathAvailable:
            {
                m_iStatus = failed;
                return true; //msg handled
            }
            
            default: 
                return false;
        }
    }

    //handled by subgoals
    return true;
}


