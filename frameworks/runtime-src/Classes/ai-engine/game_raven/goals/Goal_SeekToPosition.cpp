#include "Goal_SeekToPosition.h"
#include "Raven_SteeringBehaviors.h"
#include "game_raven/navigation/Raven_PathPlanner.h"
#include "common/misc/LogDebug.h"


//-----------------------------------------------------------------------------
Goal_SeekToPosition::Goal_SeekToPosition(Raven_Bot* pBot, Vector2D target):
                                                                                Goal<Raven_Bot>(pBot,goal_seek_to_position),
                                                                                m_vPosition(target)
{
}

                                             
//---------------------------- Activate -------------------------------------
//-----------------------------------------------------------------------------  
void Goal_SeekToPosition::activate()
{
    m_iStatus = active;

    //record the time the bot starts this goal
    m_dStartTime = std::chrono::steady_clock::now();

     //factor in a margin of error for any reactive behavior
    static const float MarginOfError = 1.0f;

    //This value is used to determine if the bot becomes stuck 
    float sec = m_pOwner->calculateTimeToReachPosition(m_vPosition);
    m_dTimeToReachPos = m_dStartTime + std::chrono::milliseconds((int)((sec+MarginOfError)*1000));

    m_pOwner->getSteering()->setTarget(m_vPosition);

    m_pOwner->getSteering()->seekOn();
}


//------------------------------ Process --------------------------------------
//-----------------------------------------------------------------------------
int Goal_SeekToPosition::process()
{
    //if status is inactive, call Activate()
    activateIfInactive();
    
    //test to see if the bot has become stuck
    if (isStuck())
    {
        m_iStatus = failed;
    }
    //test to see if the bot has reached the waypoint. If so terminate the goal
    else
    { 
        if (m_pOwner->isAtPosition(m_vPosition))
        {
            m_iStatus = completed;
        }
    }

    return m_iStatus;
}

//--------------------------- isBotStuck --------------------------------------
//
//  returns true if the bot has taken longer than expected to reach the 
//  currently active waypoint
//-----------------------------------------------------------------------------
bool Goal_SeekToPosition::isStuck()const
{  
    if (std::chrono::steady_clock::now() > m_dTimeToReachPos)
    {
        AILOG("BOT %d is stuck !!", m_pOwner->getID());
        return true;
    }

    return false;
}

//---------------------------- Terminate --------------------------------------
//-----------------------------------------------------------------------------
void Goal_SeekToPosition::terminate()
{
    m_pOwner->getSteering()->seekOff();
    m_pOwner->getSteering()->arriveOff();

    m_iStatus = completed;
}

