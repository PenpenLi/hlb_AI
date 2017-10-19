#include "Weapon_ShotGun.h"
#include "../Raven_Bot.h"
#include "../GameWorldRaven.h"
#include "../Raven_Map.h"
#include "fuzzy/FuzzyOperators.h"


//--------------------------- ctor --------------------------------------------
//-----------------------------------------------------------------------------
ShotGun::ShotGun(Raven_Bot* owner):Weapon(type_shotgun,
                                                                            Para_ShotGun_DefaultRounds,
                                                                            Para_ShotGun_MaxRoundsCarried,
                                                                            Para_ShotGun_FiringFreq,
                                                                            Para_ShotGun_IdealRange,
                                                                            Para_Pellet_MaxSpeed,
                                                                            owner),
                                                                            m_iNumBallsInShell(Para_ShotGun_NumBallsInShell),
                                                                            m_dSpread(Para_ShotGun_Spread)
{
    //setup the fuzzy module
    initializeFuzzyModule();
}

//------------------------------ shootAt --------------------------------------

inline void ShotGun::shootAt(Vector2D pos)
{ 
    if (getNumRoundsRemaining() > 0 && isReadyForNextShot())
    {
        //a shotgun cartridge contains lots of tiny metal balls called pellets. 
        //Therefore, every time the shotgun is discharged we have to calculate
        //the spread of the pellets and add one for each trajectory
        for (int b=0; b<m_iNumBallsInShell; ++b)
        {
            //determine deviation from target using a bell curve type distribution
            float deviation = RandFloatInRange(0, m_dSpread) + RandFloatInRange(0, m_dSpread) - m_dSpread;

            Vector2D AdjustedTarget = pos - m_pOwner->getPos();

            //rotate the target vector by the deviation
            Vec2DRotateAroundOrigin(AdjustedTarget, deviation);

            //add a pellet to the game world
            m_pOwner->getWorld()->addShotGunPellet(m_pOwner, AdjustedTarget + m_pOwner->getPos());
        }

        m_iNumRoundsLeft--;

        updateTimeWeaponIsNextAvailable();

        //add a trigger to the game so that the other bots can hear this shot
        //(provided they are within range)
        m_pOwner->getWorld()->getMap()->addSoundTrigger(m_pOwner, Para_ShotGun_SoundRange);
    }
}

//---------------------------- Desirability -----------------------------------
//
//-----------------------------------------------------------------------------
inline float ShotGun::getDesirability(float DistToTarget)
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

//--------------------------- initializeFuzzyModule ---------------------------
//
//  set up some fuzzy variables and rules
//-----------------------------------------------------------------------------
void ShotGun::initializeFuzzyModule()
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
    FzSet& Ammo_Loads = AmmoStatus.addRightShoulderSet("Ammo_Loads", 30, 60, 100);
    FzSet& Ammo_Okay = AmmoStatus.addTriangularSet("Ammo_Okay", 0, 30, 60);
    FzSet& Ammo_Low = AmmoStatus.addTriangularSet("Ammo_Low", 0, 0, 30);

    m_FuzzyModule.addRule(FzAND(Target_Close, Ammo_Loads), VeryDesirable);
    m_FuzzyModule.addRule(FzAND(Target_Close, Ammo_Okay), VeryDesirable);
    m_FuzzyModule.addRule(FzAND(Target_Close, Ammo_Low), VeryDesirable);

    m_FuzzyModule.addRule(FzAND(Target_Medium, Ammo_Loads), VeryDesirable);
    m_FuzzyModule.addRule(FzAND(Target_Medium, Ammo_Okay), Desirable);
    m_FuzzyModule.addRule(FzAND(Target_Medium, Ammo_Low), Undesirable);

    m_FuzzyModule.addRule(FzAND(Target_Far, Ammo_Loads), Desirable);
    m_FuzzyModule.addRule(FzAND(Target_Far, Ammo_Okay), Undesirable);
    m_FuzzyModule.addRule(FzAND(Target_Far, Ammo_Low), Undesirable);
}
