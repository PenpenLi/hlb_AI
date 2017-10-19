#ifndef RAVEN_ENV
#define RAVEN_ENV
#pragma warning (disable:4786)
//-----------------------------------------------------------------------------
//
//  Name:   GameWorldRaven.h
//
//
//  Desc:   this class creates and stores all the entities that make up the
//          Raven game environment. (walls, bots, health etc) and can read a
//          Raven map file and recreate the necessary geometry.
//
//          this class has methods for updating the game entities and for
//          rendering them.
//-----------------------------------------------------------------------------
#include <vector>
#include <string>
#include <list>

#include "common/game/BaseNode.h"
#include "common/game/Wall.h"
#include "common/game/CommonFunction.h"
#include "common/navigation/PathManager.h"
#include "navigation/Raven_PathPlanner.h"
#include "misc/Raven_Bot.h"


class BaseEntity;
class Projectile;
class Raven_Map;
class GraveMarkers;



class GameWorldRaven :public BaseNode
{
public:
    GameWorldRaven();
    
    ~GameWorldRaven();

    void onEnter();
    
    void update();

    //loads an environment from a file
    bool loadMap(const std::string& FileName); 

    void addBots(unsigned int NumBotsToAdd);
    void addRocket(Raven_Bot* shooter, Vector2D target);
    void addRailGunSlug(Raven_Bot* shooter, Vector2D target);
    void addShotGunPellet(Raven_Bot* shooter, Vector2D target);
    void addBolt(Raven_Bot* shooter, Vector2D target);

    //removes the last bot to be added
    void removeBot();

    //returns true if a bot of size BoundingRadius cannot move from A to B
    //without bumping into world geometry
    bool isPathObstructed(Vector2D A, Vector2D B, float BoundingRadius = 0)const;

    //returns a vector of pointers to bots in the FOV of the given bot
    std::vector<Raven_Bot*> getAllBotsInFOV(const Raven_Bot* pBot)const;

    //returns true if the second bot is unobstructed by walls and in the field of view of the first.
    bool isSecondVisibleToFirst(const Raven_Bot* pFirst, const Raven_Bot* pSecond)const;

    //returns true if the ray between A and B is unobstructed.
    bool isLOSOkay(Vector2D A, Vector2D B)const;

    //starting from the given origin and moving in the direction Heading this
    //method returns the distance to the closest wall
    float getDistanceToClosestWall(Vector2D Origin, Vector2D Heading)const;

  
    //returns the position of the closest visible switch that triggers the
    //door of the specified ID
    Vector2D getPosOfClosestSwitch(Vector2D botPos, unsigned int doorID)const;

    //given a position on the map this method returns the bot found with its
    //bounding radius of that position.If there is no bot at the position the
    //method returns NULL
    Raven_Bot* getBotAtPosition(Vector2D CursorPos)const;

    void togglePause(){m_bPaused = !m_bPaused;}
  
    //this method is called when the user clicks the right mouse button.
    //The method checks to see if a bot is beneath the cursor. If so, the bot
    //is recorded as selected.If the cursor is not over a bot then any selected
    // bot/s will attempt to move to that position.
    //void clickRightMouseButton(POINTS p);

    //this method is called when the user clicks the left mouse button. If there
    //is a possessed bot, this fires the weapon, else does nothing
    //void clickLeftMouseButton(POINTS p);

    //when called will release any possessed bot from user control
    //void exorciseAnyPossessedBot();
 
    //if a bot is possessed the keyboard is polled for user input and any 
    //relevant bot methods are called appropriately
    //void getPlayerInput()const;
    
    Raven_Bot* getPossessedBot()const{return m_pSelectedBot;}
    void changeWeaponOfPossessedBot(unsigned int weapon)const;

    Raven_Map* const getMap(){return m_pMap;}
    
    const std::list<Raven_Bot*>& getAllBots()const{return m_Bots;}
    
    PathManager<Raven_PathPlanner>* const getPathManager(){return m_pPathManager;}
    
    int getNumBots()const{return m_Bots.size();}

    void  tagRaven_BotsWithinViewRange(BaseEntity* pRaven_Bot, float range)
                                                                    {tagNeighbors(pRaven_Bot, m_Bots, range);}  


private:
    //the current game map
    Raven_Map* m_pMap;

    //a list of all the bots that are inhabiting the map
    std::list<Raven_Bot*> m_Bots;

    //the user may select a bot to control manually. This is a pointer to that bot
    Raven_Bot* m_pSelectedBot;
  
    //this list contains any active projectiles (slugs, rockets,
    //shotgun pellets, etc)
    std::list<Projectile*> m_Projectiles;

    //this class manages all the path planning requests
    PathManager<Raven_PathPlanner>* m_pPathManager;

    //if true the game will be paused
    bool m_bPaused;

    //if true a bot is removed from the game
    bool m_bRemoveABot;

    //when a bot is killed a "grave" is displayed for a few seconds. This
    //class manages the graves
    GraveMarkers* m_pGraveMarkers;

    pNode m_ui;
    
    //this iterates through each trigger, testing each one against each bot
    void updateTriggers();

    //deletes all entities, empties all containers and creates a new navgraph 
    void clear();

    //attempts to position a spawning bot at a free spawn point. returns false if unsuccessful 
    bool attemptToAddBot(Raven_Bot* pBot);

    //when a bot is removed from the game by a user all remaining bots
    //must be notified so that they can remove any references to that bot from
    //their memory
    void notifyAllBotsOfRemoval(Raven_Bot* pRemovedBot)const;

    void render();
};


#endif