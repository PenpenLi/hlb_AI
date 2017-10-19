#ifndef BLASTER_H
#define BLASTER_H
#pragma warning (disable:4786)
//-----------------------------------------------------------------------------
//
//  Name:   Weapon_Blaster.h
//
//
//  Desc:   
//-----------------------------------------------------------------------------
#include "Weapon.h"


class  Raven_Bot;

class Blaster : public Weapon
{
public:
    Blaster(Raven_Bot* owner);

    void shootAt(Vector2D pos);

    float getDesirability(float DistToTarget);

private:
    void initializeFuzzyModule();    
};



#endif