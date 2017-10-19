#include "Weapon_RocketLauncher.h"
#include "../Raven_Bot.h"
#include "../GameWorldRaven.h"
#include "../Raven_Map.h"
#include "common/fuzzy/FuzzyOperators.h"


//--------------------------- ctor --------------------------------------------
//-----------------------------------------------------------------------------
RocketLauncher::RocketLauncher(Raven_Bot* owner):Weapon(type_rocket_launcher,
                                                                                                    Para_RocketLauncher_DefaultRounds,
                                                                                                    Para_RocketLauncher_MaxRoundsCarried,
                                                                                                    Para_RocketLauncher_FiringFreq,
                                                                                                    Para_RocketLauncher_IdealRange,
                                                                                                    Para_Rocket_MaxSpeed,
                                                                                                    owner)
{
    //setup the fuzzy module
    initializeFuzzyModule();
}


//------------------------------ ShootAt --------------------------------------
//-----------------------------------------------------------------------------
inline void RocketLauncher::shootAt(Vector2D pos)
{ 
    if (getNumRoundsRemaining() > 0 && isReadyForNextShot())
    {
        //fire off a rocket!
        m_pOwner->getWorld()->addRocket(m_pOwner, pos);

        m_iNumRoundsLeft--;

        updateTimeWeaponIsNextAvailable();

        //add a trigger to the game so that the other bots can hear this shot
        //(provided they are within range)
        m_pOwner->getWorld()->getMap()->addSoundTrigger(m_pOwner, Para_RocketLauncher_SoundRange);
    }
}

//---------------------------- Desirability -----------------------------------
//
//-----------------------------------------------------------------------------
float RocketLauncher::getDesirability(float DistToTarget)
{
    if (m_iNumRoundsLeft == 0)
    {
        m_dLastDesirabilityScore = 0;
    }
    else
    {
        //fuzzify distance and amount of ammo
        m_FuzzyModule.fuzzify("DistToTarget", DistToTarget);
        m_FuzzyModule.fuzzify("AmmoStatus", (float)m_iNumRoundsLeft);

        m_dLastDesirabilityScore = m_FuzzyModule.deFuzzify("Desirability", FuzzyModule::max_av);
    }

    return m_dLastDesirabilityScore;
}

//-------------------------  initializeFuzzyModule ----------------------------
//
//  set up some fuzzy variables and rules
//-----------------------------------------------------------------------------
void RocketLauncher::initializeFuzzyModule()
{
    FuzzyVariable& DistToTarget = m_FuzzyModule.createFLV("DistToTarget");

    FzSet& Target_Close = DistToTarget.addLeftShoulderSet("Target_Close",0,25,150);
    FzSet& Target_Medium = DistToTarget.addTriangularSet("Target_Medium",25,150,300);
    FzSet& Target_Far = DistToTarget.addRightShoulderSet("Target_Far",150,300,1000);

    FuzzyVariable& Desirability = m_FuzzyModule.createFLV("Desirability"); 
    FzSet& VeryDesirable = Desirability.addRightShoulderSet("VeryDesirable", 50, 75, 100);
    FzSet& Desirable = Desirability.addTriangularSet("Desirable", 25, 50, 75);
    FzSet& Undesirable = Desirability.addLeftShoulderSet("Undesirable", 0, 25, 50);

    FuzzyVariable& AmmoStatus = m_FuzzyModule.createFLV("AmmoStatus");
    FzSet& Ammo_Loads = AmmoStatus.addRightShoulderSet("Ammo_Loads", 10, 30, 100);
    FzSet& Ammo_Okay = AmmoStatus.addTriangularSet("Ammo_Okay", 0, 10, 30);
    FzSet& Ammo_Low = AmmoStatus.addTriangularSet("Ammo_Low", 0, 0, 10);


    m_FuzzyModule.addRule(FzAND(Target_Close, Ammo_Loads), Undesirable);
    m_FuzzyModule.addRule(FzAND(Target_Close, Ammo_Okay), Undesirable);
    m_FuzzyModule.addRule(FzAND(Target_Close, Ammo_Low), Undesirable);

    m_FuzzyModule.addRule(FzAND(Target_Medium, Ammo_Loads), VeryDesirable);
    m_FuzzyModule.addRule(FzAND(Target_Medium, Ammo_Okay), VeryDesirable);
    m_FuzzyModule.addRule(FzAND(Target_Medium, Ammo_Low), Desirable);

    m_FuzzyModule.addRule(FzAND(Target_Far, Ammo_Loads), Desirable);
    m_FuzzyModule.addRule(FzAND(Target_Far, Ammo_Okay), Undesirable);
    m_FuzzyModule.addRule(FzAND(Target_Far, Ammo_Low), Undesirable);
}

