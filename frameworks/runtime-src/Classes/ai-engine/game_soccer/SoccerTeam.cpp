
#include "SoccerTeam.h"
#include "SoccerPitch.h"
#include "SoccerGoal.h"
#include "PlayerBase.h"
#include "GoalKeeper.h"
#include "FieldPlayer.h"
#include "SteeringBehaviors_Soccer.h"
#include "GoalKeeperStates.h"
#include "common/2D/Geometry.h"
#include "common/game/EntityManager.h"
#include "common/message/MessageDispatcher.h"
#include "SoccerMessages.h"
#include "TeamStates.h"
#include "ParaConfigSoccer.h"

using std::vector;



SoccerTeam::SoccerTeam( SoccerGoal* home_goal,
                                               SoccerGoal* opponents_goal,
                                               SoccerPitch* pitch,
                                               team_color color):m_pOpponentsGoal(opponents_goal),
                                                                           m_pHomeGoal(home_goal),
                                                                           m_pOpponents(NULL),
                                                                           m_pPitch(pitch),
                                                                           m_Color(color),
                                                                           m_dDistSqToBallOfClosestPlayer(0.0),
                                                                           m_pSupportingPlayer(NULL),
                                                                           m_pReceivingPlayer(NULL),
                                                                           m_pControllingPlayer(NULL),
                                                                           m_pPlayerClosestToBall(NULL)
{
    //setup the state machine
    m_pStateMachine = new StateMachine<SoccerTeam>(this);

    m_pStateMachine->setCurrentState(Defending::instance());
    m_pStateMachine->setPreviousState(Defending::instance());
    m_pStateMachine->setGlobalState(NULL);

    //create the players and goalkeeper
    createPlayers();
  
    //set default steering behaviors
    std::vector<PlayerBase*>::iterator it = m_Players.begin();

    for (it; it != m_Players.end(); ++it)
    {
        (*it)->getSteering()->separationOn();   
    }

    //create the sweet spot calculator
    m_pSupportSpotCalc = new SupportSpotCalculator( Para_NumSupportSpotsX,
                                                                                 Para_NumSupportSpotsY,
                                                                                 this);
}


SoccerTeam::~SoccerTeam()
{
    delete m_pStateMachine;
    
    std::vector<PlayerBase*>::iterator it = m_Players.begin();
    for (it; it != m_Players.end(); ++it)
    {
        delete *it;
    }

    delete m_pSupportSpotCalc;
}

//-------------------------- update --------------------------------------
//
//  iterates through each player's update function and calculates 
//  frequently accessed info
//------------------------------------------------------------------------
void SoccerTeam::update(float dt)
{
    //this information is used frequently so it's more efficient to 
    //calculate it just once each frame
    calculateClosestPlayerToBall();

    //the team state machine switches between attack/defense behavior. It
    //also handles the 'kick off' state where a team must return to their
    //kick off positions before the whistle is blown
    m_pStateMachine->update(dt);
  
    //now update each player
    std::vector<PlayerBase*>::iterator it = m_Players.begin();
    for (it; it != m_Players.end(); ++it)
    {
        (*it)->update(dt);
    }
}


//------------------------ CalculateClosestPlayerToBall ------------------
//
//  sets m_iClosestPlayerToBall to the player closest to the ball
//------------------------------------------------------------------------
void SoccerTeam::calculateClosestPlayerToBall()
{
    float ClosestSoFar = FloatMax;

    std::vector<PlayerBase*>::iterator it = m_Players.begin();

    for (it; it != m_Players.end(); ++it)
    {
        //calculate the dist. Use the squared value to avoid sqrt
        float dist = Vec2DistanceSq((*it)->getPos(), getPitch()->getBall()->getPos());

        //keep a record of this value for each player
        (*it)->setDistSqToBall(dist);

        if (dist < ClosestSoFar)
        {
            ClosestSoFar = dist;
            m_pPlayerClosestToBall = *it;
        }
    }

    m_dDistSqToBallOfClosestPlayer = ClosestSoFar;
}


//------------- DetermineBestSupportingAttacker ------------------------
//
// calculate the closest player to the SupportSpot
//------------------------------------------------------------------------
PlayerBase* SoccerTeam::determineBestSupportingAttacker()
{
    float ClosestSoFar = FloatMax;

    PlayerBase* BestPlayer = NULL;

    std::vector<PlayerBase*>::iterator it = m_Players.begin();

    for (it; it != m_Players.end(); ++it)
    {
        //only attackers utilize the BestSupportingSpot
        if ( ((*it)->getRole() == PlayerBase::attacker) && ((*it) != m_pControllingPlayer) )
        {
            //calculate the dist. Use the squared value to avoid sqrt
            float dist = Vec2DistanceSq((*it)->getPos(), m_pSupportSpotCalc->getBestSupportingSpot());

            //if the distance is the closest so far and the player is not a
            //goalkeeper and the player is not the one currently controlling
            //the ball, keep a record of this player
            if ((dist < ClosestSoFar) )
            {
                ClosestSoFar = dist;
                BestPlayer = (*it);
            }
        }
    }

    return BestPlayer;
}

