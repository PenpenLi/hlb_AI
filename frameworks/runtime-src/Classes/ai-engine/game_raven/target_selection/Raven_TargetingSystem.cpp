#include "Raven_TargetingSystem.h"
#include "Raven_Bot.h"
#include "Raven_SensoryMemory.h"



//-------------------------------- ctor ---------------------------------------
//-----------------------------------------------------------------------------
Raven_TargetingSystem::Raven_TargetingSystem(Raven_Bot* owner):m_pOwner(owner),
                                                                                                                        m_pCurrentTarget(0)
{
}



//----------------------------- Update ----------------------------------------

//-----------------------------------------------------------------------------
void Raven_TargetingSystem::update()
{
    float ClosestDistSoFar = MaxDouble;
    m_pCurrentTarget = nullptr;

    //grab a list of all the opponents the owner can sense
    std::list<Raven_Bot*> SensedBots;
    SensedBots = m_pOwner->getSensoryMem()->getListOfRecentlySensedOpponents();

    std::list<Raven_Bot*>::const_iterator curBot = SensedBots.begin();
    for (curBot; curBot != SensedBots.end(); ++curBot)
    {
        //make sure the bot is alive and that it is not the owner
        if ((*curBot)->isAlive() && (*curBot != m_pOwner) )
        {
            float dist = Vec2DistanceSq((*curBot)->getPos(), m_pOwner->getPos());
            if (dist < ClosestDistSoFar)
            {
                ClosestDistSoFar = dist;
                m_pCurrentTarget = *curBot;
            }
        }
    }
}


bool Raven_TargetingSystem::isTargetWithinFOV()const
{
    return m_pOwner->getSensoryMem()->isOpponentWithinFOV(m_pCurrentTarget);
}

bool Raven_TargetingSystem::isTargetShootable()const
{
    return m_pOwner->getSensoryMem()->isOpponentShootable(m_pCurrentTarget);
}

Vector2D Raven_TargetingSystem::getLastRecordedPosition()const
{
    return m_pOwner->getSensoryMem()->getLastRecordedPositionOfOpponent(m_pCurrentTarget);
}

float Raven_TargetingSystem::getTimeTargetHasBeenVisible()const
{
    return m_pOwner->getSensoryMem()->getTimeOpponentHasBeenVisible(m_pCurrentTarget);
}

float Raven_TargetingSystem::getTimeTargetHasBeenOutOfView()const
{
    return m_pOwner->getSensoryMem()->getTimeOpponentHasBeenOutOfView(m_pCurrentTarget);
}

