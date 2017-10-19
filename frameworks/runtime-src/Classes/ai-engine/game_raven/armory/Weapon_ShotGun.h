#ifndef SHOTGUN_H
#define SHOTGUN_H
#pragma warning (disable:4786)
//-----------------------------------------------------------------------------
//
//  Name:   Weapon_ShotGun.h
//
//  Desc:   class to implement a shot gun
//-----------------------------------------------------------------------------
#include "Weapon.h"


class  Raven_Bot;



class ShotGun : public Weapon
{
public:
    ShotGun(Raven_Bot* owner);

    void shootAt(Vector2D pos);

    float getDesirability(float DistToTarget);

private:
    void initializeFuzzyModule();

    //how much shot the each shell contains
    int m_iNumBallsInShell;

    //how much the shot spreads out when a cartridge is discharged
    float m_dSpread;  
};



#endif