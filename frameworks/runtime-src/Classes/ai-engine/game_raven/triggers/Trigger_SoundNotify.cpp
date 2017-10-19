#include "Trigger_SoundNotify.h"
#include "game_raven/RavenMessages.h"

//------------------------------ ctor -----------------------------------------
//-----------------------------------------------------------------------------

Trigger_SoundNotify::Trigger_SoundNotify(Raven_Bot* source, 
                                   float range):Trigger_LimitedLifetime<Raven_Bot>(Para_Weaopn_Sound_LiftTime),
                                                       m_pSoundSource(source)
{
    setPos(m_pSoundSource->getPos());
    
    //create and set this trigger's region of fluence
    addCircularTriggerRegion(getPos(), range);
}


//------------------------------ tryCheck ------------------------------------------
//
//  when triggered this trigger adds the bot that made the source of the sound 
//  to the triggering bot's perception.
//-----------------------------------------------------------------------------
void Trigger_SoundNotify::tryCheck(Raven_Bot* pBot) 
{
    //is this bot within range of this sound
    if (isTouchingTrigger(pBot->getPos(), pBot->getBoundingRadius()))
    {
        MessageDispatcher::instance()->dispatchMsg( 0, 
                                                                            -1, 
                                                                            pBot->getID(),
                                                                            Msg_GunshotSound, 
                                                                            m_pSoundSource);
    }   
}