//-------------------------- FindPass ------------------------------
//
//  The best pass is considered to be the pass that cannot be intercepted 
//  by an opponent and that is as far forward of the receiver as possible
//------------------------------------------------------------------------
bool SoccerTeam::findPass(const PlayerBase*const passer,
                                            PlayerBase*& receiver,
                                            Vector2D& passTarget,
                                            float power,
                                            float minPassingDistance) const
{  
    std::vector<PlayerBase*>::const_iterator curPlyr = getMembers().begin();

    float ClosestToGoalSoFar = FloatMax;
    Vector2D Target;

    //iterate through all this player's team members and calculate which
    //one is in a position to be passed the ball 
    for (curPlyr; curPlyr != getMembers().end(); ++curPlyr)
    {   
        //make sure the potential receiver being examined is not this player
        //and that it is further away than the minimum pass distance
        if ( (*curPlyr != passer) &&(Vec2DistanceSq(passer->getPos(), (*curPlyr)->getPos()) > 
                        minPassingDistance*minPassingDistance))                  
        {           
            if (getBestPassToReceiver(passer, *curPlyr, Target, power))
            {
                //if the pass target is the closest to the opponent's goal line found
                // so far, keep a record of it
                float Dist2Goal = fabs(Target.x - getOpponentsGoal()->center().x);

                if (Dist2Goal < ClosestToGoalSoFar)
                {
                    ClosestToGoalSoFar = Dist2Goal;

                    //keep a record of this player
                    receiver = *curPlyr;

                    //and the target
                    passTarget = Target;
                }     
            }
        }
    }//next team member

    if (receiver) 
        return true;
    else
        return false;
}


//---------------------- getBestPassToReceiver ---------------------------
//
//  Three potential passes are calculated. One directly toward the receiver's
//  current position and two that are the tangents from the ball position
//  to the circle of radius 'range' from the receiver.
//  These passes are then tested to see if they can be intercepted by an
//  opponent and to make sure they terminate within the playing area. If
//  all the passes are invalidated the function returns false. Otherwise
//  the function returns the pass that takes the ball closest to the 
//  opponent's goal area.
//------------------------------------------------------------------------
bool SoccerTeam::getBestPassToReceiver(const PlayerBase* const passer,
                                                                           const PlayerBase* const receiver,
                                                                           Vector2D& passTarget,
                                                                           float power)const
{  
    //first, calculate how much time it will take for the ball to reach 
    //this receiver, if the receiver was to remain motionless 
    float time = getPitch()->getBall()->timeToCoverDistance(getPitch()->getBall()->getPos(),
                                                                                        receiver->getPos(),
                                                                                        power);

    //return false if ball cannot reach the receiver after having been
    //kicked with the given power
    if (time < 0) return false;

    //the maximum distance the receiver can cover in this time
    float InterceptRange = time * receiver->getMaxSpeed();
  
    //Scale the intercept range
    const float ScalingFactor = 0.3f;
    InterceptRange *= ScalingFactor;

    //now calculate the pass targets which are positioned at the intercepts
    //of the tangents from the ball to the receiver's range circle.
    Vector2D ip1, ip2;

    getTangentPoints(receiver->getPos(),
                                   InterceptRange,
                                   getPitch()->getBall()->getPos(),
                                   ip1,
                                   ip2);
 
    const int NumPassesToTry = 3;
    Vector2D Passes[NumPassesToTry] = {ip1, receiver->getPos(), ip2};
  
  
    // this pass is the best found so far if it is:
    //
    //  1. Further upfield than the closest valid pass for this receiver found so far
    //  2. Within the playing area
    //  3. Cannot be intercepted by any opponents

    float ClosestSoFar = FloatMax;
    bool  bResult = false;

    for (int pass=0; pass<NumPassesToTry; ++pass)
    {    
        float dist = fabs(Passes[pass].x - getOpponentsGoal()->center().x);

        if (( dist < ClosestSoFar) && getPitch()->getPlayingArea()->isInside(Passes[pass]) &&
                                        isPassSafeFromAllOpponents(getPitch()->getBall()->getPos(),
                                                                                   Passes[pass],
                                                                                   receiver,
                                                                                   power))
            
        {
            ClosestSoFar = dist;
            passTarget   = Passes[pass];
            bResult      = true;
        }
    }

    return bResult;
}

