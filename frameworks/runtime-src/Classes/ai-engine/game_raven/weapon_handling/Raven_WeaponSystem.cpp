
#include "Raven_WeaponSystem.h"
#include "ParaConfigRaven.h"
#include "game_raven/armory/Weapon_RocketLauncher.h"
#include "game_raven/armory/Weapon_RailGun.h"
#include "game_raven/armory/Weapon_ShotGun.h"
#include "game_raven/armory/Weapon_Blaster.h"
#include "Raven_Bot.h"
#include "common/misc/UtilsEx.h"


//------------------------- ctor ----------------------------------------------
//-----------------------------------------------------------------------------
Raven_WeaponSystem::Raven_WeaponSystem(Raven_Bot* owner,
                                                               float ReactionTime,
                                                               float AimAccuracy,
                                                               float AimPersistance):m_pOwner(owner),
                                                                                  m_dReactionTime(ReactionTime),
                                                                                  m_dAimAccuracy(AimAccuracy),
                                                                                  m_dAimPersistance(AimPersistance)
{
    initialize();
}

//------------------------- dtor ----------------------------------------------
//-----------------------------------------------------------------------------
Raven_WeaponSystem::~Raven_WeaponSystem()
{
    for (unsigned int w=0; w<m_WeaponMap.size(); ++w)
    {
        delete m_WeaponMap[w];
    }
}

//------------------------------ initialize -----------------------------------
//
//  initializes the weapons
//-----------------------------------------------------------------------------
void Raven_WeaponSystem::initialize()
{
    //delete any existing weapons
    WeaponMap::iterator curW;
    for (curW = m_WeaponMap.begin(); curW != m_WeaponMap.end(); ++curW)
    {
        delete curW->second;
    }

    m_WeaponMap.clear();

    //set up the container
    m_pCurrentWeapon = new Blaster(m_pOwner);

    m_WeaponMap[type_blaster] = m_pCurrentWeapon;
    m_WeaponMap[type_shotgun] = 0;
    m_WeaponMap[type_rail_gun] = 0;
    m_WeaponMap[type_rocket_launcher] = 0;
}

//-------------------------------- selectWeapon -------------------------------
//
//-----------------------------------------------------------------------------
void Raven_WeaponSystem::selectWeapon()
{ 
    //if a target is present use fuzzy logic to determine the most desirable weapon.
    if (m_pOwner->getTargetSys()->isTargetPresent())
    {
        //calculate the distance to the target
        float DistToTarget = Vec2Distance(m_pOwner->getPos(), m_pOwner->getTargetSys()->getTarget()->getPos());

        //for each weapon in the inventory calculate its desirability given the 
        //current situation. The most desirable weapon is selected
        float BestSoFar = FloatMin;

        WeaponMap::const_iterator curWeap;
        for (curWeap=m_WeaponMap.begin(); curWeap != m_WeaponMap.end(); ++curWeap)
        {
            //grab the desirability of this weapon (desirability is based upon
            //distance to target and ammo remaining)
            if (curWeap->second)
            {
                float score = curWeap->second->getDesirability(DistToTarget);

                //if it is the most desirable so far select it
                if (score > BestSoFar)
                {
                    BestSoFar = score;

                    //place the weapon in the bot's hand.
                    m_pCurrentWeapon = curWeap->second;
                }
            }
        }
    }

    else
    {
        m_pCurrentWeapon = m_WeaponMap[type_blaster];
    }
}

//--------------------  addWeapon ------------------------------------------
//
//  this is called by a weapon affector and will add a weapon of the specified
//  type to the bot's inventory.
//
//  if the bot already has a weapon of this type then only the ammo is added
//-----------------------------------------------------------------------------
void  Raven_WeaponSystem::addWeapon(unsigned int weapon_type)
{
    //create an instance of this weapon
    Weapon* w = 0;

    switch(weapon_type)
    {
        case type_rail_gun:
            w = new RailGun(m_pOwner); 
            break;

        case type_shotgun:
            w = new ShotGun(m_pOwner); 
            break;

        case type_rocket_launcher:
            w = new RocketLauncher(m_pOwner); 
            break;
    }//end switch
  

    //if the bot already holds a weapon of this type, just add its ammo
    Weapon* present = getWeaponFromInventory(weapon_type);
    if (present)
    {
        present->incrementRounds(w->getNumRoundsRemaining());
        delete w;
    }
    //if not already holding, add to inventory
    else
    {
        m_WeaponMap[weapon_type] = w;
    }
}


//------------------------- getWeaponFromInventory -------------------------------
//
//  returns a pointer to any matching weapon.
//
//  returns a null pointer if the weapon is not present
//-----------------------------------------------------------------------------
Weapon* Raven_WeaponSystem::getWeaponFromInventory(int weapon_type)
{
    return m_WeaponMap[weapon_type];
}

