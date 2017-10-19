
#include "GameWorldRaven.h"
#include "navigation/Raven_PathPlanner.h"
#include "common/game/EntityManager.h"
#include "sensor_memory/Raven_SensoryMemory.h"
#include "weapon_handling/Raven_WeaponSystem.h"
#include "common/message/MessageDispatcher.h"
#include "misc/Raven_SteeringBehaviors.h"
#include "misc/RavenMessages.h"
#include "misc/GraveMarkers.h"
#include "misc/Raven_Map.h"
#include "misc/Raven_Door.h"
#include "armory/Projectile.h"
#include "armory/Projectile_Rocket.h"
#include "armory/Projectile_Pellet.h"
#include "armory/Projectile_Slug.h"
#include "armory/Projectile_Bolt.h"
#include "goals/Goal_Think.h"
#include "goals/Raven_Goal_Types.h"
#include "common/2D/Geometry.h"
#include "common/2D/WallIntersectionTests.h"
#include "common/misc/LogDebug.h"
#include "misc/ParaConfigRaven.h"


//----------------------------- ctor ------------------------------------------
//-----------------------------------------------------------------------------
GameWorldRaven::GameWorldRaven():m_pSelectedBot(NULL),
                                                    m_bPaused(false),
                                                    m_bRemoveABot(false),
                                                    m_pMap(NULL),
                                                    m_pPathManager(NULL),
                                                    m_pGraveMarkers(NULL)
{
    //load in the default map
    //loadMap(Para_StartMap));
}


//------------------------------ dtor -----------------------------------------
//-----------------------------------------------------------------------------
GameWorldRaven::~GameWorldRaven()
{
    clear();
    delete m_pPathManager;
    delete m_pMap;
    delete m_pGraveMarkers;
}

void GameWorldRaven::onEnter()
{
    AILOG("GameWorldVehicle::onEnter");  

    BaseNode::onEnter();
    enableScheduleUpdate(this);
}

//---------------------------- Clear ------------------------------------------
//
//  deletes all the current objects ready for a map load
//-----------------------------------------------------------------------------
void GameWorldRaven::clear()
{
    AILOG("------------------------------ Clearup -------------------------------");

    //delete the bots
    std::list<Raven_Bot*>::iterator it = m_Bots.begin();
    for (it; it != m_Bots.end(); ++it)
    {
        delete *it;
    }

    //delete any active projectiles
    std::list<Projectile*>::iterator curW = m_Projectiles.begin();
    for (curW; curW != m_Projectiles.end(); ++curW)
    {
        delete *curW;
    }

    //clear the containers
    m_Projectiles.clear();
    m_Bots.clear();

    m_pSelectedBot = NULL;
}

//-------------------------------- update -------------------------------------
//
//  calls the update function of each entity
//-----------------------------------------------------------------------------
void GameWorldRaven::update()
{ 
    //don't update if the user has paused the game
    if (m_bPaused) return;

    m_pGraveMarkers->update();

    //get any player keyboard input
    //getPlayerInput();
  
    //update all the queued searches in the path manager
    m_pPathManager->updateSearches();

    //update any doors
    std::vector<Raven_Door*>::iterator curDoor =m_pMap->getDoors().begin();
    for (curDoor; curDoor != m_pMap->getDoors().end(); ++curDoor)
    {
        (*curDoor)->update();
    }

    //update any current projectiles
    std::list<Projectile*>::iterator curW = m_Projectiles.begin();
    while (curW != m_Projectiles.end())
    {
        //test for any dead projectiles and remove them if necessary
        if (!(*curW)->isDead())
        {
            (*curW)->update();
            ++curW;
        }
        else
        { 
            delete *curW;
            curW = m_Projectiles.erase(curW);
        } 
  }
  
    //update the bots
    bool bSpawnPossible = true;
  
    std::list<Raven_Bot*>::iterator curBot = m_Bots.begin();
    for (curBot; curBot != m_Bots.end(); ++curBot)
    {
        //if this bot's status is 'respawning' attempt to resurrect it from
        //an unoccupied spawn point
        if ((*curBot)->isSpawning() && bSpawnPossible)
        {
            bSpawnPossible = attemptToAddBot(*curBot);
        }

        //if this bot's status is 'dead' add a grave at its current location 
        //then change its status to 'respawning'
        else if ((*curBot)->isDead())
        {
            //create a grave
            m_pGraveMarkers->addGrave((*curBot)->getPos());

            //change its status to spawning
            (*curBot)->setSpawning();
        }

        //if this bot is alive update it.
        else if ( (*curBot)->isAlive())
        {
            (*curBot)->update();
        }  
    } 

    //update the triggers
    m_pMap->updateTriggerSystem(m_Bots);

    //if the user has requested that the number of bots be decreased, remove one
    if (m_bRemoveABot)
    { 
        if (!m_Bots.empty())
        {
            Raven_Bot* pBot = m_Bots.back();
            if (pBot == m_pSelectedBot) 
            {
                m_pSelectedBot = nullptr;
            }
            notifyAllBotsOfRemoval(pBot);
            
            delete m_Bots.back();
            
            m_Bots.remove(pBot);
            pBot = nullptr;
        }

        m_bRemoveABot = false;
    }

    render();
}


