
#include "ParaConfigSoccer.h"
#include "FieldPlayerStates.h"
#include "SoccerPitch.h"
#include "FieldPlayer.h"
#include "SteeringBehaviors_Soccer.h"
#include "SoccerTeam.h"
#include "SoccerGoal.h"
#include "SoccerBall.h"

#include "common/misc/Regulator.h"
#include "common/misc/LogDebug.h"
#include "common/message/Telegram.h"
#include "common/message/MessageDispatcher.h"
#include "SoccerMessages.h"
#include "ParaConfigSoccer.h"


//************************************************************************ Global state

GlobalPlayerState* GlobalPlayerState::instance()
{
    static GlobalPlayerState instance;

    return &instance;
}


void GlobalPlayerState::onExecute(FieldPlayer* player)                                     
{
    //if a player is in possession and close to the ball reduce his max speed
    if((player->isBallWithinReceivingRange()) && (player->isControllingPlayer()))
    {
        player->setMaxSpeed(Para_PlayerMaxSpeedWithBall);
    }
    else
    {
        player->setMaxSpeed(Para_PlayerMaxSpeedWithBall);
    }
}


bool GlobalPlayerState::onMessage(FieldPlayer* player, const Telegram& telegram)
{
    switch(telegram.m_msgId)
    {
        case Msg_ReceiveBall:
        {
            //set the target
            player->getSteering()->setTarget(*(static_cast<Vector2D*>(telegram.m_extraInfo)));

            //change state 
            player->getFSM()->changeState(ReceiveBall::instance());

            return true;
        }

        break;

        case Msg_SupportAttacker:
        {
            //if already supporting just return
            if (player->getFSM()->isInState(*SupportAttacker::instance()))
            {
                return true;
            }

            //set the target to be the best supporting position
            player->getSteering()->setTarget(player->getTeam()->getSupportSpot());

            //change the state
            player->getFSM()->changeState(SupportAttacker::instance());

            return true;
        }

        break;

        case Msg_Wait:
        {
            //change the state
            player->getFSM()->changeState(Wait::instance());

            return true;
        }

        break;

        case Msg_GoHome:
        {
            player->setDefaultHomeRegion();

            player->getFSM()->changeState(ReturnToHomeRegion::instance());

            return true;
        }

        break;

        case Msg_PassToMe:
        {  

            //get the position of the player requesting the pass 
            FieldPlayer* receiver = static_cast<FieldPlayer*>(telegram.m_extraInfo);

            AILOG("Player %d received request from %d to make pass", player->getID(), receiver->getID());

            //if the ball is not within kicking range or their is already a 
            //receiving player, this player cannot pass the ball to the player
            //making the request.
            if (player->getTeam()->getReceiver() != NULL ||!player->isBallWithinKickingRange() )
            {
                return true;
            }

            //make the pass   
            player->getBall()->kick(receiver->getPos() - player->getBall()->getPos(), Para_MaxPassingForce);

            AILOG("Player %d  Passed ball to requesting player", player->getID());
            
            //let the receiver know a pass is coming 
            MessageDispatcher::instance()->dispatchMsg( 0, 
                                                                                player->getID(), 
                                                                                receiver->getID(),
                                                                                Msg_ReceiveBall, 
                                                                                &receiver->getPos());
            
            //change state   
            player->getFSM()->changeState(Wait::instance());

            player->findSupport();

            return true;
        }
        break;

    }//end switch

    return false;
}
                                

       

//***************************************************************************** CHASEBALL

ChaseBall* ChaseBall::instance()
{
    static ChaseBall instance;

    return &instance;
}


void ChaseBall::onEnter(FieldPlayer* player)
{
    AILOG("ChaseBall::onEnter: ID=%d", player->getID());
    player->getSteering()->seekOn();
}

void ChaseBall::onExecute(FieldPlayer* player)                                     
{
    //if the ball is within kicking range the player changes state to KickBall.
    if (player->isBallWithinKickingRange())
    {
        player->getFSM()->changeState(KickBall::instance());
        return;
    }
                                                                              
    //if the player is the closest player to the ball then he should keep
    //chasing it
    if (player->isClosestTeamMemberToBall())
    {
        player->getSteering()->setTarget(player->getBall()->getPos());
        return;
    }

    //if the player is not closest to the ball anymore, he should return back
    //to his home region and wait for another opportunity
    player->getFSM()->changeState(ReturnToHomeRegion::instance());
}


