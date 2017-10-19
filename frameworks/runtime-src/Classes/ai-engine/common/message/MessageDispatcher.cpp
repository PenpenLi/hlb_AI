
#include "MessageDispatcher.h"
#include "common/game/BaseEntity.h"
#include "common/game/EntityManager.h"
#include "common/misc/LogDebug.h"

using std::set;

MessageDispatcher* MessageDispatcher::instance()
{
    static MessageDispatcher instance; 

    return &instance;
}

//----------------------------- Dispatch ---------------------------------
//  see description in header
//------------------------------------------------------------------------
void MessageDispatcher::execute(BaseEntity* pReceiver, const Telegram& telegram)
{
    if (!pReceiver->handleMessage(telegram))
    {
        AILOG("[MessageDispatcher::execute] message no handled");  
    }
}

//---------------------------- DispatchMsg ---------------------------
//
//  given a message, a receiver, a sender and any time delay, this function
//  routes the message to the correct agent (if no delay) or stores
//  in the message queue to be dispatched at the correct time
//------------------------------------------------------------------------
void MessageDispatcher::dispatchMsg(float delay,
                                                               int sender,
                                                               int receiver,
                                                               int msg,
                                                               void* extraInfo)
{
    //get a pointer to the receiver
    BaseEntity* pReceiver = EntityManager::instance()->getEntityByID(receiver);

    //make sure the receiver is valid
    if (pReceiver == NULL)
    {
        AILOG("Warning! No Receiver with ID of  %d found", receiver);
        return;
    }
    
    //create the telegram
    Telegram telegram(delay, sender, receiver, msg, extraInfo);
  
    //if there is no delay, route telegram immediately                       
    if (delay <= 0.0)                                                        
    {
        //send the telegram to the recipient
        execute(pReceiver, telegram);
    }
    else //calculate the time when the telegram should be dispatched
    {
        m_delayQueue.push(telegram);
    }
}

//---------------------- DispatchDelayedMessages -------------------------
//
//  This function dispatches any telegrams with a timestamp that has
//  expired. Any dispatched telegrams are removed from the queue
//------------------------------------------------------------------------
void MessageDispatcher::dispatchMsgDelay()
{ 
    std::chrono::steady_clock::time_point curTime = std::chrono::steady_clock::now();
    
    while (m_delayQueue.size() > 0)
    {
        auto iter = m_delayQueue.top();
        if (iter.m_dispatchTime < curTime )
        {
            BaseEntity* pReceiver = EntityManager::instance()->getEntityByID(iter.m_receiver);
            execute(pReceiver, iter);
            m_delayQueue.pop();
        }
        else 
        {
            break;
        }
    }
}


