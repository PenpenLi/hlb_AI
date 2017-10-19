#ifndef GOAL_SEEK_TO_POSITION_H
#define GOAL_SEEK_TO_POSITION_H
#pragma warning (disable:4786)

#include "common/goals/Goal.h"
#include "common/2D/Vector2D.h"
#include "Raven_Goal_Types.h"
#include "game_raven/Raven_Bot.h"
#include <chrono>


class Goal_SeekToPosition : public Goal<Raven_Bot>
{
public:
    Goal_SeekToPosition(Raven_Bot* pBot, Vector2D target);

    //the usual suspects
    void activate();
    
    int  process();
    
    void terminate();
    
private:
    //the position the bot is moving to
    Vector2D m_vPosition;

    //this records the time this goal was activated
    std::chrono::steady_clock::time_point m_dStartTime;

    //the approximate time the bot should take to travel the target location
    std::chrono::steady_clock::time_point m_dTimeToReachPos;
    
    //returns true if a bot gets stuck
    bool isStuck()const;
};




#endif