void ChaseBall::onExit(FieldPlayer* player)
{
    player->getSteering()->seekOff();
}


//*****************************************************************************SUPPORT ATTACKING PLAYER

SupportAttacker* SupportAttacker::instance()
{
    static SupportAttacker instance;

    return &instance;
}


void SupportAttacker::onEnter(FieldPlayer* player)
{
    player->getSteering()->arriveOn();

    player->getSteering()->setTarget(player->getTeam()->getSupportSpot());
}

void SupportAttacker::onExecute(FieldPlayer* player)                                     
{
    //if his team loses control go back home
    if (!player->getTeam()->isInControl())
    {
        player->getFSM()->changeState(ReturnToHomeRegion::instance()); return;
    } 


    //if the best supporting spot changes, change the steering target
    if (player->getTeam()->getSupportSpot() != player->getSteering()->getTarget())
    {    
        player->getSteering()->setTarget(player->getTeam()->getSupportSpot());

        player->getSteering()->arriveOn();
    }

    //if this player has a shot at the goal AND the attacker can pass
    //the ball to him the attacker should pass the ball to this player
    if( player->getTeam()->canShoot(player->getPos(), Para_MaxShootingForce))
    {
        player->getTeam()->requestPass(player);
    }


    //if this player is located at the support spot and his team still have
    //possession, he should remain still and turn to face the ball
    if (player->isAtTarget())
    {
        player->getSteering()->arriveOff();

        //the player should keep his eyes on the ball!
        player->trackBall();

        player->setSpeed(Vector2D(0,0));

        //if not threatened by another player request a pass
        if (!player->isThreatened())
        {
            player->getTeam()->requestPass(player);
        }
    }
}


void SupportAttacker::onExit(FieldPlayer* player)
{
    //set supporting player to null so that the team knows it has to 
    //determine a new one.
    player->getTeam()->setSupportingPlayer(NULL);

    player->getSteering()->arriveOff();
}




//************************************************************************ RETURN TO HOME REGION

ReturnToHomeRegion* ReturnToHomeRegion::instance()
{
    static ReturnToHomeRegion instance;

    return &instance;
}


void ReturnToHomeRegion::onEnter(FieldPlayer* player)
{
    player->getSteering()->arriveOn();

    if (!player->getHomeRegion()->isInside(player->getSteering()->getTarget(), Region::halfsize))
    {
        player->getSteering()->setTarget(player->getHomeRegion()->center());
    }
}

void ReturnToHomeRegion::onExecute(FieldPlayer* player)
{
    if (player->getPitch()->isGameOn())
    {
        //if the ball is nearer this player than any other team member  &&
        //there is not an assigned receiver && the goalkeeper does not gave
        //the ball, go chase it
        if ( player->isClosestTeamMemberToBall() &&
            (player->getTeam()->getReceiver() == NULL) &&
            !player->getPitch()->isGoalKeeperHasBall())
        {
            player->getFSM()->changeState(ChaseBall::instance());

            return;
        }
    }

    //if game is on and close enough to home, change state to wait and set the 
    //player target to his current position.(so that if he gets jostled out of 
    //position he can move back to it)
    if (player->getPitch()->isGameOn() && player->getHomeRegion()->isInside(player->getPos(),
                                                             Region::halfsize))
    {
        player->getSteering()->setTarget(player->getPos());
        player->getFSM()->changeState(Wait::instance());
    }
    //if game is not on the player must return much closer to the center of his home region
    else if(!player->getPitch()->isGameOn() && player->isAtTarget())
    {
        player->getFSM()->changeState(Wait::instance());
    }
}

void ReturnToHomeRegion::onExit(FieldPlayer* player)
{
    player->getSteering()->arriveOff();
}




//***************************************************************************** WAIT

Wait* Wait::instance()
{
    static Wait instance;

    return &instance;
}


void Wait::onEnter(FieldPlayer* player)
{
    //if the game is not on make sure the target is the center of the player's
    //home region. This is ensure all the players are in the correct positions
    //ready for kick off
    if (!player->getPitch()->isGameOn())
    {
        player->getSteering()->setTarget(player->getHomeRegion()->center());
    }
}

