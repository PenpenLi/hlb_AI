#ifndef SLUG_H
#define SLUG_H
#pragma warning (disable:4786)
//-----------------------------------------------------------------------------
//
//  Name:   Slug.h
//
//
//  Desc:   class to implement a railgun slug
//-----------------------------------------------------------------------------

#include "Projectile.h"

class Raven_Bot;

class Slug : public Projectile
{
private:
    //when this projectile hits something it's trajectory is rendered
    //for this amount of time
    float m_dTimeShotIsVisible;

    //tests the trajectory of the shell for an impact
    void testForImpact();

    //returns true if the shot is still to be rendered
    bool isVisibleToPlayer()const
    {
        auto curTime = std::chrono::steady_clock::now();
        auto endTime = m_dTimeOfCreation + std::chrono::milliseconds((int)(m_dTimeShotIsVisible*1000));
        return curTime < endTime;
    }
  
public:
    Slug(Raven_Bot* shooter, Vector2D target);

    void update();
};


#endif