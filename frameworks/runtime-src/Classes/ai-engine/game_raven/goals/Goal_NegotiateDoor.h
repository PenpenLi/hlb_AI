#ifndef GOAL_NEGOTIATE_DOOR_H
#define GOAL_NEGOTIATE_DOOR_H
#pragma warning (disable:4786)

#include "common/goals/GoalComposite.h"
#include "Raven_Goal_Types.h"
#include "../Raven_Bot.h"
#include "../navigation/PathEdge.h"


class Goal_NegotiateDoor : public GoalComposite<Raven_Bot>
{
public:
    Goal_NegotiateDoor(Raven_Bot* pBot, PathEdge edge, bool LastEdge);

    //the usual suspects
    void activate();
    
    int  process();
    
    void terminate(){}

private:
    PathEdge m_PathEdge;

    bool m_bLastEdgeInPath;    
};

#endif
