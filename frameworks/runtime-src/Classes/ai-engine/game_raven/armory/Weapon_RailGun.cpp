#include "Weapon_RailGun.h"
#include "../Raven_Bot.h"
#include "../GameWorldRaven.h"
#include "../Raven_Map.h"
#include "common/fuzzy/FuzzyOperators.h"


//--------------------------- ctor --------------------------------------------
//-----------------------------------------------------------------------------
RailGun::RailGun(Raven_Bot* owner):Weapon(  type_rail_gun,
                                                                            Para_RailGun_DefaultRounds,
                                                                            Para_RailGun_MaxRoundsCarried,
                                                                            Para_RailGun_FiringFreq,
                                                                            Para_RailGun_IdealRange,
                                                                            Para_Slug_MaxSpeed,
                                                                            owner)
{
    //setup the fuzzy module
    initializeFuzzyModule();
}


//------------------------------ shootAt --------------------------------------

inline void RailGun::shootAt(Vector2D pos)
{ 
    if (getNumRoundsRemaining() > 0 && isReadyForNextShot())
    {
        //fire a round
        m_pOwner->getWorld()->addRailGunSlug(m_pOwner, pos);

        updateTimeWeaponIsNextAvailable();

        m_iNumRoundsLeft--;

        //add a trigger to the game so that the other bots can hear this shot
        //(provided they are within range)
        m_pOwner->getWorld()->getMap()->addSoundTrigger(m_pOwner, script->GetDouble("RailGun_SoundRange"));
    }
}

//---------------------------- Desirability -----------------------------------
//
//-----------------------------------------------------------------------------
float RailGun::getDesirability(float DistToTarget)
{
    if (m_iNumRoundsLeft == 0)
    {
        m_dLastDesirabilityScore = 0;
    }
    else
    {
        //fuzzify distance and amount of ammo
        m_FuzzyModule.fuzzify("DistanceToTarget", DistToTarget);
        m_FuzzyModule.fuzzify("AmmoStatus", (float)m_iNumRoundsLeft);

        m_dLastDesirabilityScore = m_FuzzyModule.deFuzzify("Desirability", FuzzyModule::max_av);
    }

    return m_dLastDesirabilityScore;
}

//----------------------- initializeFuzzyModule -------------------------------
//
//  set up some fuzzy variables and rules
//-----------------------------------------------------------------------------
void RailGun::initializeFuzzyModule()
{ 
    FuzzyVariable& DistanceToTarget = m_FuzzyModule.createFLV("DistanceToTarget");

    FzSet& Target_Close = DistanceToTarget.addLeftShoulderSet("Target_Close", 0, 25, 150);
    FzSet& Target_Medium = DistanceToTarget.addTriangularSet("Target_Medium", 25, 150, 300);
    FzSet& Target_Far = DistanceToTarget.addRightShoulderSet("Target_Far", 150, 300, 1000);

    FuzzyVariable& Desirability = m_FuzzyModule.createFLV("Desirability");

    FzSet& VeryDesirable = Desirability.addRightShoulderSet("VeryDesirable", 50, 75, 100);
    FzSet& Desirable = Desirability.addTriangularSet("Desirable", 25, 50, 75);
    FzSet& Undesirable = Desirability.addLeftShoulderSet("Undesirable", 0, 25, 50);

    FuzzyVariable& AmmoStatus = m_FuzzyModule.createFLV("AmmoStatus");
    FzSet& Ammo_Loads = AmmoStatus.addRightShoulderSet("Ammo_Loads", 15, 30, 100);
    FzSet& Ammo_Okay = AmmoStatus.addTriangularSet("Ammo_Okay", 0, 15, 30);
    FzSet& Ammo_Low = AmmoStatus.addTriangularSet("Ammo_Low", 0, 0, 15);


    m_FuzzyModule.addRule(FzAND(Target_Close, Ammo_Loads), FzFairly(Desirable));
    m_FuzzyModule.addRule(FzAND(Target_Close, Ammo_Okay),  FzFairly(Desirable));
    m_FuzzyModule.addRule(FzAND(Target_Close, Ammo_Low), Undesirable);

    m_FuzzyModule.addRule(FzAND(Target_Medium, Ammo_Loads), VeryDesirable);
    m_FuzzyModule.addRule(FzAND(Target_Medium, Ammo_Okay), Desirable);
    m_FuzzyModule.addRule(FzAND(Target_Medium, Ammo_Low), Desirable);

    m_FuzzyModule.addRule(FzAND(Target_Far, Ammo_Loads), FzVery(VeryDesirable));
    m_FuzzyModule.addRule(FzAND(Target_Far, Ammo_Okay), FzVery(VeryDesirable));
    m_FuzzyModule.addRule(FzAND(Target_Far, FzFairly(Ammo_Low)), VeryDesirable);
}

