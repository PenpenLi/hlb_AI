
#include "GameConfig.h"
#include "GameEntry.h"

#include "game_vehicle/GameWorldVehicle.h"
#include "game_soccer/SoccerPitch.h"
#include "game_raven/GameWorldRaven.h"

cocos2d::Node *GameEntry::create()
{
    cocos2d::Node *ret = nullptr;
    
#if defined(GAME_VEHICLE) 
    ret = (cocos2d::Node *)GameWorldVehicle::create(Win_Width, Win_Height, true);
    
#elif defined(GAME_SOCCER)
    ret = (cocos2d::Node *)SoccerPitch::create(Win_Width, Win_Height);

 #elif defined(GAME_RAVEN)
    ret = (cocos2d::Node *)GameWorldRaven::create();
#endif 
    
    if (ret && ret->init())
    {
        ret->autorelease();
    } 
    
    return ret;
}

