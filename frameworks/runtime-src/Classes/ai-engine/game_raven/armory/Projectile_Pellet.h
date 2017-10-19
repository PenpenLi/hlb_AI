#ifndef PELLET_H
#define PELLET_H
#pragma warning (disable:4786)
//-----------------------------------------------------------------------------
//
//  Name:   Pellet.h
//
//
//  Desc:   class to implement a pellet type projectile
//
//-----------------------------------------------------------------------------

#include "Projectile.h"
#include <chrono>

class Raven_Bot;

class Pellet : public Projectile
{
private:
    //when this projectile hits something it's trajectory is rendered
    //for this amount of time
    float m_dTimeShotIsVisible;

    //tests the trajectory of the pellet for an impact
    void testForImpact();

    //returns true if the shot is still to be rendered
    bool isVisibleToPlayer()const
    {
        auto curTime = std::chrono::steady_clock::now();
        auto endTime = m_dTimeOfCreation + std::chrono::milliseconds((int)(m_dTimeShotIsVisible*1000));
        return curTime < endTime;
    }
  
public:
    Pellet(Raven_Bot* shooter, Vector2D target);

    void update();
};

#endif
