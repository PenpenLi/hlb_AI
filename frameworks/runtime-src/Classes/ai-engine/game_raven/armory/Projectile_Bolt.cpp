#include "Projectile_Bolt.h"
#include "../lua/Raven_Scriptor.h"
#include "misc/cgdi.h"
#include "../Raven_Bot.h"
#include "../GameWorldRaven.h"
#include "../constants.h"
#include "2d/WallIntersectionTests.h"
#include "../Raven_Map.h"

#include "../RavenMessages.h"
#include "common/message/MessageDispatcher.h"


//-------------------------- ctor ---------------------------------------------
//-----------------------------------------------------------------------------
Bolt::Bolt(Raven_Bot* shooter, Vector2D target):Projectile(target,
                                                                                             shooter->getWorld(),
                                                                                             shooter->getID(),
                                                                                             shooter->getPos(),
                                                                                             shooter->getFacing(),
                                                                                             Para_Bolt_Damage,
                                                                                             Para_Bolt_MaxSpeed,
                                                                                             Para_Bolt_Mass,
                                                                                             Para_Bolt_MaxForce)
{
    assert (target != Vector2D());
}


//------------------------------ Update ---------------------------------------
//-----------------------------------------------------------------------------
void Bolt::update()
{
    if (!m_bImpacted)
    {
        m_vVelocity = getMaxSpeed() * getHeading();

        //make sure vehicle does not exceed maximum velocity
        m_vVelocity.truncate(m_dMaxSpeed);

        //update the position
        m_vPosition += m_vVelocity;


        //if the projectile has reached the target position or it hits an entity
        //or wall it should explode/inflict damage/whatever and then mark itself
        //as dead


        //test to see if the line segment connecting the bolt's current position
        //and previous position intersects with any bots.
        Raven_Bot* hit = getClosestIntersectingBot(m_vPosition - m_vVelocity, m_vPosition);
    
        //if hit
        if (hit)
        {
            m_bImpacted = true;
            m_bDead     = true;

            //send a message to the bot to let it know it's been hit, and who the
            //shot came from
            MessageDispatcher::instance()->dispatchMsg( 0, 
                                                                                m_iShooterID, 
                                                                                hit->getID(),
                                                                                Msg_TakeThatMF, 
                                                                                (void*)&m_iDamageInflicted);
        }

        //test for impact with a wall
        float dist;
        if( findClosestPointOfIntersectionWithWalls(  m_vPosition - m_vVelocity,
                                                                         m_vPosition,
                                                                         dist,
                                                                         m_vImpactPoint,
                                                                         m_pWorld->getMap()->getWalls()))
        {
            m_bDead     = true;
            m_bImpacted = true;

            m_vPosition = m_vImpactPoint;

            return;
        }
    }
}
