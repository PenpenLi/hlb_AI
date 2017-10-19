
#include "ParaConfigSoccer.h"
#include "PlayerBase.h"
#include "SteeringBehaviors_Soccer.h"
#include "common/2D/Matrix2D.h"
#include "common/game/Region.h"
#include "common/message/MessageDispatcher.h"
#include "SoccerMessages.h"
#include "SoccerTeam.h"
#include "SoccerGoal.h"
#include "SoccerBall.h"
#include "SoccerPitch.h"


PlayerBase::~PlayerBase()
{
    delete m_pSteering;
}

//----------------------------- ctor -------------------------------------
//------------------------------------------------------------------------
PlayerBase::PlayerBase( SoccerTeam* team,
                                            int home_region,
                                            Vector2D heading,
                                            Vector2D velocity,
                                            float mass,
                                            float max_force,
                                            float max_speed,
                                            float max_turn_rate,
                                            player_role role): MovingEntity(velocity,
                                                                                            heading,
                                                                                            mass,
                                                                                            max_turn_rate,
                                                                                            max_speed,
                                                                                            max_force),
                                                                       m_pTeam(team),
                                                                       m_distSqToBall(FloatMax),
                                                                       m_homeRegion(home_region),
                                                                       m_defaultRegion(home_region),
                                                                       m_PlayerRole(role)
{
    //set up the steering behavior class
    m_pSteering = new SteeringBehaviors_Soccer(this, m_pTeam->getPitch(), getBall());  

    setPos(team->getPitch()->getRegionFromIndex(home_region)->center());
    
    //a player's start target is its start position (because it's just waiting)
    m_pSteering->setTarget(team->getPitch()->getRegionFromIndex(home_region)->center());
}




//----------------------------- trackBall --------------------------------
//
//  sets the player's heading to point at the ball
//------------------------------------------------------------------------
void PlayerBase::trackBall()
{
    rotateHeadingToFacePosition(getBall()->getPos());  
}

//----------------------------- trackTarget --------------------------------
//
//  sets the player's heading to point at the current target
//------------------------------------------------------------------------
void PlayerBase::trackTarget()
{
    setHeading(Vec2Normalize(getSteering()->getTarget() - this->getPos()));
}

//------------------------- positionInFrontOfPlayer ---------------------------
//
//  returns true if subject is within field of view of this player
//-----------------------------------------------------------------------
bool PlayerBase::positionInFrontOfPlayer(Vector2D position)const
{
    Vector2D ToSubject = position - this->getPos();

    if (ToSubject.dot(getHeading()) > 0) 
    {
        return true;
    }
    return false;
}

//------------------------- IsThreatened ---------------------------------
//
//  returns true if there is an opponent within this player's 
//  comfort zone
//------------------------------------------------------------------------
bool PlayerBase::isThreatened()const
{
    //check against all opponents to make sure non are within this
    //player's comfort zone
    float zoneSq = Para_PlayerComfortZone*Para_PlayerComfortZone;
    std::vector<PlayerBase*>::const_iterator curOpp;  
    curOpp = getTeam()->getOpponents()->getMembers().begin();

    for (curOpp; curOpp != getTeam()->getOpponents()->getMembers().end(); ++curOpp)
    {
        //calculate distance to the player. if dist is less than our
        //comfort zone, and the opponent is infront of the player, return true
        if (positionInFrontOfPlayer((*curOpp)->getPos()) &&(Vec2DistanceSq(getPos(), 
                (*curOpp)->getPos()) < zoneSq))
        {        
            return true;
        }
    }// next opp

    return false;
}

