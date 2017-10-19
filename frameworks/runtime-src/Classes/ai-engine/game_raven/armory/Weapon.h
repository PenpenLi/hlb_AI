#ifndef WEAPON_BASE_H
#define WEAPON_BASE_H
#pragma warning (disable:4786)
//-----------------------------------------------------------------------------
//
//  Name:   Weapon.h
//
//
//  Desc:   Base Weapon class for the raven project
//-----------------------------------------------------------------------------
#include <vector>

#include "common/2D/Vector2D.h"
#include "../Raven_Bot.h"
#include "Fuzzy/FuzzyModule.h"
#include <chrono>


class  Raven_Bot;

class Weapon
{
protected:
    //a weapon is always (in this game) carried by a bot
    Raven_Bot* m_pOwner;

    //an enumeration indicating the type of weapon
    unsigned int m_iType;

    //fuzzy logic is used to determine the desirability of a weapon. Each weapon
    //owns its own instance of a fuzzy module because each has a different rule 
    //set for inferring desirability.
    FuzzyModule m_FuzzyModule;

    //amount of ammo carried for this weapon
    unsigned int m_iNumRoundsLeft;

    //maximum number of rounds a bot can carry for this weapon
    unsigned int m_iMaxRoundsCarried;

    //the number of times this weapon can be fired per second
    int m_dRateOfFire;

    //the earliest time the next shot can be taken
    std::chrono::steady_clock::time_point m_dTimeNextAvailable;

    //this is used to keep a local copy of the previous desirability score
    //so that we can give some feedback for debugging
    float m_dLastDesirabilityScore;

    //this is the prefered distance from the enemy when using this weapon
    float m_dIdealRange;

    //the max speed of the projectile this weapon fires
    float m_dMaxProjectileSpeed;

    //The number of times a weapon can be discharges depends on its rate of fire.
    //This method returns true if the weapon is able to be discharged at the 
    //current time. (called from ShootAt() )
    bool isReadyForNextShot();

    //this is called when a shot is fired to update m_dTimeNextAvailable
    void updateTimeWeaponIsNextAvailable();

    //this method initializes the fuzzy module with the appropriate fuzzy 
    //variables and rule base.
    virtual void initializeFuzzyModule() = 0;

public:
    Weapon(unsigned int TypeOfGun,
                        unsigned int DefaultNumRounds,
                        unsigned int MaxRoundsCarried,
                        float RateOfFire,
                        float IdealRange,
                        float ProjectileSpeed,
                        Raven_Bot* OwnerOfGun):m_iType(TypeOfGun),
                                                                 m_iNumRoundsLeft(DefaultNumRounds),
                                                                 m_pOwner(OwnerOfGun),
                                                                 m_dRateOfFire(RateOfFire),
                                                                 m_iMaxRoundsCarried(MaxRoundsCarried),
                                                                 m_dLastDesirabilityScore(0),
                                                                 m_dIdealRange(IdealRange),
                                                                 m_dMaxProjectileSpeed(ProjectileSpeed)
    {  
        m_dTimeNextAvailable = std::chrono::steady_clock::now();
    }

    virtual ~Weapon(){}

    //this method aims the weapon at the given target by rotating the weapon's
    //owner's facing direction (constrained by the bot's turning rate). It returns  
    //true if the weapon is directly facing the target.
    bool aimAt(Vector2D target)const;

    //this discharges a projectile from the weapon at the given target position
    //(provided the weapon is ready to be discharged... every weapon has its
    //own rate of fire)
    virtual void shootAt(Vector2D pos) = 0;

    //this method returns a value representing the desirability of using the
    //weapon. This is used by the AI to select the most suitable weapon for
    //a bot's current situation. This value is calculated using fuzzy logic
    virtual float getDesirability(float DistToTarget)=0;

    //returns the desirability score calculated in the last call to GetDesirability
    //(just used for debugging)
    float getLastDesirabilityScore()const{return m_dLastDesirabilityScore;}

    //returns the maximum speed of the projectile this weapon fires
    float getMaxProjectileSpeed()const{return m_dMaxProjectileSpeed;}

    //returns the number of rounds remaining for the weapon
    int getNumRoundsRemaining()const{return m_iNumRoundsLeft;}
    void decrementNumRounds(){if (m_iNumRoundsLeft>0) --m_iNumRoundsLeft;}
    void incrementRounds(int num); 
    unsigned int getType()const{return m_iType;}
    float getIdealRange()const{return m_dIdealRange;}
};


///////////////////////////////////////////////////////////////////////////////
//------------------------ ReadyForNextShot -----------------------------------
//
//  returns true if the weapon is ready to be discharged
//-----------------------------------------------------------------------------
inline bool Weapon::isReadyForNextShot()
{
    if (std::chrono::steady_clock::now() > m_dTimeNextAvailable)
    {
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
inline void Weapon::updateTimeWeaponIsNextAvailable()
{
    m_dTimeNextAvailable = std::chrono::steady_clock::now() + std::chrono::milliseconds((int)(1000/m_dRateOfFire));
}


//-----------------------------------------------------------------------------
inline bool Weapon::aimAt(Vector2D target)const
{
    return m_pOwner->rotateFacingTowardPosition(target);
}

//-----------------------------------------------------------------------------
inline void Weapon::incrementRounds(int num)
{
    m_iNumRoundsLeft += num;
    clamp(m_iNumRoundsLeft, 0, m_iMaxRoundsCarried);
} 

#endif