#ifndef GOAL_FOLLOWPATH_H
#define GOAL_FOLLOWPATH_H
#pragma warning (disable:4786)
//-----------------------------------------------------------------------------
//
//  Name:   Goal_FollowPath.h
//
//
//  Desc:
//-----------------------------------------------------------------------------
#include "common/goals/GoalComposite.h"
#include "Raven_Goal_Types.h"
#include "game_raven/Raven_Bot.h"
#include "common/navigation/Raven_PathPlanner.h"
#include "common/navigation/PathEdge.h"



class Goal_FollowPath : public GoalComposite<Raven_Bot>
{
public:
    Goal_FollowPath(Raven_Bot* pBot, std::list<PathEdge> path);
    //the usual suspects
    void activate();
    int process();
    void terminate(){}
  
private:
    //a local copy of the path returned by the path planner
    std::list<PathEdge> m_Path;  
};

#endif

