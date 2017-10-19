#ifndef ROCKET_H
#define ROCKET_H
#pragma warning (disable:4786)
//-----------------------------------------------------------------------------
//
//  Name:   Rocket.h
//
//
//  Desc:   class to implement a rocket
//
//-----------------------------------------------------------------------------

#include "Projectile.h"

class Raven_Bot;

class Rocket : public Projectile
{
private:
    //the radius of damage, once the rocket has impacted
    float m_dBlastRadius;

    //this is used to render the splash when the rocket impacts
    float m_dCurrentBlastRadius;

    //If the rocket has impacted we test all bots to see if they are within the 
    //blast radius and reduce their health accordingly
    void inflictDamageOnBotsWithinBlastRadius();

    //tests the trajectory of the shell for an impact
    void testForImpact();

public:
    Rocket(Raven_Bot* shooter, Vector2D target);

    void update();

};


#endif