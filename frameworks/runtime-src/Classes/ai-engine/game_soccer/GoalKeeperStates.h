#ifndef KEEPERSTATES_H
#define KEEPERSTATES_H
//------------------------------------------------------------------------
//
//  Name: GoalKeeperStates.h
//
//  Desc:   Declarations of all the states used by a Simple Soccer goalkeeper
//
//------------------------------------------------------------------------
#include <string>
#include "common/fsm/State.h"
#include "common/message/Telegram.h"


class GoalKeeper;
class SoccerPitch;


class GlobalKeeperState: public State<GoalKeeper>
{
private:
    GlobalKeeperState(){}

public:
    //this is a singleton
    static GlobalKeeperState* instance();

    void onEnter(GoalKeeper* keeper){}

    void onExecute(GoalKeeper* keeper){}

    void onExit(GoalKeeper* keeper){}

    bool onMessage(GoalKeeper*, const Telegram&);
};

//-----------------------------------------------------------------------------

class TendGoal: public State<GoalKeeper>
{
private:
  TendGoal(){}

public:
    //this is a singleton
    static TendGoal* instance();

    void onEnter(GoalKeeper* keeper);

    void onExecute(GoalKeeper* keeper);

    void onExit(GoalKeeper* keeper);

    bool onMessage(GoalKeeper*, const Telegram&){return false;}
};

//------------------------------------------------------------------------
class InterceptBall: public State<GoalKeeper>
{
private:
    InterceptBall(){}

public:

    //this is a singleton
    static InterceptBall* instance();

    void onEnter(GoalKeeper* keeper);

    void onExecute(GoalKeeper* keeper);

    void onExit(GoalKeeper* keeper);

    bool onMessage(GoalKeeper*, const Telegram&){return false;}
};

//------------------------------------------------------------------------
class ReturnHome: public State<GoalKeeper>
{
private:
    ReturnHome(){}

public:
    //this is a singleton
    static ReturnHome* instance();

    void onEnter(GoalKeeper* keeper);

    void onExecute(GoalKeeper* keeper);

    void onExit(GoalKeeper* keeper);

    bool onMessage(GoalKeeper*, const Telegram&){return false;}
};

//------------------------------------------------------------------------
class PutBallBackInPlay: public State<GoalKeeper>
{
private:
    PutBallBackInPlay(){}

public:
    //this is a singleton
    static PutBallBackInPlay* instance();

    void onEnter(GoalKeeper* keeper);

    void onExecute(GoalKeeper* keeper);

    void onExit(GoalKeeper* keeper){}

    bool onMessage(GoalKeeper*, const Telegram&){return false;}
};





#endif