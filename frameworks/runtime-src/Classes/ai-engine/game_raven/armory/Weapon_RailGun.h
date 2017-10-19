#ifndef RAILGUN_H
#define RAILGUN_H
#pragma warning (disable:4786)
//-----------------------------------------------------------------------------
//
//  Name:   Weapon_RailGun.h
//
//
//  Desc:   class to implement a rail gun
//-----------------------------------------------------------------------------
#include "Weapon.h"


class  Raven_Bot;



class RailGun : public Weapon
{
public:
    RailGun(Raven_Bot* owner);

    void shootAt(Vector2D pos);

    float getDesirability(double DistToTarget);
  
private:
    void initializeFuzzyModule();  
};



#endif