//----------------------------- FindSupport -----------------------------------
//
//  determines the player who is closest to the SupportSpot and messages him
//  to tell him to change state to SupportAttacker
//-----------------------------------------------------------------------------
void PlayerBase::findSupport()const
{  
    PlayerBase* BestSupportPly = getTeam()->determineBestSupportingAttacker();

    //if there is no support we need to find a suitable player.
    if (getTeam()->getSupportingPlayer() == NULL)
    {
        

        getTeam()->setSupportingPlayer(BestSupportPly);

        MessageDispatcher::instance()->dispatchMsg( 0, 
                                                                            getID(), 
                                                                            getTeam()->getSupportingPlayer()->getID(),
                                                                            Msg_SupportAttacker, 
                                                                            NULL);
    }
    
    //if the best player available to support the attacker changes, update
    //the pointers and send messages to the relevant players to update their states
    if (BestSupportPly && (BestSupportPly != getTeam()->getSupportingPlayer()))
    {
        if (getTeam()->getSupportingPlayer())
        {
            MessageDispatcher::instance()->dispatchMsg( 0, 
                                                                                getID(), 
                                                                                getTeam()->getSupportingPlayer()->getID(),
                                                                                Msg_GoHome, 
                                                                                NULL);
        }

        getTeam()->setSupportingPlayer(BestSupportPly);
        MessageDispatcher::instance()->dispatchMsg( 0, 
                                                                            getID(), 
                                                                            getTeam()->getSupportingPlayer()->getID(),
                                                                            Msg_SupportAttacker, 
                                                                            NULL);
    }
}


//calculate distance to opponent's goal. Used frequently by the passing//methods
float PlayerBase::getDistToOppGoal()const
{
    return fabs(getPos().x - getTeam()->getOpponentsGoal()->center().x);
}

float PlayerBase::getDistToHomeGoal()const
{
    return fabs(getPos().x - getTeam()->getHomeGoal()->center().x);
}

bool PlayerBase::isControllingPlayer()const
{
    return getTeam()->getControllingPlayer()==this;
}

bool PlayerBase::isBallWithinKeeperRange()const
{
    return (Vec2DistanceSq(getPos(), getBall()->getPos()) < Para_KeeperInBallRange*Para_KeeperInBallRange);
}

bool PlayerBase::isBallWithinReceivingRange()const
{
    return (Vec2DistanceSq(getPos(), getBall()->getPos()) < 
                        Para_BallWithinReceivingRange*Para_BallWithinReceivingRange);
}

bool PlayerBase::isBallWithinKickingRange()const
{
    return (Vec2DistanceSq(getBall()->getPos(), getPos()) < Para_PlayerKickingDistance*Para_PlayerKickingDistance);
}

bool PlayerBase::isInHomeRegion()const
{
    if (m_PlayerRole == goal_keeper)
    {
        return getPitch()->getRegionFromIndex(m_homeRegion)->isInside(getPos(), Region::normal);
    }
    else
    {
        return getPitch()->getRegionFromIndex(m_homeRegion)->isInside(getPos(), Region::halfsize);
    }
}

bool PlayerBase::isAtTarget()const
{
    return (Vec2DistanceSq(getPos(), getSteering()->getTarget()) < Para_PlayerInTargetRange*Para_PlayerInTargetRange);
}

bool PlayerBase::isClosestTeamMemberToBall()const
{
    return getTeam()->getPlayerClosestToBall() == this;
}

bool PlayerBase::isClosestPlayerOnPitchToBall()const
{
    return isClosestTeamMemberToBall() && 
                    (getDistSqToBall() < getTeam()->getOpponents()->getClosestDistToBallSq());
}

bool PlayerBase::isInHotRegion()const
{
    return fabs(getPos().y - getTeam()->getOpponentsGoal()->center().y ) <
                    getPitch()->getPlayingArea()->length()/3.0;
}

bool PlayerBase::isAheadOfAttacker()const
{
    return fabs(getPos().x - getTeam()->getOpponentsGoal()->center().x) <
           fabs(getTeam()->getControllingPlayer()->getPos().x - getTeam()->getOpponentsGoal()->center().x);
}

SoccerBall* const PlayerBase::getBall()const
{
    return getTeam()->getPitch()->getBall();
}

SoccerPitch* const PlayerBase::getPitch()const
{
    return getTeam()->getPitch();
}

const Region* const PlayerBase::getHomeRegion()const
{
    return getPitch()->getRegionFromIndex(m_homeRegion);
}