//----------------------- isPassSafeFromOpponent -------------------------
//
//  test if a pass from 'from' to 'to' can be intercepted by an opposing
//  player
//------------------------------------------------------------------------
bool SoccerTeam::isPassSafeFromOpponent( Vector2D from,
                                                                                Vector2D target,
                                                                                const PlayerBase* const receiver,
                                                                                const PlayerBase* const opp,
                                                                                float passingForce)const
{
    //move the opponent into local space.
    Vector2D ToTarget = target - from;
    Vector2D ToTargetNormalized = Vec2Normalize(ToTarget);

    Vector2D LocalPosOpp = PointToLocalSpace( opp->getPos(),
                                                                             ToTargetNormalized,
                                                                             ToTargetNormalized.getPerp(),
                                                                             from);

    //if opponent is behind the kicker then pass is considered okay(this is 
    //based on the assumption that the ball is going to be kicked with a 
    //velocity greater than the opponent's max velocity)
    if ( LocalPosOpp.x < 0 )
    {     
        return true;
    }
  
    //if the opponent is further away than the target we need to consider if
    //the opponent can reach the position before the receiver.
    if (Vec2DistanceSq(from, target) < Vec2DistanceSq(opp->getPos(), from))
    {
        if (receiver)
        {
            if ( Vec2DistanceSq(target, opp->getPos()) > Vec2DistanceSq(target, receiver->getPos()) )
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            return true;
        } 
    }
  
    //calculate how long it takes the ball to cover the distance to the 
    //position orthogonal to the opponents position
    float TimeForBall = getPitch()->getBall()->timeToCoverDistance(Vector2D(0,0),
                                                                                       Vector2D(LocalPosOpp.x, 0),
                                                                                       passingForce);

    //now calculate how far the opponent can run in this time
    float reach = opp->getMaxSpeed() * TimeForBall + getPitch()->getBall()->getBoundingRadius()+
                                    opp->getBoundingRadius();

    //if the distance to the opponent's y position is less than his running
    //range plus the radius of the ball and the opponents radius then the
    //ball can be intercepted
    if ( fabs(LocalPosOpp.y) < reach )
    {
        return false;
    }

    return true;
}

//---------------------- isPassSafeFromAllOpponents ----------------------
//
//  tests a pass from position 'from' to position 'target' against each member
//  of the opposing team. Returns true if the pass can be made without
//  getting intercepted
//------------------------------------------------------------------------
bool SoccerTeam::isPassSafeFromAllOpponents( Vector2D from,
                                                                                Vector2D target,
                                                                                const PlayerBase* const receiver,
                                                                                float passingForce) const
{
    std::vector<PlayerBase*>::const_iterator opp = getOpponents()->getMembers().begin();

    for (opp; opp != getOpponents()->getMembers().end(); ++opp)
    {
        if (!isPassSafeFromOpponent(from, target, receiver, *opp, passingForce))
        {
            return false;
        }
    }

    return true;
}

//------------------------ CanShoot --------------------------------------
//
//  Given a ball position, a kicking power and a reference to a vector2D
//  this function will sample random positions along the opponent's goal-
//  mouth and check to see if a goal can be scored if the ball was to be
//  kicked in that direction with the given power. If a possible shot is 
//  found, the function will immediately return true, with the target 
//  position stored in the vector ShotTarget.
//------------------------------------------------------------------------
bool SoccerTeam::canShoot(Vector2D BallPos, float power, Vector2D& ShotTarget) const
{
    //the number of randomly created shot targets this method will test 
    int NumAttempts = Para_NumAttemptsToFindValidStrike;

    while (NumAttempts--)
    {
        //choose a random position along the opponent's goal mouth. (making
        //sure the ball's radius is taken into account)
        ShotTarget = getOpponentsGoal()->center();

        //the y value of the shot position should lay somewhere between two
        //goalposts (taking into consideration the ball diameter)
        int MinYVal = (int)(getOpponentsGoal()->getLeftPost().y + getPitch()->getBall()->getBoundingRadius());
        int MaxYVal = (int)(getOpponentsGoal()->getRightPost().y - getPitch()->getBall()->getBoundingRadius());

        ShotTarget.y =  (float)RandIntInRange(MinYVal, MaxYVal);

        //make sure striking the ball with the given power is enough to drive
        //the ball over the goal line.
        float time = getPitch()->getBall()->timeToCoverDistance(BallPos,ShotTarget,power);
        //if it is, this shot is then tested to see if any of the opponents can intercept it.
        if (time >= 0)
        {
            if (isPassSafeFromAllOpponents(BallPos, ShotTarget, NULL, power))
            {
                return true;
            }
        }
    }
  
    return false;
}

 
//--------------------- ReturnAllFieldPlayersToHome ---------------------------
//
//  sends a message to all players to return to their home areas forthwith
//------------------------------------------------------------------------
void SoccerTeam::returnAllFieldPlayersToHome()const
{
    std::vector<PlayerBase*>::const_iterator it = m_Players.begin();

    for (it; it != m_Players.end(); ++it)
    {
        if ((*it)->getRole() != PlayerBase::goal_keeper)
        {
            MessageDispatcher::instance()->dispatchMsg( 0, 
                                                                                1, 
                                                                                (*it)->getID(),
                                                                                Msg_GoHome,
                                                                                NULL);
        }
    }
}


