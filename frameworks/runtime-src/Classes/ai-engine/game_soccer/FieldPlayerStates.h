#ifndef FIELDPLAYERSTATES_H
#define FIELDPLAYERSTATES_H
//------------------------------------------------------------------------
//
//  Name: FieldPlayerStates.h
//
//  Desc: States for the field players of Simple Soccer. See my book
//              for detailed descriptions
//
//------------------------------------------------------------------------

#include <string>

#include "common/fsm/State.h"
//#include "common/message/Telegram.h"


class FieldPlayer;
class SoccerPitch;
struct Telegram;

//------------------------------------------------------------------------
class GlobalPlayerState :public State<FieldPlayer>
{
private:
    GlobalPlayerState(){}

public:
    //this is a singleton
    static GlobalPlayerState* instance();

    void onEnter(FieldPlayer* player){}

    void onExecute(FieldPlayer* player);

    void onExit(FieldPlayer* player){}

    bool onMessage(FieldPlayer*, const Telegram&);
};

//------------------------------------------------------------------------
class ChaseBall : public State<FieldPlayer>
{
private:
    ChaseBall(){}

public:

    //this is a singleton
    static ChaseBall* instance();

    void onEnter(FieldPlayer* player);

    void onExecute(FieldPlayer* player);

    void onExit(FieldPlayer* player);

    bool onMessage(FieldPlayer*, const Telegram&){return false;}
};

//------------------------------------------------------------------------
class Dribble : public State<FieldPlayer>
{
private:
    Dribble(){}

public:
    //this is a singleton
    static Dribble* instance();

    void onEnter(FieldPlayer* player);

    void onExecute(FieldPlayer* player);

    void onExit(FieldPlayer* player){}

    bool onMessage(FieldPlayer*, const Telegram&){return false;}
};


//------------------------------------------------------------------------
class ReturnToHomeRegion: public State<FieldPlayer>
{
private:
    ReturnToHomeRegion(){}

public:
    //this is a singleton
    static ReturnToHomeRegion* instance();

    void onEnter(FieldPlayer* player);

    void onExecute(FieldPlayer* player);

    void onExit(FieldPlayer* player);

    bool onMessage(FieldPlayer*, const Telegram&){return false;}
};

//------------------------------------------------------------------------
class Wait: public State<FieldPlayer>
{
private:
    Wait(){}

public:

    //this is a singleton
    static Wait* instance();

    void onEnter(FieldPlayer* player);

    void onExecute(FieldPlayer* player);

    void onExit(FieldPlayer* player);

    bool onMessage(FieldPlayer*, const Telegram&){return false;}
};

//------------------------------------------------------------------------
class KickBall: public State<FieldPlayer>
{
private:
    KickBall(){}

public:

    //this is a singleton
    static KickBall* instance();

    void onEnter(FieldPlayer* player);

    void onExecute(FieldPlayer* player);

    void onExit(FieldPlayer* player){}

    bool onMessage(FieldPlayer*, const Telegram&){return false;}
};

//------------------------------------------------------------------------
class ReceiveBall: public State<FieldPlayer>
{
private:
    ReceiveBall(){}

public:
    //this is a singleton
    static ReceiveBall* instance();

    void onEnter(FieldPlayer* player);

    void onExecute(FieldPlayer* player);

    void onExit(FieldPlayer* player);

    bool onMessage(FieldPlayer*, const Telegram&){return false;}
};


//------------------------------------------------------------------------
class SupportAttacker: public State<FieldPlayer>
{
private:
    SupportAttacker(){}

public:
    //this is a singleton
    static SupportAttacker* instance();

    void onEnter(FieldPlayer* player);

    void onExecute(FieldPlayer* player);

    void onExit(FieldPlayer* player);

    bool onMessage(FieldPlayer*, const Telegram&){return false;}
};

#endif