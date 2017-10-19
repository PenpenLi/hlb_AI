#include "Projectile.h"
#include "../GameWorldRaven.h"
#include <list>

//------------------ getClosestIntersectingBot --------------------------------

Raven_Bot* Projectile::getClosestIntersectingBot(Vector2D From, Vector2D To)const
{
    Raven_Bot* ClosestIntersectingBot = 0;
    float ClosestSoFar = FloatMax;

    //iterate through all entities checking against the line segment FromTo
    std::list<Raven_Bot*>::const_iterator curBot;
    for (curBot =  m_pWorld->getAllBots().begin();curBot != m_pWorld->getAllBots().end();++curBot)
    {
        //make sure we don't check against the shooter of the projectile
        if ( ((*curBot)->getID() != m_iShooterID))
        {
            //if the distance to FromTo is less than the entity's bounding radius then
            //there is an intersection
            if (distToLineSegment(From, To, (*curBot)->getPos()) < (*curBot)->getBoundingRadius())
            {
                //test to see if this is the closest so far
                float Dist = Vec2DistanceSq((*curBot)->getPos(), m_vOrigin);
                if (Dist < ClosestSoFar)
                {
                    Dist = ClosestSoFar;
                    ClosestIntersectingBot = *curBot;
                }
            }
        }
    }

    return ClosestIntersectingBot;
}


//---------------------- getListOfIntersectingBots ----------------------------
std::list<Raven_Bot*> Projectile::getListOfIntersectingBots(Vector2D From, Vector2D To)const
{
    //this will hold any bots that are intersecting with the line segment
    std::list<Raven_Bot*> hits;

    //iterate through all entities checking against the line segment FromTo
    std::list<Raven_Bot*>::const_iterator curBot;
    for (curBot =  m_pWorld->getAllBots().begin();curBot != m_pWorld->getAllBots().end();++curBot)
    {
        //make sure we don't check against the shooter of the projectile
        if ( ((*curBot)->getID() != m_iShooterID))
        {
            //if the distance to FromTo is less than the entities bounding radius then
            //there is an intersection so add it to hits
            if (distToLineSegment(From, To, (*curBot)->getPos()) < (*curBot)->getBoundingRadius())
            {
                hits.push_back(*curBot);
            }
        }
    }

    return hits;
}

