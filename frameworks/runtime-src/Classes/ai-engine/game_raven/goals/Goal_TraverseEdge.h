#ifndef GOAL_TRAVERSE_EDGE_H
#define GOAL_TRAVERSE_EDGE_H
#pragma warning (disable:4786)

#include "common/goals/Goal.h"
#include "common/2D/Vector2D.h"
#include "common/navigation/Raven_PathPlanner.h"
#include "common/navigation/PathEdge.h"
#include "game_raven/Raven_Bot.h"
#include <chrono>


class Goal_TraverseEdge : public Goal<Raven_Bot>
{
public:
    Goal_TraverseEdge(Raven_Bot* pBot,PathEdge edge, bool isLastEdge); 

    //the usual suspects
    void activate();
    
    int  process();
    
    void terminate();
    
private:
    //the edge the bot will follow
    PathEdge m_Edge;

    //true if m_Edge is the last in the path.
    bool m_bLastEdgeInPath;

    //the estimated time the bot should take to traverse the edge
    std::chrono::steady_clock::time_point m_dTimeExpected;

    //this records the time this goal was activated
    std::chrono::steady_clock::time_point m_dStartTime;

    //returns true if the bot gets stuck
    bool isStuck()const;    
};




#endif

