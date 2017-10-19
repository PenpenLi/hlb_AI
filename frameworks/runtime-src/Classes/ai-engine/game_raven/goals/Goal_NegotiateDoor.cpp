#include "Goal_NegotiateDoor.h"
#include "..\Raven_Bot.h"
#include "..\GameWorldRaven.h"
#include "../navigation/Raven_PathPlanner.h" 
#include "Goal_MoveToPosition.h"
#include "Goal_TraverseEdge.h"



//---------------------------- ctor -------------------------------------------
//-----------------------------------------------------------------------------
Goal_NegotiateDoor::Goal_NegotiateDoor(Raven_Bot* pBot, PathEdge edge,bool LastEdge):
                                                                            GoalComposite<Raven_Bot>(pBot, goal_negotiate_door),
                                                                            m_PathEdge(edge),
                                                                            m_bLastEdgeInPath(LastEdge)

{
}

//---------------------------- activate ---------------------------------------
//-----------------------------------------------------------------------------
void Goal_NegotiateDoor::activate()
{
    m_iStatus = active;

    //if this goal is reactivated then there may be some existing subgoals that
    //must be removed
    removeAllSubgoals();
  
    //get the position of the closest navigable switch
    Vector2D posSw = m_pOwner->getWorld()->getPosOfClosestSwitch(m_pOwner->getPos(),m_PathEdge.getDoorID());

    //because goals are *pushed* onto the front of the subgoal list they must
    //be added in reverse order.

    //first the goal to traverse the edge that passes through the door
    addSubgoal(new Goal_TraverseEdge(m_pOwner, m_PathEdge, m_bLastEdgeInPath));

    //next, the goal that will move the bot to the beginning of the edge that
    //passes through the door
    addSubgoal(new Goal_MoveToPosition(m_pOwner, m_PathEdge.getSource()));

    //finally, the Goal that will direct the bot to the location of the switch
    addSubgoal(new Goal_MoveToPosition(m_pOwner, posSw));
}


//------------------------------ Process --------------------------------------
//-----------------------------------------------------------------------------
int Goal_NegotiateDoor::process()
{
    //if status is inactive, call activate()
    activateIfInactive();

    //process the subgoals
    m_iStatus = processSubgoals();

    return m_iStatus;
}