//----------------------------- attemptToAddBot -------------------------------
//-----------------------------------------------------------------------------
bool GameWorldRaven::attemptToAddBot(Raven_Bot* pBot)
{
    //make sure there are some spawn points available
    if (m_pMap->getSpawnPoints().size() <= 0)
    {
        AILOG("Map has no spawn points!");
        return false;
    }

    //we'll make the same number of attempts to spawn a bot this update as
    //there are spawn points
    int attempts = m_pMap->getSpawnPoints().size();
    while (--attempts >= 0)
    { 
        //select a random spawn point
        Vector2D pos = m_pMap->getRandomSpawnPoint();

        //check to see if it's occupied
        std::list<Raven_Bot*>::const_iterator curBot = m_Bots.begin();

        bool bAvailable = true;

        for (curBot; curBot != m_Bots.end(); ++curBot)
        {
            //if the spawn point is unoccupied spawn a bot
            if (Vec2Distance(pos, (*curBot)->getPos()) < (*curBot)->getBoundingRadius())
            {
                bAvailable = false;
            }
        }

        if (bAvailable)
        {  
            pBot->spawn(pos);
            return true;   
        }
    }

    return false;
}

//-------------------------- addBots --------------------------------------
//
//  Adds a bot and switches on the default steering behavior
//-----------------------------------------------------------------------------
void GameWorldRaven::addBots(unsigned int NumBotsToAdd)
{ 
    while (NumBotsToAdd--)
    {
        //create a bot. (its position is irrelevant at this point because it will
        //not be rendered until it is spawned)
        Raven_Bot* rb = new Raven_Bot(this, Vector2D());
    
        //switch the default steering behaviors on
        rb->getSteering()->wallAvoidanceOn();
        rb->getSteering()->separationOn();
        m_Bots.push_back(rb);

        //register the bot with the entity manager
        EntityManager::instance()->addEntity(rb);
    }
}

//---------------------------- notifyAllBotsOfRemoval -------------------------
//
//  when a bot is removed from the game by a user all remianing bots
//  must be notifies so that they can remove any references to that bot from
//  their memory
//-----------------------------------------------------------------------------
void GameWorldRaven::notifyAllBotsOfRemoval(Raven_Bot* pRemovedBot)const
{
    std::list<Raven_Bot*>::const_iterator curBot = m_Bots.begin();
    for (curBot; curBot != m_Bots.end(); ++curBot)
    {
        MessageDispatcher::instance()->dispatchMsg( 0, 
                                                                            -1, 
                                                                            (*curBot)->getID(),
                                                                            Msg_UserHasRemovedBot, 
                                                                            pRemovedBot);
    }
}
//-------------------------------removeBot ------------------------------------
//
//  removes the last bot to be added from the game
//-----------------------------------------------------------------------------
void GameWorldRaven::removeBot()
{
    m_bRemoveABot = true;
}

//--------------------------- addBolt -----------------------------------------
//-----------------------------------------------------------------------------
void GameWorldRaven::addBolt(Raven_Bot* shooter, Vector2D target)
{
    Projectile* rp = new Bolt(shooter, target);

    m_Projectiles.push_back(rp);
}

//------------------------------ addRocket --------------------------------
void GameWorldRaven::addRocket(Raven_Bot* shooter, Vector2D target)
{
    Projectile* rp = new Rocket(shooter, target);

    m_Projectiles.push_back(rp);
}

//------------------------- addRailGunSlug -----------------------------------
void GameWorldRaven::addRailGunSlug(Raven_Bot* shooter, Vector2D target)
{
    Projectile* rp = new Slug(shooter, target);

    m_Projectiles.push_back(rp);
}

//------------------------- addShotGunPellet -----------------------------------
void GameWorldRaven::addShotGunPellet(Raven_Bot* shooter, Vector2D target)
{
    Projectile* rp = new Pellet(shooter, target);

    m_Projectiles.push_back(rp);
}


//----------------------------- getBotAtPosition ------------------------------
//
//  given a position on the map this method returns the bot found with its
//  bounding radius of that position.
//  If there is no bot at the position the method returns NULL
//-----------------------------------------------------------------------------
Raven_Bot* GameWorldRaven::getBotAtPosition(Vector2D CursorPos)const
{
    std::list<Raven_Bot*>::const_iterator curBot = m_Bots.begin();

    for (curBot; curBot != m_Bots.end(); ++curBot)
    {
        if (Vec2Distance((*curBot)->getPos(), CursorPos) < (*curBot)->getBoundingRadius())
        {
            if ((*curBot)->isAlive())
            {
                return *curBot;
            }
        }
    }

    return NULL;
}

