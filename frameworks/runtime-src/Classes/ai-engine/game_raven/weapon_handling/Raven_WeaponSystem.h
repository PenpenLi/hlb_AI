#ifndef RAVEN_WEAPONSYSTEM
#define RAVEN_WEAPONSYSTEM
#pragma warning (disable:4786)
//-----------------------------------------------------------------------------
//
//  Name:   Raven_WeaponSystem.h
//
//
//  Desc:   class to manage all operations specific to weapons and their deployment
//
//-----------------------------------------------------------------------------
#include <map>
#include "common/2D/Vector2D.h"

class Raven_Bot;
class Weapon;



class Raven_WeaponSystem
{
public:
    Raven_WeaponSystem(Raven_Bot* owner,
                                                    float ReactionTime,
                                                    float AimAccuracy,
                                                    float AimPersistance);
  
    ~Raven_WeaponSystem();

    //sets up the weapon map with just one weapon: the blaster
    void initialize();

    //this method aims the bot's current weapon at the target (if there is a
    //target) and, if aimed correctly, fires a round. (Called each update-step
    //from Raven_Bot::Update)
    void takeAimAndShoot()const;

    //this method determines the most appropriate weapon to use given the current
    //game state. (Called every n update-steps from Raven_Bot::Update)
    void selectWeapon();
  
    //this will add a weapon of the specified type to the bot's inventory. 
    //If the bot already has a weapon of this type only the ammo is added. 
    //(called by the weapon giver-triggers to give a bot a weapon)
    void addWeapon(unsigned int weapon_type);

    //changes the current weapon to one of the specified type (provided that type
    //is in the bot's possession)
    void changeWeapon(unsigned int type);

    //shoots the current weapon at the given position
    void shootAt(Vector2D pos)const;

    //returns a pointer to the current weapon
    Weapon* getCurrentWeapon()const{return m_pCurrentWeapon;} 

    //returns a pointer to the specified weapon type (if in inventory, null if 
    //not)
    Weapon* getWeaponFromInventory(int weapon_type);

    //returns the amount of ammo remaining for the specified weapon
    int getAmmoRemainingForWeapon(unsigned int weapon_type);

    float getReactionTime()const{return m_dReactionTime;}


private:
    //a map of weapon instances indexed into by type
    typedef std::map<int, Weapon*> WeaponMap;

private:
    Raven_Bot* m_pOwner;

    //pointers to the weapons the bot is carrying (a bot may only carry one
    //instance of each weapon)
    WeaponMap m_WeaponMap;

    //a pointer to the weapon the bot is currently holding
    Weapon* m_pCurrentWeapon;

    //this is the minimum amount of time a bot needs to see an opponent before
    //it can react to it. This variable is used to prevent a bot shooting at
    //an opponent the instant it becomes visible.
    float m_dReactionTime;

    //each time the current weapon is fired a certain amount of random noise is
    //added to the the angle of the shot. This prevents the bots from hitting
    //their opponents 100% of the time. The lower this value the more accurate
    //a bot's aim will be. Recommended values are between 0 and 0.2 (the value
    //represents the max deviation in radians that can be added to each shot).
    float m_dAimAccuracy;

    //the amount of time a bot will continue aiming at the position of the target
    //even if the target disappears from view.
    float m_dAimPersistance;

    //predicts where the target will be by the time it takes the current weapon's
    //projectile type to reach it. Used by TakeAimAndShoot
    Vector2D predictFuturePositionOfTarget()const;

    //adds a random deviation to the firing angle not greater than m_dAimAccuracy rads
    void addNoiseToAim(Vector2D& AimingPos)const;
    
};

#endif