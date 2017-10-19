
#include "GameConfig.h"
#include "Raven_Bot.h"

#include "common/misc/Regulator.h"
#include "common/misc/LogDebug.h"
#include "common/misc/UtilsEx.h"
#include "common/message/Telegram.h"
#include "common/message/MessageDispatcher.h"
#include "../GameWorldRaven.h"
#include "../navigation/Raven_PathPlanner.h"
#include "../weapon_handling/Raven_WeaponSystem.h"
#include "../sensor_memory/Raven_SensoryMemory.h"
#include "../goals/Raven_Goal_Types.h"
#include "../goals/Goal_Think.h"

#include "ParaConfigRaven.h"
#include "RavenMessages.h"
#include "Raven_SteeringBehaviors.h"



//-------------------------- ctor ---------------------------------------------
Raven_Bot::Raven_Bot(GameWorldRaven* world,Vector2D pos):
                                                        MovingEntity(Vector2D(0,0),
                                                                           Vector2D(1,0),
                                                                           Para_Bot_Mass,
                                                                           Para_Bot_MaxHeadTurnRate,
                                                                           Para_Bot_MaxSpeed,
                                                                           Para_Bot_MaxForce),
                                                        m_iMaxHealth(Para_Bot_MaxHealth),
                                                        m_iHealth(Para_Bot_MaxHealth),
                                                        m_pPathPlanner(NULL),
                                                        m_pSteering(NULL),
                                                        m_pWorld(world),
                                                        m_pBrain(NULL),
                                                        m_iNumUpdatesHitPersistant((int)(FrameRate * Para_HitFlashTime)),
                                                        m_bHit(false),
                                                        m_iScore(0),
                                                        m_Status(spawning),
                                                        m_bPossessed(false),
                                                        m_dFieldOfView(degreeToRadians(Para_Bot_FOV))
           
{
    setEntityType(type_bot);

    //a bot starts off facing in the direction it is heading
    m_vFacing = m_vHeading;

    //create the navigation module
    m_pPathPlanner = new Raven_PathPlanner(this);

    //create the steering behavior class
    m_pSteering = new Raven_SteeringBehaviors(world, this);

    //create the regulators
    m_pWeaponSelectionRegulator = new Regulator(Para_Bot_WeaponSelectionFrequency);
    m_pGoalArbitrationRegulator =  new Regulator(Para_Bot_GoalAppraisalUpdateFreq);
    m_pTargetSelectionRegulator = new Regulator(Para_Bot_TargetingUpdateFreq);
    m_pTriggerTestRegulator = new Regulator(Para_Bot_TriggerUpdateFreq);
    m_pVisionUpdateRegulator = new Regulator(Para_Bot_VisionUpdateFreq);

    //create the goal queue
    m_pBrain = new Goal_Think(this);

    //create the targeting system
    m_pTargSys = new Raven_TargetingSystem(this);

    m_pWeaponSys = new Raven_WeaponSystem(  this,
                                                                            Para_Bot_ReactionTime,
                                                                            Para_Bot_AimAccuracy,
                                                                            Para_Bot_AimPersistance);

    m_pSensoryMem = new Raven_SensoryMemory(this, Para_Bot_MemorySpan);
}

//-------------------------------- dtor ---------------------------------------
//-----------------------------------------------------------------------------
Raven_Bot::~Raven_Bot()
{
    AILOG("deleting raven bot (id = %d)", getID());

    delete m_pBrain;
    delete m_pPathPlanner;
    delete m_pSteering;
    delete m_pWeaponSelectionRegulator;
    delete m_pTargSys;
    delete m_pGoalArbitrationRegulator;
    delete m_pTargetSelectionRegulator;
    delete m_pTriggerTestRegulator;
    delete m_pVisionUpdateRegulator;
    delete m_pWeaponSys;
    delete m_pSensoryMem;
}

//------------------------------- Spawn ---------------------------------------
//
//  spawns the bot at the given position
//-----------------------------------------------------------------------------
void Raven_Bot::spawn(Vector2D pos)
{
    setAlive();
    m_pBrain->removeAllSubgoals();
    m_pTargSys->clearTarget();
    setPos(pos);
    m_pWeaponSys->initialize();
    restoreHealthToMaximum();
}

//-------------------------------- Update -------------------------------------
//
void Raven_Bot::update()
{
    //process the currently active goal. Note this is required even if the bot
    //is under user control. This is because a goal is created whenever a user 
    //clicks on an area of the map that necessitates a path planning request.
    m_pBrain->process();
  
    //Calculate the steering force and update the bot's velocity and position
    updateMovement();

    //if the bot is under AI control but not scripted
    if (!isPossessed())
    {           
        //examine all the opponents in the bots sensory memory and select one
        //to be the current target
        if (m_pTargetSelectionRegulator->isReady())
        {      
            m_pTargSys->update();
        }

        //appraise and arbitrate between all possible high level goals
        if (m_pGoalArbitrationRegulator->isReady())
        {
            m_pBrain->arbitrate(); 
        }

        //update the sensory memory with any visual stimulus
        if (m_pVisionUpdateRegulator->isReady())
        {
            m_pSensoryMem->updateVision();
        }

        //select the appropriate weapon to use from the weapons currently in
        //the inventory
        if (m_pWeaponSelectionRegulator->isReady())
        {       
            m_pWeaponSys->selectWeapon();       
        }

        //this method aims the bot's current weapon at the current target
        //and takes a shot if a shot is possible
        m_pWeaponSys->takeAimAndShoot();
    }
}


