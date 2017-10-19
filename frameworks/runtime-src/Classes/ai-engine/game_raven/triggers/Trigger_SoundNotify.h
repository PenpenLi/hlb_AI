#ifndef TRIGGER_SOUNDNOTIFY_H
#define TRIGGER_SOUNDNOTIFY_H
#pragma warning (disable:4786)
//-----------------------------------------------------------------------------
//
//  Name:     Trigger_SoundNotify.h
//
//  Desc:     whenever an agent makes a sound -- such as when a weapon fires --
//            this trigger can be used to notify other bots of the event.
//
//            This type of trigger has a circular trigger region and a lifetime
//            of 1 update-step
//
//-----------------------------------------------------------------------------
#include "common/triggers/Trigger_LimitedLifetime.h"
#include "../misc/Raven_Bot.h"



class Trigger_SoundNotify : public Trigger_LimitedLifetime<Raven_Bot>
{
public:
    Trigger_SoundNotify(Raven_Bot* source, float range);
    
    void tryCheck(Raven_Bot*);
    
private:
    //a pointer to the bot that has made the sound
    Raven_Bot* m_pSoundSource;
};




#endif