//----------------------- changeWeapon ----------------------------------------
void Raven_WeaponSystem::changeWeapon(unsigned int type)
{
    Weapon* w = getWeaponFromInventory(type);

    if (w) m_pCurrentWeapon = w;
}

//--------------------------- takeAimAndShoot ---------------------------------
//
//  this method aims the bots current weapon at the target (if there is a
//  target) and, if aimed correctly, fires a round
//-----------------------------------------------------------------------------
void Raven_WeaponSystem::takeAimAndShoot()const
{
    //aim the weapon only if the current target is shootable or if it has only
    //very recently gone out of view (this latter condition is to ensure the 
    //weapon is aimed at the target even if it temporarily dodges behind a wall or other cover)
    if (m_pOwner->getTargetSys()->isTargetShootable() ||
        (m_pOwner->getTargetSys()->getTimeTargetHasBeenOutOfView() <  m_dAimPersistance) )
    {
        //the position the weapon will be aimed at
        Vector2D AimingPos = m_pOwner->getTargetBot()->getPos();
    
        //if the current weapon is not an instant hit type gun the target position
        //must be adjusted to take into account the predicted movement of the 
        //target
        if (getCurrentWeapon()->getType() == type_rocket_launcher ||
             getCurrentWeapon()->getType() == type_blaster)
        {
            AimingPos = predictFuturePositionOfTarget();

            //if the weapon is aimed correctly, there is line of sight between the
            //bot and the aiming position and it has been in view for a period longer
            //than the bot's reaction time, shoot the weapon
            if ( m_pOwner->rotateFacingTowardPosition(AimingPos) &&
                (m_pOwner->getTargetSys()->getTimeTargetHasBeenVisible() >m_dReactionTime) &&
                 m_pOwner->hasLOSto(AimingPos) )
            {
                addNoiseToAim(AimingPos);

                getCurrentWeapon()->shootAt(AimingPos);
            }
        }
        //no need to predict movement, aim directly at target
        else
        {
            //if the weapon is aimed correctly and it has been in view for a period
            //longer than the bot's reaction time, shoot the weapon
            if ( m_pOwner->rotateFacingTowardPosition(AimingPos) &&
                (m_pOwner->getTargetSys()->getTimeTargetHasBeenVisible() >m_dReactionTime) )
            {
                addNoiseToAim(AimingPos);

                getCurrentWeapon()->shootAt(AimingPos);
            }
        }

    }
    //no target to shoot at so rotate facing to be parallel with the bot's
    //heading direction
    else
    {
        m_pOwner->rotateFacingTowardPosition(m_pOwner->getPos()+ m_pOwner->getHeading());
    }
}

//---------------------------- addNoiseToAim ----------------------------------
//
//  adds a random deviation to the firing angle not greater than m_dAimAccuracy 
//  rads
//-----------------------------------------------------------------------------
void Raven_WeaponSystem::addNoiseToAim(Vector2D& AimingPos)const
{
    Vector2D toPos = AimingPos - m_pOwner->getPos();

    Vec2DRotateAroundOrigin(toPos, RandFloatInRange(-m_dAimAccuracy, m_dAimAccuracy));

    AimingPos = toPos + m_pOwner->getPos();
}

//-------------------------- predictFuturePositionOfTarget --------------------
//
//  predicts where the target will be located in the time it takes for a
//  projectile to reach it. This uses a similar logic to the Pursuit steering
//  behavior.
//-----------------------------------------------------------------------------
Vector2D Raven_WeaponSystem::predictFuturePositionOfTarget()const
{
    float MaxSpeed = getCurrentWeapon()->getMaxProjectileSpeed();

    //if the target is ahead and facing the agent shoot at its current pos
    Vector2D ToEnemy = m_pOwner->getTargetBot()->getPos() - m_pOwner->getPos();

    //the lookahead time is proportional to the distance between the enemy
    //and the pursuer; and is inversely proportional to the sum of the
    //agent's velocities
    float LookAheadTime = ToEnemy.length() / (MaxSpeed + m_pOwner->getTargetBot()->getMaxSpeed());

    //return the predicted future position of the enemy
    return m_pOwner->getTargetBot()->getPos() + m_pOwner->getTargetBot()->getVelocity() * LookAheadTime;
}


//------------------ GetAmmoRemainingForWeapon --------------------------------
//
//  returns the amount of ammo remaining for the specified weapon. Return zero
//  if the weapon is not present
//-----------------------------------------------------------------------------
int Raven_WeaponSystem::getAmmoRemainingForWeapon(unsigned int weapon_type)
{
    if (m_WeaponMap[weapon_type])
    {
        return m_WeaponMap[weapon_type]->getNumRoundsRemaining();
    }

    return 0;
}

//---------------------------- shootAt ----------------------------------------
//
//  shoots the current weapon at the given position
//-----------------------------------------------------------------------------
void Raven_WeaponSystem::shootAt(Vector2D pos)const
{
    getCurrentWeapon()->shootAt(pos);
}

