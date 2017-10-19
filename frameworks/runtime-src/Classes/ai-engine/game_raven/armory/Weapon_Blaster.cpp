#include "Weapon_Blaster.h"
#include "../Raven_Bot.h"
#include "../GameWorldRaven.h"
#include "../Raven_Map.h"
#include "fuzzy/FuzzyOperators.h"


//--------------------------- ctor --------------------------------------------
//-----------------------------------------------------------------------------
Blaster::Blaster(Raven_Bot* owner):Weapon(type_blaster,
                                                                        Para_Blaster_DefaultRounds,
                                                                        Para_Blaster_MaxRoundsCarried,
                                                                        Para_Blaster_FiringFreq,
                                                                        Para_Blaster_IdealRange,
                                                                        Para_Bolt_MaxSpeed,
                                                                        owner)
{
    //setup the fuzzy module
    initializeFuzzyModule();
}


//------------------------------ shootAt --------------------------------------

inline void Blaster::shootAt(Vector2D pos)
{ 
    if (isReadyForNextShot())
    {
        //fire!
        m_pOwner->getWorld()->addBolt(m_pOwner, pos);

        updateTimeWeaponIsNextAvailable();

        //add a trigger to the game so that the other bots can hear this shot
        //(provided they are within range)
        m_pOwner->getWorld()->getMap()->addSoundTrigger(m_pOwner, Para_Blaster_SoundRange);
    }
}



//---------------------------- Desirability -----------------------------------
//
//-----------------------------------------------------------------------------
float Blaster::getDesirability(float DistToTarget)
{
    //fuzzify distance and amount of ammo
    m_FuzzyModule.fuzzify("DistToTarget", DistToTarget);

    m_dLastDesirabilityScore = m_FuzzyModule.deFuzzify("Desirability", FuzzyModule::max_av);

    return m_dLastDesirabilityScore;
}

//----------------------- initializeFuzzyModule -------------------------------
//
//  set up some fuzzy variables and rules
//-----------------------------------------------------------------------------
void Blaster::initializeFuzzyModule()
{
    FuzzyVariable& DistToTarget = m_FuzzyModule.createFLV("DistToTarget");

    FzSet& Target_Close = DistToTarget.addLeftShoulderSet("Target_Close",0,25,150);
    FzSet& Target_Medium = DistToTarget.addTriangularSet("Target_Medium",25,150,300);
    FzSet& Target_Far = DistToTarget.addRightShoulderSet("Target_Far",150,300,1000);

    FuzzyVariable& Desirability = m_FuzzyModule.createFLV("Desirability"); 
    FzSet& VeryDesirable = Desirability.addRightShoulderSet("VeryDesirable", 50, 75, 100);
    FzSet& Desirable = Desirability.addTriangularSet("Desirable", 25, 50, 75);
    FzSet& Undesirable = Desirability.addLeftShoulderSet("Undesirable", 0, 25, 50);

    m_FuzzyModule.addRule(Target_Close, Desirable);
    m_FuzzyModule.addRule(Target_Medium, FzVery(Undesirable));
    m_FuzzyModule.addRule(Target_Far, FzVery(Undesirable));
}

