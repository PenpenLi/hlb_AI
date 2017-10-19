#include "Projectile_Pellet.h"
#include "../lua/Raven_Scriptor.h"
#include "misc/cgdi.h"
#include "../Raven_Bot.h"
#include "../GameWorldRaven.h"
#include "game/EntityFunctionTemplates.h"
#include "../constants.h"
#include "2d/WallIntersectionTests.h"
#include "../Raven_Map.h"
#include <list>

#include "../Raven_Messages.h"
#include "Messaging/MessageDispatcher.h"



//-------------------------- ctor ---------------------------------------------
//-----------------------------------------------------------------------------
Pellet::Pellet(Raven_Bot* shooter, Vector2D target): Projectile(target,
                                                                                    shooter->getWorld(),
                                                                                    shooter->getID(),
                                                                                    shooter->getPos(),
                                                                                    shooter->getFacing(),
                                                                                    Para_Pellet_Damage,
                                                                                    Para_Pellet_MaxSpeed,
                                                                                    Para_Pellet_Mass,
                                                                                    Para_Pellet_MaxForce),
                                                                                   m_dTimeShotIsVisible(Para_Pellet_Persistance)
{
}

//------------------------------ Update ---------------------------------------
        

void Pellet::update()
{
    if (!hasImpacted())
    {
        //calculate the steering force
        Vector2D DesiredVelocity = Vec2Normalize(m_vTarget - getPos()) * getMaxSpeed();

        Vector2D sf = DesiredVelocity - getVelocity();

        //update the position
        Vector2D accel = sf /m_dMass;

        m_vVelocity += accel;

        //make sure vehicle does not exceed maximum velocity
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
void Pellet::testForImpact()
{
    //a shot gun shell is an instantaneous projectile so it only gets the chance
    //to update once 
    m_bImpacted = true;

    //first find the closest wall that this ray intersects with. Then we
    //can test against all entities within this range.
    float DistToClosestImpact;
    findClosestPointOfIntersectionWithWalls(m_vOrigin,
                                                              m_vPosition,
                                                              DistToClosestImpact,
                                                              m_vImpactPoint,
                                                              m_pWorld->getMap()->getWalls());

    //test to see if the ray between the current position of the shell and 
    //the start position intersects with any bots.
    Raven_Bot* hit = getClosestIntersectingBot(m_vOrigin, m_vImpactPoint);

    //if no bots hit just return;
    if (!hit) 
        return;

    //determine the impact point with the bot's bounding circle so that the
    //shell can be rendered properly
    getLineSegmentCircleClosestIntersectionPoint(m_vOrigin,
                                                                                   m_vImpactPoint,
                                                                                   hit->getPos(),
                                                                                   hit->getBoundingRadius(),
                                                                                   m_vImpactPoint);

    //send a message to the bot to let it know it's been hit, and who the
    //shot came from
    MessageDispatcher::instance()->dispatchMsg( 0, 
                                                                        m_iShooterID, 
                                                                        hit->getID(),
                                                                        Msg_TakeThatMF, 
                                                                        (void*)&m_iDamageInflicted);  
}