//------------------------- updateMovement ------------------------------------
//
//  this method is called from the update method. It calculates and applies
//  the steering force for this time-step.
//-----------------------------------------------------------------------------
void Raven_Bot::updateMovement()
{
    //calculate the combined steering force
    Vector2D force = m_pSteering->calculate();

    //if no steering force is produced decelerate the player by applying a
    //braking force
    if (m_pSteering->getForce().isZero())
    {
        const float BrakingRate = 0.8f; 

        m_vVelocity = m_vVelocity * BrakingRate;                                     
    }

    //calculate the acceleration
    Vector2D accel = force / m_dMass;

    //update the velocity
    m_vVelocity += accel;

    //make sure vehicle does not exceed maximum velocity
    m_vVelocity.truncate(m_dMaxSpeed);

    //update the position
    setPos(getPos() + m_vVelocity);

    //if the vehicle has a non zero velocity the heading and side vectors must 
    //be updated
    if (!m_vVelocity.isZero())
    {    
        m_vHeading = Vec2Normalize(m_vVelocity);

        m_vSide = m_vHeading.getPerp();
    }

    
    render();
}
//---------------------------- isReadyForTriggerUpdate ------------------------
//
//  returns true if the bot is ready to be tested against the world triggers
//-----------------------------------------------------------------------------
bool Raven_Bot::isReadyForTriggerUpdate()const
{
    return m_pTriggerTestRegulator->isReady();
}

//--------------------------- handleMessage -----------------------------------
//-----------------------------------------------------------------------------
bool Raven_Bot::handleMessage(const Telegram& msg)
{
    //first see if the current goal accepts the message
    if (getBrain()->handleMessage(msg)) return true;
 
    //handle any messages not handles by the goals
    switch(msg.m_msgId)
    {
        case Msg_TakeThatMF:
        {
            //just return if already dead or spawning
            if (isDead() || isSpawning()) return true;

            //the extra info field of the telegram carries the amount of damage
            reduceHealth(dereferenceToType<int>(msg.m_extraInfo));

            //if this bot is now dead let the shooter know
            if (isDead())
            {
                MessageDispatcher::instance()->dispatchMsg( 0, 
                                                                                    getID(), 
                                                                                    msg.m_sender,
                                                                                    Msg_YouGotMeYouSOB, 
                                                                                    nullptr);
            
            }
            return true;
        }
        case Msg_YouGotMeYouSOB:
        {
            incrementScore();
            //the bot this bot has just killed should be removed as the target
            m_pTargSys->clearTarget();
            return true;
        }
        
        case Msg_GunshotSound:
        {
            //add the source of this sound to the bot's percepts
            getSensoryMem()->updateWithSoundSource((Raven_Bot*)msg.m_extraInfo);
            return true;
        }
        
        case Msg_UserHasRemovedBot:
        {
            Raven_Bot* pRemovedBot = (Raven_Bot*)msg.m_extraInfo;

            getSensoryMem()->removeBotFromMemory(pRemovedBot);

            //if the removed bot is the target, make sure the target is cleared
            if (pRemovedBot == getTargetSys()->getTarget())
            {
                getTargetSys()->clearTarget();
            }
            return true;
        }

        default: 
            return false;
    }
}

//------------------ rotateFacingTowardPosition -------------------------------
//
//  given a target position, this method rotates the bot's facing vector
//  by an amount not greater than m_dMaxTurnRate until it
//  directly faces the target.
//
//  returns true when the heading is facing in the desired direction
//----------------------------------------------------------------------------
bool Raven_Bot::rotateFacingTowardPosition(Vector2D target)
{
    Vector2D toTarget = Vec2Normalize(target - getPos());

    float dot = m_vFacing.dot(toTarget);

    //clamp to rectify any rounding errors
    clamp(dot, -1, 1);

    //determine the angle between the heading vector and the target
    float angle = acos(dot);

    //return true if the bot's facing is within WeaponAimTolerance degs of
    //facing the target
    const float WeaponAimTolerance = 0.01f;  //degs approx

    if (angle < WeaponAimTolerance)
    {
        m_vFacing = toTarget;
        return true;
    }

    //clamp the amount to turn to the max turn rate
    if (angle > m_dMaxTurnRate) angle = m_dMaxTurnRate;
  
    //The next few lines use a rotation matrix to rotate the player's facing
    //vector accordingly
    Matrix2D RotationMatrix;
  
    //notice how the direction of rotation has to be determined when creating
    //the rotation matrix
    RotationMatrix.Rotate(angle * m_vFacing.sign(toTarget));
    RotationMatrix.TransformVector2Ds(m_vFacing);

    return false;
}