//------------------------- createPlayers --------------------------------
//
//  creates the players
//------------------------------------------------------------------------
void SoccerTeam::createPlayers()
{
    if (getTeamColor() == blue)
    {
        //goalkeeper
        m_Players.push_back(new GoalKeeper(this,
                                                                   1,
                                                                   TendGoal::instance(),
                                                                   Vector2D(0,1),
                                                                   Vector2D(0.0, 0.0),
                                                                   Para_PlayerMass,
                                                                   Para_PlayerMaxForce,
                                                                   Para_PlayerMaxSpeedWithoutBall,
                                                                   Para_PlayerMaxTurnRate));
 
        //create the players
        m_Players.push_back(new FieldPlayer(this,
                                                                   6,
                                                                   Wait::instance(),
                                                                   Vector2D(0,1),
                                                                   Vector2D(0.0, 0.0),
                                                                   (float)Para_PlayerMass,
                                                                   (float)Para_PlayerMaxForce,
                                                                   (float)Para_PlayerMaxSpeedWithoutBall,
                                                                   (float)Para_PlayerMaxTurnRate,
                                                                   PlayerBase::attacker));

        m_Players.push_back(new FieldPlayer(this,
                                                                   8,
                                                                   Wait::instance(),
                                                                   Vector2D(0,1),
                                                                   Vector2D(0.0, 0.0),
                                                                   Para_PlayerMass,
                                                                   Para_PlayerMaxForce,
                                                                   Para_PlayerMaxSpeedWithoutBall,
                                                                   Para_PlayerMaxTurnRate,
                                                                   PlayerBase::attacker));

        m_Players.push_back(new FieldPlayer(this,
                                                                   3,
                                                                   Wait::instance(),
                                                                   Vector2D(0,1),
                                                                   Vector2D(0.0, 0.0),
                                                                   Para_PlayerMass,
                                                                   Para_PlayerMaxForce,
                                                                   Para_PlayerMaxSpeedWithoutBall,
                                                                   Para_PlayerMaxTurnRate,
                                                                   PlayerBase::defender));

        m_Players.push_back(new FieldPlayer(this,
                                                                   5,
                                                                   Wait::instance(),
                                                                   Vector2D(0,1),
                                                                   Vector2D(0.0, 0.0),
                                                                   Para_PlayerMass,
                                                                   Para_PlayerMaxForce,
                                                                   Para_PlayerMaxSpeedWithoutBall,
                                                                   Para_PlayerMaxTurnRate,
                                                                  PlayerBase::defender));
    }
    else
    {
         //goalkeeper
        m_Players.push_back(new GoalKeeper(this,
                                                                   16,
                                                                   TendGoal::instance(),
                                                                   Vector2D(0,-1),
                                                                   Vector2D(0.0, 0.0),
                                                                   Para_PlayerMass,
                                                                   Para_PlayerMaxForce,
                                                                   Para_PlayerMaxSpeedWithoutBall,
                                                                   Para_PlayerMaxTurnRate));

        //create the players
        m_Players.push_back(new FieldPlayer(this,
                                                                   9,
                                                                   Wait::instance(),
                                                                   Vector2D(0,-1),
                                                                   Vector2D(0.0, 0.0),
                                                                   Para_PlayerMass,
                                                                   Para_PlayerMaxForce,
                                                                   Para_PlayerMaxSpeedWithoutBall,
                                                                   Para_PlayerMaxTurnRate,
                                                                   PlayerBase::attacker));

        m_Players.push_back(new FieldPlayer(this,
                                                                   11,
                                                                   Wait::instance(),
                                                                   Vector2D(0,-1),
                                                                   Vector2D(0.0, 0.0),
                                                                   Para_PlayerMass,
                                                                   Para_PlayerMaxForce,
                                                                   Para_PlayerMaxSpeedWithoutBall,
                                                                   Para_PlayerMaxTurnRate,
                                                                   PlayerBase::attacker));

        m_Players.push_back(new FieldPlayer(this,
                                                                   12,
                                                                   Wait::instance(),
                                                                   Vector2D(0,-1),
                                                                   Vector2D(0.0, 0.0),
                                                                   Para_PlayerMass,
                                                                   Para_PlayerMaxForce,
                                                                   Para_PlayerMaxSpeedWithoutBall,
                                                                   Para_PlayerMaxTurnRate,
                                                                   PlayerBase::defender));

        m_Players.push_back(new FieldPlayer(this,
                                                                   14,
                                                                   Wait::instance(),
                                                                   Vector2D(0,-1),
                                                                   Vector2D(0.0, 0.0),
                                                                   Para_PlayerMass,
                                                                   Para_PlayerMaxForce,
                                                                   Para_PlayerMaxSpeedWithoutBall,
                                                                   Para_PlayerMaxTurnRate,
                                                                   PlayerBase::defender));
    }

    //register the players with the entity manager
    std::vector<PlayerBase*>::iterator it = m_Players.begin();
    
    for (it; it != m_Players.end(); ++it)
    {
        EntityManager::instance()->addEntity(*it);
    }
}


