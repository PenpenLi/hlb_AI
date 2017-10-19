#include "Projectile_Rocket.h"
#include "../lua/Raven_Scriptor.h"
#include "misc/cgdi.h"
#include "../Raven_Bot.h"
#include "../GameWorldRaven.h"
#include "../constants.h"
#include "2d/WallIntersectionTests.h"
#include "../Raven_Map.h"

#include "../Raven_Messages.h"
#include "Messaging/MessageDispatcher.h"


//-------------------------- ctor ---------------------------------------------
//-----------------------------------------------------------------------------
Rocket::Rocket(Raven_Bot* shooter, Vector2D target):Projectile(target,
                                                                                                    shooter->getWorld(),
                                                                                                    shooter->getID(),
                                                                                                    shooter->getPos(),
                                                                                                    shooter->getFacing(),
                                                                                                    Para_Rocket_Damage,
                                                                                                    Para_Rocket_MaxSpeed,
                                                                                                    Para_Rocket_Mass,
                                                                                                    Para_Rocket_MaxForce),

                                                                                       m_dCurrentBlastRadius(0.0),
                                                                                       m_dBlastRadius(Para_Rocket_BlastRadius)
{
    assert (target != Vector2D());
}


//------------------------------ Update ---------------------------------------
//-----------------------------------------------------------------------------
void Rocket::update()
{
    if (!m_bImpacted)
    {
        m_vVelocity = getMaxSpeed() * getHeading();

        //make sure vehicle does not exceed maximum velocity
        m_vVelocity.Truncate(m_dMaxSpeed);

        //update the position
        m_vPosition += m_vVelocity;

        testForImpact();  
    }
    else
    {
        m_dCurrentBlastRadius += Para_Rocket_ExplosionDecayRate;

        //when the rendered blast circle becomes equal in size to the blast radius
        //the rocket can be removed from the game
        if (m_dCurrentBlastRadius > m_dBlastRadius)
        {
            m_bDead = true;
        }
    }
}

void Rocket::testForImpact()
{
   
    //if the projectile has reached the target position or it hits an entity
    //or wall it should explode/inflict damage/whatever and then mark itself
    //as dead

    //test to see if the line segment connecting the rocket's current position
    //and previous position intersects with any bots.
    Raven_Bot* hit = getClosestIntersectingBot(m_vPosition - m_vVelocity, m_vPosition);

    //if hit
    if (hit)
    {
        m_bImpacted = true;

        //send a message to the bot to let it know it's been hit, and who the
        //shot came from
        MessageDispatcher::instance()->dispatchMsg( 0, 
                                                                            m_iShooterID, 
                                                                            hit->getID(),
                                                                            Msg_TakeThatMF, 
                                                                            (void*)&m_iDamageInflicted);  

        //test for bots within the blast radius and inflict damage
        inflictDamageOnBotsWithinBlastRadius();
    }

    //test for impact with a wall
    float dist;
    if( findClosestPointOfIntersectionWithWalls(  m_vPosition - m_vVelocity,
                                                                     m_vPosition,
                                                                     dist,
                                                                     m_vImpactPoint,
                                                                     m_pWorld->getMap()->getWalls()))
    {
        m_bImpacted = true;
        //test for bots within the blast radius and inflict damage
        inflictDamageOnBotsWithinBlastRadius();
        m_vPosition = m_vImpactPoint;
        return;
    }
    
    //test to see if rocket has reached target position. If so, test for
    //all bots in vicinity
    const float tolerance = 5.0;   
    if (Vec2DistanceSq(getPos(), m_vTarget) < tolerance*tolerance)
    {
        m_bImpacted = true;
        inflictDamageOnBotsWithinBlastRadius();
    }
}

//--------------- inflictDamageOnBotsWithinBlastRadius ------------------------
//
//  If the rocket has impacted we test all bots to see if they are within the 
//  blast radius and reduce their health accordingly
//-----------------------------------------------------------------------------
void Rocket::inflictDamageOnBotsWithinBlastRadius()
{
    std::list<Raven_Bot*>::const_iterator curBot = m_pWorld->getAllBots().begin();

    for (curBot; curBot != m_pWorld->getAllBots().end(); ++curBot)
    {
        if (Vec2Distance(getPos(), (*curBot)->getPos()) < m_dBlastRadius + (*curBot)->getBoundingRadius())
        {
            //send a message to the bot to let it know it's been hit, and who the
            //shot came from
            MessageDispatcher::instance()->dispatchMsg( 0, 
                                                                                m_iShooterID, 
                                                                                (*curBot)->getID(),
                                                                                Msg_TakeThatMF, 
                                                                                (void*)&m_iDamageInflicted);  
        }
    }  
}