void Wait::onExecute(FieldPlayer* player)
{    
    //if the player has been jostled out of position, get back in position  
    if (!player->isAtTarget())
    {
        player->getSteering()->arriveOn();

        return;
    }
    else
    {
        player->getSteering()->arriveOff();

        player->setSpeed(Vector2D(0,0));

        //the player should keep his eyes on the ball!
        player->trackBall();
    }

    //if this player's team is controlling AND this player is not the attacker
    //AND is further up the field than the attacker he should request a pass.
    if ( player->getTeam()->isInControl() &&
            (!player->isControllingPlayer()) &&
            player->isAheadOfAttacker() )
    {
        player->getTeam()->requestPass(player);

        return;
    }

    if (player->getPitch()->isGameOn())
    {
        //if the ball is nearer this player than any other team member  AND
        //there is not an assigned receiver AND neither goalkeeper has
        //the ball, go chase it
        if (player->isClosestTeamMemberToBall() &&
            player->getTeam()->getReceiver() == NULL  &&
            !player->getPitch()->isGoalKeeperHasBall())
        {
            player->getFSM()->changeState(ChaseBall::instance());

            return;
        }
    } 
}

void Wait::onExit(FieldPlayer* player){}




//************************************************************************ KICK BALL

KickBall* KickBall::instance()
{
  static KickBall instance;

  return &instance;
}


void KickBall::onEnter(FieldPlayer* player)
{
    AILOG("KickBall::onEnter, ID=%d", player->getID());
    
    //let the team know this player is controlling
    player->getTeam()->setControllingPlayer(player);

    //the player can only make so many kick attempts per second.
    if (!player->isReadyForNextKick()) 
    {
        player->getFSM()->changeState(ChaseBall::instance());
    }
}

void KickBall::onExecute(FieldPlayer* player)
{ 
    AILOG("KickBall::onExecute, ID=%d", player->getID());
    //calculate the dot product of the vector pointing to the ball
    //and the player's heading
    Vector2D ToBall = player->getBall()->getPos() - player->getPos();
    float dot = player->getHeading().dot(Vec2Normalize(ToBall)); 

    //cannot kick the ball if the goalkeeper is in possession or if it is 
    //behind the player or if there is already an assigned receiver. So just
    //continue chasing the ball
    if (player->getTeam()->getReceiver() != NULL   ||
        player->getPitch()->isGoalKeeperHasBall() ||
        (dot < 0) ) 
    {
        AILOG("change to state of ChaseBall: rev=%x, dot=%f", player->getTeam()->getReceiver(), dot);
        player->getFSM()->changeState(ChaseBall::instance());
        return;
    }

    /* Attempt a shot at the goal */

    //if a shot is possible, this vector will hold the position along the 
    //opponent's goal line the player should aim for.
    Vector2D BallTarget;

    //the dot product is used to adjust the shooting force. The more
    //directly the ball is ahead, the more forceful the kick
    float power = Para_MaxShootingForce * dot;

    //if it is determined that the player could score a goal from this position
    //OR if he should just kick the ball anyway, the player will attempt
    //to make the shot
    if (player->getTeam()->canShoot(player->getBall()->getPos(), power,BallTarget) || 
            (RandFloat_0_1() < Para_ChancePlayerAttemptsPotShot))
    {
        //add some noise to the kick. We don't want players who are 
        //too accurate! The amount of noise can be adjusted by altering
        //Prm.PlayerKickingAccuracy
        BallTarget = addNoiseToKick(player->getBall()->getPos(), BallTarget);

        //this is the direction the ball will be kicked in
        Vector2D KickDirection = BallTarget - player->getBall()->getPos();

        player->getBall()->kick(KickDirection, power);
    
        //change state   
        player->getFSM()->changeState(Wait::instance());

        player->findSupport();
  
        return;
    }


    /* Attempt a pass to a player */

    //if a receiver is found this will point to it
    PlayerBase* receiver = NULL;

    power = Para_MaxPassingForce * dot;
  
    //test if there are any potential candidates available to receive a pass
    if (player->isThreatened()  && player->getTeam()->findPass( player,
                                                                                                  receiver,
                                                                                                  BallTarget,
                                                                                                  power,
                                                                                                  Para_MinPassDistance))
    {     
        AILOG("is threatened ,and pass ball...");
        //add some noise to the kick
        BallTarget = addNoiseToKick(player->getBall()->getPos(), BallTarget);

        Vector2D KickDirection = BallTarget - player->getBall()->getPos();

        player->getBall()->kick(KickDirection, power);


        //let the receiver know a pass is coming 
        MessageDispatcher::instance()->dispatchMsg( 0, 
                                                                            player->getID(), 
                                                                            receiver->getID(),
                                                                            Msg_ReceiveBall, 
                                                                            &BallTarget);

        //the player should wait at his current position unless instruced
        //otherwise  
        player->getFSM()->changeState(Wait::instance());

        player->findSupport();

        return;
    }

    //cannot shoot or pass, so dribble the ball upfield
    else
    {   
        player->findSupport();
        
        player->getFSM()->changeState(Dribble::instance());
    }   
}


