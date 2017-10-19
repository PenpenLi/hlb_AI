#include "ParaConfigSoccer.h"
#include "GoalKeeperStates.h"
#include "SoccerPitch.h"
#include "PlayerBase.h"
#include "GoalKeeper.h"
#include "SteeringBehaviors_Soccer.h"
#include "SoccerTeam.h"
#include "SoccerGoal.h"
#include "common/2D/Geometry.h"
#include "FieldPlayer.h"
#include "common/message/Telegram.h"
#include "common/message/MessageDispatcher.h"
#include "SoccerMessages.h"


//--------------------------- GlobalKeeperState -------------------------------
//-----------------------------------------------------------------------------

GlobalKeeperState* GlobalKeeperState::instance()
{
    static GlobalKeeperState instance;

    return &instance;
}


bool GlobalKeeperState::onMessage(GoalKeeper* keeper, const Telegram& telegram)
{
    switch(telegram.m_msgId)
    {
        case Msg_GoHome:
        {
            keeper->setDefaultHomeRegion();

            keeper->getFSM()->changeState(ReturnHome::instance());
        }
        break;

        case Msg_ReceiveBall:
        {
            keeper->getFSM()->changeState(InterceptBall::instance());
        }
        break;

    }//end switch

    return false;
}


//--------------------------- TendGoal -----------------------------------
//
//  This is the main state for the goalkeeper. When in this state he will
//  move left to right across the goalmouth using the 'interpose' steering
//  behavior to put himself between the ball and the back of the net.
//
//  If the ball comes within the 'goalkeeper range' he moves out of the
//  goalmouth to attempt to intercept it. (see next state)
//------------------------------------------------------------------------

TendGoal* TendGoal::instance()
{
    static TendGoal instance;

    return &instance;
}

void TendGoal::onEnter(GoalKeeper* keeper)
{
    //turn interpose on
    keeper->getSteering()->interposeOn(Para_GoalKeeperTendingDistance);

    //interpose will position the agent between the ball position and a target
    //position situated along the goal mouth. This call sets the target
    keeper->getSteering()->setTarget(keeper->getRearInterposeTarget());
}

void TendGoal::onExecute(GoalKeeper* keeper)
{
    //the rear interpose target will change as the ball's position changes
    //so it must be updated each update-step 
    keeper->getSteering()->setTarget(keeper->getRearInterposeTarget());

    //if the ball comes in range the keeper traps it and then changes state
    //to put the ball back in play
    if (keeper->isBallWithinKeeperRange())
    {
        keeper->getBall()->trap();

        keeper->getPitch()->setGoalKeeperHasBall(true);

        keeper->getFSM()->changeState(PutBallBackInPlay::instance());

        return;
    }

    //if ball is within a predefined distance, the keeper moves out from
    //position to try and intercept it.
    if (keeper->isBallWithinRangeForIntercept() && !keeper->getTeam()->isInControl())
    {
        keeper->getFSM()->changeState(InterceptBall::instance());
    }

    //if the keeper has ventured too far away from the goal-line and there
    //is no threat from the opponents he should move back towards it
    if (keeper->isTooFarFromGoalMouth() && keeper->getTeam()->isInControl())
    {
        keeper->getFSM()->changeState(ReturnHome::instance());

        return;
    }
}


void TendGoal::onExit(GoalKeeper* keeper)
{
    keeper->getSteering()->interposeOff();
}


//------------------------- ReturnHome: ----------------------------------
//
//  In this state the goalkeeper simply returns back to the center of
//  the goal region before changing state back to TendGoal
//------------------------------------------------------------------------

ReturnHome* ReturnHome::instance()
{
    static ReturnHome instance;

    return &instance;
}


void ReturnHome::onEnter(GoalKeeper* keeper)
{
    keeper->getSteering()->arriveOn();
}

void ReturnHome::onExecute(GoalKeeper* keeper)
{
    keeper->getSteering()->setTarget(keeper->getHomeRegion()->center());

    //if close enough to home or the opponents get control over the ball,
    //change state to tend goal
    if (keeper->isInHomeRegion() || !keeper->getTeam()->isInControl())
    {
        keeper->getFSM()->changeState(TendGoal::instance());
    }
}

void ReturnHome::onExit(GoalKeeper* keeper)
{
    keeper->getSteering()->arriveOff();
}



//----------------- InterceptBall ----------------------------------------
//
//  In this state the GP will attempt to intercept the ball using the
//  pursuit steering behavior, but he only does so so long as he remains
//  within his home region.
//------------------------------------------------------------------------

InterceptBall* InterceptBall::instance()
{
    static InterceptBall instance;

    return &instance;
}


void InterceptBall::onEnter(GoalKeeper* keeper)
{
    keeper->getSteering()->pursuitOn();  
}

void InterceptBall::onExecute(GoalKeeper* keeper)
{ 
    //if the goalkeeper moves to far away from the goal he should return to his
    //home region UNLESS he is the closest player to the ball, in which case,
    //he should keep trying to intercept it.
    if (keeper->isTooFarFromGoalMouth() && !keeper->isClosestPlayerOnPitchToBall())
    {
        keeper->getFSM()->changeState(ReturnHome::instance());

        return;
    }
  
    //if the ball becomes in range of the goalkeeper's hands he traps the 
    //ball and puts it back in play
    if (keeper->isBallWithinKeeperRange())
    {
        keeper->getBall()->trap();

        keeper->getPitch()->setGoalKeeperHasBall(true);

        keeper->getFSM()->changeState(PutBallBackInPlay::instance());

        return;
    }
}

void InterceptBall::onExit(GoalKeeper* keeper)
{
    keeper->getSteering()->pursuitOff();
}



//--------------------------- PutBallBackInPlay --------------------------
//
//------------------------------------------------------------------------

PutBallBackInPlay* PutBallBackInPlay::instance()
{
    static PutBallBackInPlay instance;

    return &instance;
}

void PutBallBackInPlay::onEnter(GoalKeeper* keeper)
{
    //let the team know that the keeper is in control
    keeper->getTeam()->setControllingPlayer(keeper);

    //send all the players home
    keeper->getTeam()->getOpponents()->returnAllFieldPlayersToHome();
    keeper->getTeam()->returnAllFieldPlayersToHome();
}


void PutBallBackInPlay::onExecute(GoalKeeper* keeper)
{
    PlayerBase* receiver = NULL;
    Vector2D BallTarget;
    
    //test if there are players further forward on the field we might
    //be able to pass to. If so, make a pass.
    if (keeper->getTeam()->findPass(keeper,
                                                          receiver,
                                                          BallTarget,
                                                          Para_MaxPassingForce,
                                                          Para_GoalkeeperMinPassDistance))
    {     
        //make the pass   
        keeper->getBall()->kick(Vec2Normalize(BallTarget - keeper->getBall()->getPos()),
                                                Para_MaxPassingForce);

        //goalkeeper no longer has ball 
        keeper->getPitch()->setGoalKeeperHasBall(false);


        //let the receiving player know the ball's comin' at him
        MessageDispatcher::instance()->dispatchMsg( 0, 
                                                                            keeper->getID(), 
                                                                            receiver->getID(),
                                                                            Msg_ReceiveBall, 
                                                                            &BallTarget);
            
        //go back to tending the goal   
        keeper->getFSM()->changeState(TendGoal::instance());

        return;
    }  

    keeper->setSpeed(Vector2D());
}

