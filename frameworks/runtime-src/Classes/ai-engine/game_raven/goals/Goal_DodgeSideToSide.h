#ifndef GOAL_DODGE_SIDE_H
#define GOAL_DODGE_SIDE_H
#pragma warning (disable:4786)
//-----------------------------------------------------------------------------
//
//  Name:   Goal_DodgeSideToSide.h
//
//  Desc:   this goal makes the bot dodge from side to side
//
//-----------------------------------------------------------------------------
#include "common/goals/Goal.h"
#include "common/misc/UtilsEx.h"
#include "Raven_Goal_Types.h"
#include "../Raven_Bot.h"



class Goal_DodgeSideToSide : public Goal<Raven_Bot>
{
public:
    Goal_DodgeSideToSide(Raven_Bot* pBot):Goal<Raven_Bot>(pBot, goal_strafe),
                                    m_bClockwise(RandBool())
    {
    }
    
    void activate();

    int process();

    void terminate();
    
private:
    Vector2D m_vStrafeTarget;
    bool m_bClockwise;
    Vector2D getStrafeTarget()const;    
};






#endif
