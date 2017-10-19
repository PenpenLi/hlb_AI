#ifndef BOLT_H
#define BOLT_H
#pragma warning (disable:4786)
//-----------------------------------------------------------------------------
//
//  Name:   Bolt.h
//
//
//  Desc:   class to implement a bolt type projectile
//
//-----------------------------------------------------------------------------
#include "Projectile.h"

class Raven_Bot;



class Bolt : public Projectile
{
private:
    //tests the trajectory of the shell for an impact
    void testForImpact();
  
public:
    Bolt(Raven_Bot* shooter, Vector2D target);
  
    void update();
};


#endif