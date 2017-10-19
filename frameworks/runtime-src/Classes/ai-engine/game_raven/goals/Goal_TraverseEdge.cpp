#include "Goal_TraverseEdge.h"
#include "Raven_Goal_Types.h"
#include "../Raven_SteeringBehaviors.h"
#include "../navigation/Raven_PathPlanner.h"
#include "common/misc/LogDebug.h"



//---------------------------- ctor -------------------------------------------
//-----------------------------------------------------------------------------
Goal_TraverseEdge::Goal_TraverseEdge(Raven_Bot* pBot,PathEdge edge, bool isLastEdge):
                                                                                Goal<Raven_Bot>(pBot, goal_traverse_edge),
                                                                                m_Edge(edge),
                                                                                m_bLastEdgeInPath(isLastEdge)
                            
{
}

                            
                                             
//---------------------------- activate -------------------------------------
//-----------------------------------------------------------------------------  
void Goal_TraverseEdge::activate()
{
    m_iStatus = active;

    //the edge behavior flag may specify a type of movement that necessitates a 
    //change in the bot's max possible speed as it follows this edge
    switch(m_Edge.Behavior())
    {
        case NavGraphEdge::swim:
        {
            m_pOwner->setMaxSpeed(Para_Bot_MaxSwimmingSpeed));
            //show animation...
        }
        break;

        case NavGraphEdge::crawl:
        {
            m_pOwner->setMaxSpeed(Para_Bot_MaxCrawlingSpeed);
             //show animation...
        }
        break;
    }
  

    //record the time the bot starts this goal
    m_dStartTime = std::chrono::steady_clock::now();

    //factor in a margin of error for any reactive behavior
    static const float MarginOfError = 2.0f;

    //calculate the expected time required to reach the this waypoint. This value
    //is used to determine if the bot becomes stuck 
    float sec = m_pOwner->calculateTimeToReachPosition(m_Edge.getDestination());
    m_dTimeExpected = m_dStartTime + std::chrono::milliseconds((int)((sec+MarginOfError)*1000));
  
    //set the steering target
    m_pOwner->getSteering()->setTarget(m_Edge.getDestination());

    //Set the appropriate steering behavior. If this is the last edge in the path
    //the bot should arrive at the position it points to, else it should seek
    if (m_bLastEdgeInPath)
    {
        m_pOwner->getSteering()->arriveOn();
    }
    else
    {
        m_pOwner->getSteering()->seekOn();
    }
}



//------------------------------ process --------------------------------------
//-----------------------------------------------------------------------------
int Goal_TraverseEdge::process()
{
    //if status is inactive, call activate()
    activateIfInactive();

    //if the bot has become stuck return failure
    if (isStuck())
    {
        m_iStatus = failed;
    }
    //if the bot has reached the end of the edge return completed
    else
    { 
        if (m_pOwner->isAtPosition(m_Edge.getDestination()))
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
bool Goal_TraverseEdge::isStuck()const
{  
    if (std::chrono::steady_clock::now() > m_dTimeExpected)
    {
        AILOG("BOT %d is stuck !!", m_pOwner->getID());
        return true;
    }
    
    return false;
}

//---------------------------- terminate --------------------------------------
//-----------------------------------------------------------------------------
void Goal_TraverseEdge::terminate()
{
    //turn off steering behaviors.
    m_pOwner->getSteering()->seekOff();
    m_pOwner->getSteering()->arriveOff();

    //return max speed back to normal
    m_pOwner->setMaxSpeed(Para_Bot_MaxSpeed);
}