PlayerBase* SoccerTeam::getPlayerFromID(int id)const
{
    std::vector<PlayerBase*>::const_iterator it = m_Players.begin();

    for (it; it != m_Players.end(); ++it)
    {
        if ((*it)->getID() == id) return *it;
    }

    return NULL;
}


void SoccerTeam::setPlayerHomeRegion(int plyr, int region)const
{
    assert ( (plyr>=0) && (plyr<(int)m_Players.size()) );

    m_Players[plyr]->setHomeRegion(region);
}


//---------------------- UpdateTargetsOfWaitingPlayers ------------------------
//
//  
void SoccerTeam::updateTargetsOfWaitingPlayers()const
{
    std::vector<PlayerBase*>::const_iterator it = m_Players.begin();

    for (it; it != m_Players.end(); ++it)
    {  
        if ( (*it)->getRole() != PlayerBase::goal_keeper )
        {
            //cast to a field player
            FieldPlayer* plyr = static_cast<FieldPlayer*>(*it);

            if ( plyr->getFSM()->isInState(*Wait::instance()) ||
            plyr->getFSM()->isInState(*ReturnToHomeRegion::instance()) )
            {
                plyr->getSteering()->setTarget(plyr->getHomeRegion()->center());
            }
        }
    }
}


//--------------------------- AllPlayersAtHome --------------------------------
//
//  returns false if any of the team are not located within their home region
//-----------------------------------------------------------------------------
bool SoccerTeam::isAllPlayersAtHome()const
{
    std::vector<PlayerBase*>::const_iterator it = m_Players.begin();

    for (it; it != m_Players.end(); ++it)
    {
        if ((*it)->isInHomeRegion() == false)
        {
            return false;
        }
    }

    return true;
}

//------------------------- RequestPass ---------------------------------------
//
//  this tests to see if a pass is possible between the requester and
//  the controlling player. If it is possible a message is sent to the
//  controlling player to pass the ball asap.
//-----------------------------------------------------------------------------
void SoccerTeam::requestPass(FieldPlayer* requester)const
{
    //maybe put a restriction here
    if (RandFloat_0_1() > 0.1) return;
  
    if (isPassSafeFromAllOpponents(getControllingPlayer()->getPos(),
                                                     requester->getPos(),
                                                     requester,
                                                     Para_MaxPassingForce))
    {

        //tell the player to make the pass
        //let the receiver know a pass is coming 
        MessageDispatcher::instance()->dispatchMsg(0,
                                                                            requester->getID(),
                                                                            getControllingPlayer()->getID(),
                                                                            Msg_PassToMe,
                                                                            requester); 
    }
}


//----------------------------- isOpponentWithinRadius ------------------------
//
//  returns true if an opposing player is within the radius of the position
//  given as a parameter
//-----------------------------------------------------------------------------
bool SoccerTeam::isOpponentWithinRadius(Vector2D pos, float rad)
{
    std::vector<PlayerBase*>::const_iterator end = getOpponents()->getMembers().end();
    std::vector<PlayerBase*>::const_iterator it;

    for (it=getOpponents()->getMembers().begin(); it !=end; ++it)
    {
        if (Vec2DistanceSq(pos, (*it)->getPos()) < rad*rad)
        {
            return true;
        }
    }

    return false;
}
