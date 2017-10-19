#ifndef RAVEN_WEAPON_EVALUATOR
#define RAVEN_WEAPON_EVALUATOR
#pragma warning (disable:4786)
//-----------------------------------------------------------------------------
//
//  Name:   Evaluator_GetWeapon.h
//
//
//  Desc:  class to calculate how desirable the goal of fetching a weapon item
//         is 
//-----------------------------------------------------------------------------

#include "Evaluator.h"
#include "../Raven_Bot.h"


class Evaluator_GetWeapon : public Evaluator
{ 
public:
    Evaluator_GetWeapon(float bias,int WeaponType):Evaluator(bias),m_iWeaponType(WeaponType)
    {
    }
  
    float calculateDesirability(Raven_Bot* pBot);

    void  setGoal(Raven_Bot* pEnt);
    
private:
      int m_iWeaponType;
};

#endif