//-------------------------------- loadMap ------------------------------------
//
//  sets up the game environment from map file
//-----------------------------------------------------------------------------
bool GameWorldRaven::loadMap(const std::string& filename)
{  
    //clear any current bots and projectiles
    clear();

    //out with the old
    delete m_pMap;
    delete m_pGraveMarkers;
    delete m_pPathManager;

    //in with the new
    m_pGraveMarkers = new GraveMarkers(Para_GraveLifetime);
    m_pPathManager = new PathManager<Raven_PathPlanner>(Para_MaxSearchCyclesPerUpdateStep);
    m_pMap = new Raven_Map();

    //make sure the entity manager is reset
    EntityManager::instance()->reset();

    //load the new map data
    if (m_pMap->loadMap(filename))
    { 
        addBots(Para_NumBots);
        return true;
    }
    
    return false;

}

#if 0
//------------------------- exorciseAnyPossessedBot ---------------------------
//
//  when called will release any possessed bot from user control
//-----------------------------------------------------------------------------
void GameWorldRaven::exorciseAnyPossessedBot()
{
    if (m_pSelectedBot) 
        m_pSelectedBot->exorcise(); 
}


//-------------------------- clickRightMouseButton -----------------------------
//
//  this method is called when the user clicks the right mouse button.
//
//  the method checks to see if a bot is beneath the cursor. If so, the bot
//  is recorded as selected.
//
//  if the cursor is not over a bot then any selected bot/s will attempt to
//  move to that position.
//-----------------------------------------------------------------------------
void GameWorldRaven::clickRightMouseButton(POINTS p)
{
  Raven_Bot* pBot = getBotAtPosition(POINTStoVector(p));

  //if there is no selected bot just return;
  if (!pBot && m_pSelectedBot == NULL) return;

  //if the cursor is over a different bot to the existing selection,
  //change selection
  if (pBot && pBot != m_pSelectedBot)
  { 
    if (m_pSelectedBot) m_pSelectedBot->exorcise();
    m_pSelectedBot = pBot;

    return;
  }

  //if the user clicks on a selected bot twice it becomes possessed(under
  //the player's control)
  if (pBot && pBot == m_pSelectedBot)
  {
    m_pSelectedBot->TakePossession();

    //clear any current goals
    m_pSelectedBot->GetBrain()->RemoveAllSubgoals();
  }

  //if the bot is possessed then a right click moves the bot to the cursor
  //position
  if (m_pSelectedBot->isPossessed())
  {
    //if the shift key is pressed down at the same time as clicking then the
    //movement command will be queued
    if (IS_KEY_PRESSED('Q'))
    {
      m_pSelectedBot->GetBrain()->QueueGoal_MoveToPosition(POINTStoVector(p));
    }
    else
    {
      //clear any current goals
      m_pSelectedBot->GetBrain()->RemoveAllSubgoals();

      m_pSelectedBot->GetBrain()->AddGoal_MoveToPosition(POINTStoVector(p));
    }
  }
}

//---------------------- clickLeftMouseButton ---------------------------------
//-----------------------------------------------------------------------------
void GameWorldRaven::clickLeftMouseButton(POINTS p)
{
  if (m_pSelectedBot && m_pSelectedBot->isPossessed())
  {
    m_pSelectedBot->FireWeapon(POINTStoVector(p));
  }
}

//------------------------ getPlayerInput -------------------------------------
//
//  if a bot is possessed the keyboard is polled for user input and any 
//  relevant bot methods are called appropriately
//-----------------------------------------------------------------------------
void GameWorldRaven::getPlayerInput()const
{
  if (m_pSelectedBot && m_pSelectedBot->isPossessed())
  {
      m_pSelectedBot->RotateFacingTowardPosition(getClientCursorPosition());
   }
}
#endif

//-------------------- changeWeaponOfPossessedBot -----------------------------
//
//  changes the weapon of the possessed bot
//-----------------------------------------------------------------------------
void GameWorldRaven::changeWeaponOfPossessedBot(unsigned int weapon)const
{
    //ensure one of the bots has been possessed
    if (m_pSelectedBot)
    {
        switch(weapon)
        {
            case type_blaster:
                getPossessedBot()->changeWeapon(type_blaster); 
                return;

            case type_shotgun:
                getPossessedBot()->changeWeapon(type_shotgun); 
                return;

            case type_rocket_launcher:
                getPossessedBot()->changeWeapon(type_rocket_launcher); 
                return;

            case type_rail_gun:
                getPossessedBot()->changeWeapon(type_rail_gun); 
                return;
        }
    }
}

