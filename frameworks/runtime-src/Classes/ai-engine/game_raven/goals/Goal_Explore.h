#ifndef GOAL_EXPLORE_H
#define GOAL_EXPLORE_H
#pragma warning (disable:4786)

#include "common/goals/GoalComposite.h"
#include "Raven_Goal_Types.h"


class Raven_Bot;

class Goal_Explore : public GoalComposite<Raven_Bot>
{
public:
    Goal_Explore(Raven_Bot* pOwner):GoalComposite<Raven_Bot>(pOwner,goal_explore),
                                                               m_bDestinationIsSet(false)
    {
    }

    void activate();

    int process();

    void terminate(){}

    bool handleMessage(const Telegram& msg);
    
private:
    Vector2D m_CurrentDestination;

    //set to true when the destination for the exploration has been established
    bool m_bDestinationIsSet;    
};


#endif
