#ifndef TEAMSTATES_H
#define TEAMSTATES_H
//------------------------------------------------------------------------
//
//  Name: TeamStates.h
//
//  Desc: State prototypes for soccer team states
//
//
//------------------------------------------------------------------------
#include <string>

#include "common/fsm/State.h"
#include "common/message/Telegram.h"


class SoccerTeam;


//------------------------------------------------------------------------
class Attacking : public State<SoccerTeam>
{ 
public:
    static Attacking* instance();

    void onEnter(SoccerTeam* team);

    void onExecute(SoccerTeam* team);

    void onExit(SoccerTeam* team);

    bool onMessage(SoccerTeam*, const Telegram&){return false;}

private:
    Attacking(){}    
};

//------------------------------------------------------------------------
class Defending : public State<SoccerTeam>
{ 
public:
    //this is a singleton
    static Defending* instance();

    void onEnter(SoccerTeam* team);

    void onExecute(SoccerTeam* team);

    void onExit(SoccerTeam* team);

    bool onMessage(SoccerTeam*, const Telegram&){return false;}

private:
    Defending(){}
};

//------------------------------------------------------------------------
class PrepareForKickOff : public State<SoccerTeam>
{ 
public:
    //this is a singleton
    static PrepareForKickOff* instance();

    void onEnter(SoccerTeam* team);

    void onExecute(SoccerTeam* team);

    void onExit(SoccerTeam* team);
    
    bool onMessage(SoccerTeam*, const Telegram&){return false;}

private:
    PrepareForKickOff(){}
};


#endif