//---------------------------- isLOSOkay --------------------------------------
//
//  returns true if the ray between A and B is unobstructed.
//------------------------------------------------------------------------------
bool GameWorldRaven::isLOSOkay(Vector2D A, Vector2D B)const
{
    return !doWallsObstructLineSegment(A, B, m_pMap->getWalls());
}

//------------------------- isPathObstructed ----------------------------------
//
//  returns true if a bot cannot move from A to B without bumping into 
//  world geometry. It achieves this by stepping from A to B in steps of
//  size BoundingRadius and testing for intersection with world geometry at
//  each point.
//-----------------------------------------------------------------------------
bool GameWorldRaven::isPathObstructed(Vector2D A,Vector2D B,float BoundingRadius)const
{
    Vector2D ToB = Vec2Normalize(B-A);
    Vector2D curPos = A;

    while (Vec2DistanceSq(curPos, B) > BoundingRadius*BoundingRadius)
    {   
        //advance curPos one step
        curPos += ToB * 0.5 * BoundingRadius;

        //test all walls against the new position
        if (doWallsIntersectCircle(m_pMap->getWalls(), curPos, BoundingRadius))
        {
            return true;
        }
    }

    return false;
}


//----------------------------- getAllBotsInFOV ------------------------------
//
//  returns a vector of pointers to bots within the given bot's field of view
//-----------------------------------------------------------------------------
std::vector<Raven_Bot*>
GameWorldRaven::getAllBotsInFOV(const Raven_Bot* pBot)const
{
    std::vector<Raven_Bot*> VisibleBots;

    std::list<Raven_Bot*>::const_iterator curBot = m_Bots.begin();
    for (curBot; curBot != m_Bots.end(); ++curBot)
    {
        //make sure time is not wasted checking against the same bot or against a
        // bot that is dead or re-spawning
        if (*curBot == pBot ||  !(*curBot)->isAlive()) continue;

        //first of all test to see if this bot is within the FOV
        if (isSecondInFOVOfFirst(pBot->getPos(),pBot->getFacing(),(*curBot)->getPos(),pBot->getFieldOfView()))
        {
            //cast a ray from between the bots to test visibility. If the bot is
            //visible add it to the vector
            if (!doWallsObstructLineSegment(pBot->getPos(),(*curBot)->getPos(),m_pMap->getWalls()))
            {
                VisibleBots.push_back(*curBot);
            }
        }
    }

    return VisibleBots;
}

//---------------------------- isSecondVisibleToFirst -------------------------

bool GameWorldRaven::isSecondVisibleToFirst(const Raven_Bot* pFirst, const Raven_Bot* pSecond)const
{
    //if the two bots are equal or if one of them is not alive return false
    if ( !(pFirst == pSecond) && pSecond->isAlive())
    {
        //first of all test to see if this bot is within the FOV
        if (isSecondInFOVOfFirst(pFirst->getPos(),pFirst->getFacing(),pSecond->getPos(),pFirst->getFieldOfView()))
        {
            //test the line segment connecting the bot's positions against the walls.
            //If the bot is visible add it to the vector
            if (!doWallsObstructLineSegment(pFirst->getPos(),pSecond->getPos(),m_pMap->getWalls()))
            {
                return true;
            }
        }
    }

    return false;
}

//--------------------- getPosOfClosestSwitch -----------------------------
//
//  returns the position of the closest visible switch that triggers the
//  door of the specified ID
//-----------------------------------------------------------------------------
Vector2D GameWorldRaven::getPosOfClosestSwitch(Vector2D botPos, unsigned int doorID)const
{
    std::vector<unsigned int> SwitchIDs;

    //first we need to get the ids of the switches attached to this door
    std::vector<Raven_Door*>::const_iterator curDoor;
    for (curDoor = m_pMap->getDoors().begin();curDoor != m_pMap->getDoors().end();++curDoor)
    {
        if ((*curDoor)->getID() == doorID)
        {
            SwitchIDs = (*curDoor)->getSwitchIDs(); 
            break;
        }
    }

    Vector2D closest;
    float ClosestDist = FloatMax;

    //now test to see which one is closest and visible
    std::vector<unsigned int>::iterator it;
    for (it = SwitchIDs.begin(); it != SwitchIDs.end(); ++it)
    {
        BaseEntity* trig = EntityManager::instance()->getEntityByID(*it);

        if (isLOSOkay(botPos, trig->getPos()))
        {
            float dist = Vec2DistanceSq(botPos, trig->getPos());

            if ( dist < ClosestDist)
            {
                ClosestDist = dist;
                closest = trig->getPos();
            }
        }
    }

    return closest;
}

void GameWorldRaven::render()
{
    if(!m_ui)
    {
        //for ui
        m_ui = (pNode)getNewNode();
        addChildToUI(m_ui);
    }
}


