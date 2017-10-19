#include "Goal_FollowPath.h"
#include "Goal_TraverseEdge.h"
#include "Goal_NegotiateDoor.h"


Goal_FollowPath::Goal_FollowPath(Raven_Bot* pBot,std::list<PathEdge> path):
                                                            GoalComposite<Raven_Bot>(pBot, goal_follow_path),
                                                            m_Path(path)
{
}


//------------------------------ activate -------------------------------------
//-----------------------------------------------------------------------------
void Goal_FollowPath::activate()
{
    m_iStatus = active;

    //get a reference to the next edge
    PathEdge edge = m_Path.front();

    //remove the edge from the path
    m_Path.pop_front(); 

    //some edges specify that the bot should use a specific behavior when
    //following them. This switch statement queries the edge behavior flag and
    //adds the appropriate goals/s to the subgoal list.
    switch(edge.Behavior())
    {
        case NavGraphEdge::normal:
        {
            addSubgoal(new Goal_TraverseEdge(m_pOwner, edge, m_Path.empty()));
            break;
        }

        case NavGraphEdge::goes_through_door:
        {
            //also add a goal that is able to handle opening the door
            addSubgoal(new Goal_NegotiateDoor(m_pOwner, edge, m_Path.empty()));
            break;
        }
        
        case NavGraphEdge::jump:
        {
            //add subgoal to jump along the edge
            break;
        }

        case NavGraphEdge::grapple:
        {
            //add subgoal to grapple along the edge
            break;
        }

        default:
            throw std::runtime_error("<Goal_FollowPath::activate>: Unrecognized edge type");
    }
}


//-------------------------- Process ------------------------------------------
//-----------------------------------------------------------------------------
int Goal_FollowPath::process()
{
    //if status is inactive, call activate()
    activateIfInactive();

    m_iStatus = processSubgoals();

    //if there are no subgoals present check to see if the path still has edges.
    //remaining. If it does then call activate to grab the next edge.
    if (m_iStatus == completed && !m_Path.empty())
    {
        activate(); 
    }

    return m_iStatus;
}

