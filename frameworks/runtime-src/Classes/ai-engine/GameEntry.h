
#ifndef GAMEENTRY_H
#define GAMEENTRY_H 

#include "cocos2d.h"


//Game App...
//#define GAME_VEHICLE  

//#define GAME_SOCCER

#define GAME_RAVEN 


class GameEntry:public cocos2d::Node
{
public:
    GameEntry() {};
    ~GameEntry() {};

    static cocos2d::Node *create(); 
    
};

#endif 

