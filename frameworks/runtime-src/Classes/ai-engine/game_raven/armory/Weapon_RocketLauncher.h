#ifndef ROCKETLAUNCHER_H
#define ROCKETLAUNCHER_H
#pragma warning (disable:4786)
//-----------------------------------------------------------------------------
//
//  Name:   RocketLauncher
//
//
//  Desc:   class to implement a rocket launcher
//-----------------------------------------------------------------------------
#include "Weapon.h"



class  Raven_Bot;

class RocketLauncher : public Weapon
{
public:
    RocketLauncher(Raven_Bot* owner);

    void shootAt(Vector2D pos);

    float getDesirability(float DistToTarget);
  
private:
    void initializeFuzzyModule();  
};



#endif