//*************************************************************************** DRIBBLE

Dribble* Dribble::instance()
{
    static Dribble instance;

    return &instance;
}


void Dribble::onEnter(FieldPlayer* player)
{
    AILOG("Dribble::onEnter, ID=%d", player->getID());
    //let the team know this player is controlling
    player->getTeam()->setControllingPlayer(player);
}

void Dribble::onExecute(FieldPlayer* player)
{
    float dot = player->getTeam()->getHomeGoal()->getFacing().dot(player->getHeading());

    //if the ball is between the player and the home goal, it needs to swivel
    // the ball around by doing multiple small kicks and turns until the player 
    //is facing in the correct direction
    if (dot < 0)
    {
        //the player's heading is going to be rotated by a small amount (Pi/4) 
        //and then the ball will be kicked in that direction
        Vector2D direction = player->getHeading();

        //calculate the sign (+/-) of the angle between the player heading and the 
        //facing direction of the goal so that the player rotates around in the 
        //correct direction
        float angle = -_PI_/4 * player->getTeam()->getHomeGoal()->getFacing().sign(player->getHeading());

        Vec2DRotateAroundOrigin(direction, angle);

        //this value works well whjen the player is attempting to control the
        //ball and turn at the same time
        const float KickingForce = 0.8f;

        player->getBall()->kick(direction, KickingForce);
    }

    //kick the ball down the field
    else
    {
        player->getBall()->kick(player->getTeam()->getHomeGoal()->getFacing(), Para_MaxDribbleForce);  
    }

    //the player has kicked the ball so he must now change state to follow it
    player->getFSM()->changeState(ChaseBall::instance());

    return;  
}



//************************************************************************     RECEIVEBALL

ReceiveBall* ReceiveBall::instance()
{
    static ReceiveBall instance;

    return &instance;
}


void ReceiveBall::onEnter(FieldPlayer* player)
{
    //let the team know this player is receiving the ball
    player->getTeam()->setReceiver(player);

    //this player is also now the controlling player
    player->getTeam()->setControllingPlayer(player);

    //there are two types of receive behavior. One uses arrive to direct
    //the receiver to the position sent by the passer in its telegram. The
    //other uses the pursuit behavior to pursue the ball. 
    //This statement selects between them dependent on the probability
    //ChanceOfUsingArriveTypeReceiveBehavior, whether or not an opposing
    //player is close to the receiving player, and whether or not the receiving
    //player is in the opponents 'hot region' (the third of the pitch closest
    //to the opponent's goal
    const float PassThreatRadius = 70.0f;

    if (( player->isInHotRegion() ||RandFloat_0_1() < Para_ChanceOfUsingArriveTypeReceiveBehavior) &&
                !player->getTeam()->isOpponentWithinRadius(player->getPos(), PassThreatRadius))
    {
        player->getSteering()->arriveOn();
    }
    else
    {
        player->getSteering()->pursuitOn();
    }
}

void ReceiveBall::onExecute(FieldPlayer* player)
{
    //if the ball comes close enough to the player or if his team lose control
    //he should change state to chase the ball
    if (player->isBallWithinReceivingRange() || !player->getTeam()->isInControl())
    {
        player->getFSM()->changeState(ChaseBall::instance());

        return;
    }  

    if (player->getSteering()->isPursuitOn())
    {
        player->getSteering()->setTarget(player->getBall()->getPos());
    }

    //if the player has 'arrived' at the steering target he should wait and
    //turn to face the ball
    if (player->isAtTarget())
    {
        player->getSteering()->arriveOff();
        player->getSteering()->pursuitOff();
        player->trackBall();    
        player->setSpeed(Vector2D(0,0));
    } 
}

void ReceiveBall::onExit(FieldPlayer* player)
{
    player->getSteering()->arriveOff();
    player->getSteering()->pursuitOff();

    player->getTeam()->setReceiver(NULL);
}