//--------------------------------- reduceHealth ----------------------------
void Raven_Bot::reduceHealth(unsigned int val)
{
    m_iHealth -= val;

    if (m_iHealth <= 0)
    {
        setDead();
    }
    m_bHit = true;

    m_iNumUpdatesHitPersistant = (int)(FrameRate * Para_HitFlashTime);
}

//--------------------------- Possess -----------------------------------------
//
//  this is called to allow a human player to control the bot
//-----------------------------------------------------------------------------
void Raven_Bot::takePossession()
{
    if ( !(isSpawning() || isDead()))
    {
        m_bPossessed = true;
        AILOG("Player Possesses bot  %d", getID());
    }
}

//------------------------------- exorcise ------------------------------------
//
//  called when a human is exorcised from this bot and the AI takes control
//-----------------------------------------------------------------------------
void Raven_Bot::exorcise()
{
    m_bPossessed = false;

    //when the player is exorcised then the bot should resume normal service
    m_pBrain->addGoal_Explore();
    AILOG("Player is exorcised from bot  %d", getID());
}


//----------------------- changeWeapon ----------------------------------------
void Raven_Bot::changeWeapon(unsigned int type)
{
    m_pWeaponSys->changeWeapon(type);
}
  

//---------------------------- fireWeapon -------------------------------------
//
//  fires the current weapon at the given position
//-----------------------------------------------------------------------------
void Raven_Bot::fireWeapon(Vector2D pos)
{
    m_pWeaponSys->shootAt(pos);
}

//----------------- CalculateExpectedTimeToReachPosition ----------------------
//
//  returns a value indicating the time in seconds it will take the bot
//  to reach the given position at its current speed.
//-----------------------------------------------------------------------------
float Raven_Bot::calculateTimeToReachPosition(Vector2D pos)const
{
    return Vec2Distance(getPos(), pos) / (getMaxSpeed() * FrameRate);
}

//------------------------ isAtPosition ---------------------------------------
//
//  returns true if the bot is close to the given position
//-----------------------------------------------------------------------------
bool Raven_Bot::isAtPosition(Vector2D pos)const
{
    const static float tolerance = 10.0;

    return Vec2DistanceSq(getPos(), pos) < tolerance * tolerance;
}

//------------------------- hasLOSt0 ------------------------------------------
//
//  returns true if the bot has line of sight to the given position.
//-----------------------------------------------------------------------------
bool Raven_Bot::hasLOSto(Vector2D pos)const
{
    return m_pWorld->isLOSOkay(getPos(), pos);
}

//returns true if this bot can move directly to the given position
//without bumping into any walls
bool Raven_Bot::canWalkTo(Vector2D pos)const
{
    return !m_pWorld->isPathObstructed(getPos(), pos, getBoundingRadius());
}

//similar to above. Returns true if the bot can move between the two
//given positions without bumping into any walls
bool Raven_Bot::canWalkBetween(Vector2D from, Vector2D to)const
{
    return !m_pWorld->isPathObstructed(from, to, getBoundingRadius());
}

//--------------------------- canStep Methods ---------------------------------
//
//  returns true if there is space enough to step in the indicated direction
//  If true PositionOfStep will be assigned the offset position
//-----------------------------------------------------------------------------
bool Raven_Bot::canStepLeft(Vector2D& PositionOfStep)const
{
    static const float StepDistance = getBoundingRadius() * 2;

    PositionOfStep = getPos() - getFacing().getPerp() * StepDistance - getFacing().getPerp() * getBoundingRadius();

    return canWalkTo(PositionOfStep);
}

bool Raven_Bot::canStepRight(Vector2D& PositionOfStep)const
{
    static const float StepDistance = getBoundingRadius() * 2;

    PositionOfStep = getPos() + getFacing().getPerp() * StepDistance + getFacing().getPerp() * getBoundingRadius();

    return canWalkTo(PositionOfStep);
}

bool Raven_Bot::canStepForward(Vector2D& PositionOfStep)const
{
    static const float StepDistance = getBoundingRadius() * 2;

    PositionOfStep = getPos() + getFacing() * StepDistance + getFacing() * getBoundingRadius();

    return canWalkTo(PositionOfStep);
}

bool Raven_Bot::canStepBackward(Vector2D& PositionOfStep)const
{
    static const float StepDistance = getBoundingRadius() * 2;

    PositionOfStep = getPos() - getFacing() * StepDistance - getFacing() * getBoundingRadius();

    return canWalkTo(PositionOfStep);
}


void Raven_Bot::restoreHealthToMaximum()
{
    m_iHealth = m_iMaxHealth;
}

void Raven_Bot::increaseHealth(unsigned int val)
{
    m_iHealth+=val; 
    clamp(m_iHealth, 0, m_iMaxHealth);
}

void Raven_Bot::render()
{
    if(!m_ui)
    {
        //for ui
        m_ui = (pNode)getNewNode();
        loadImgToNode(m_ui, "game_raven/raven_bot.png");
        addChildToUI(m_ui);
    }
    setNodePos(m_ui, getPos());
}
