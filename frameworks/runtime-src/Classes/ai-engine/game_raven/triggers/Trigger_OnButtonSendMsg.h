#ifndef TRIGGER_SEND_MESSAGE_H
#define TRIGGER_SEND_MESSAGE_H
///-----------------------------------------------------------------------------
//
//  Name:   Trigger_OnButtonSendMsg.h
//
//
//  Desc:   trigger class to define a button that sends a msg to a 
//          specific entity when activated.
//-----------------------------------------------------------------------------
#include "common/triggers/Trigger.h"
#include "common/message/MessageDispatcher.h"

template <class entity_type>
class Trigger_OnButtonSendMsg : public Trigger<entity_type>
{
public:
    Trigger_OnButtonSendMsg(Vector2D pos, int msgId, int entityId, float r):m_iReceiver(entityId),
                                                                                                                                m_msgId(msgId)
    {
        setPos(pos);
        setBoundingRadius(r);
        
        //create and set this trigger's region of fluence
        addRectangularTriggerRegion(getPos()-Vector2D(r, r),   //top left corner
                                                    getPos()+Vector2D(r, r));  //bottom right corner
    }

    ~Trigger_OnButtonSendMsg()
    {

    }
    
    void tryCheck(entity_type* pEnt);

    void update();

    void render();

    bool handleMessage(const Telegram& msg);

private:
    //when triggered a message is sent to the entity with the following ID
    int m_iReceiver;

    //the message that is sent
    int m_msgId;

    float m_radius;    
};


///////////////////////////////////////////////////////////////////////////////



template <class entity_type>
void Trigger_OnButtonSendMsg<entity_type>::tryCheck(entity_type* pEnt)
{
    if (isTouchingTrigger(pEnt->getPos(), pEnt->getBoundingRadius()))
    {
        MessageDispatcher::instance()->dispatchMsg( 0, 
                                                                            this->getID(),
                                                                            m_iReceiver,
                                                                            m_msgId, 
                                                                            NULL);
    }
}

template <class entity_type>
void Trigger_OnButtonSendMsg<entity_type>::update()
{
}

template <class entity_type>
void Trigger_OnButtonSendMsg<entity_type>::render()
{
}

template <class entity_type>
bool Trigger_OnButtonSendMsg<entity_type>::handleMessage(const Telegram& msg)
{
    return false;
}

#endif
