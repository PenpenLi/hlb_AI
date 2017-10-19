#include "Projectile_Slug.h"
#include "../lua/Raven_Scriptor.h"
#include "misc/cgdi.h"
#include "../Raven_Bot.h"
#include "../GameWorldRaven.h"
#include "game/EntityFunctionTemplates.h"
#include "2d/WallIntersectionTests.h"
#include "../Raven_Map.h"

#include "../Raven_Messages.h"
#include "Messaging/MessageDispatcher.h"

#include <list>


//-------------------------- ctor ---------------------------------------------
//-----------------------------------------------------------------------------
Slug::Slug(Raven_Bot* shooter, Vector2D target):Projectile(target,
                                                                                            shooter->getWorld(),
                                                                                            shooter->getID(),
                                                                                            shooter->getPos(),
                                                                                            shooter->getFacing(),
                                                                                            Para_Slug_Damage,
                                                                                            Para_Slug_MaxSpeed,
                                                                                            Para_Slug_Mass,
                                                                                            Para_Slug_MaxForce),
                                                                                m_dTimeShotIsVisible(Para_Slug_Persistance)
{
}

//------------------------------ Update ---------------------------------------
        

void Slug::update()
{
    if (!hasImpacted())
    {
        //calculate the steering force
        Vector2D DesiredVelocity = Vec2Normalize(m_vTarget - getPos()) * getMaxSpeed();

        Vector2D sf = DesiredVelocity - getVelocity();

        //update the position
        Vector2D accel = sf / m_dMass;

        m_vVelocity += accel;

        //make sure the slug does not exceed maximum velocity
        m_vVelocity.truncate(m_dMaxSpeed);

        //update the position
        m_vPosition += m_vVelocity; 

        testForImpact();
    }
    else if (!isVisibleToPlayer())
    {
        m_bDead = true;
    }
}

//----------------------------------- testForImpact ---------------------------
void Slug::testForImpact()
{
    // a rail gun slug travels VERY fast. It only gets the chance to update once 
    m_bImpacted = true;

    //first find the closest wall that this ray intersects with. Then we
    //can test against all entities within this range.
    float DistToClosestImpact;
    findClosestPointOfIntersectionWithWalls(m_vOrigin,
                                          m_vPosition,
                                          DistToClosestImpact,
                                          m_vImpactPoint,
                                          m_pWorld->getMap()->getWalls());

    //test to see if the ray between the current position of the slug and 
    //the start position intersects with any bots.
    std::list<Raven_Bot*> hits = getListOfIntersectingBots(m_vOrigin, m_vPosition);

    //if no bots hit just return;
    if (hits.empty()) return;

    //give some damage to the hit bots
    std::list<Raven_Bot*>::iterator it;
    for (it=hits.begin(); it != hits.end(); ++it)
    {
        //send a message to the bot to let it know it's been hit, and who the
        //shot came from
        MessageDispatcher::instance()->dispatchMsg( 0, 
                                                                        m_iShooterID, 
                                                                        (*it)->getID(),
                                                                        Msg_TakeThatMF, 
                                                                        (void*)&m_iDamageInflicted);      
    }
}

