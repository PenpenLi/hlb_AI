
#include "TeamStates.h"
#include "SoccerTeam.h"
#include "PlayerBase.h"
#include "common/message/MessageDispatcher.h"
#include "SoccerMessages.h"
#include "SoccerPitch.h"
#include "ParaConfigSoccer.h"
#include "common/misc/LogDebug.h"


void changePlayerHomeRegions(SoccerTeam* team, const int NewRegions[Para_TeamSize])
{
    for (int plyr=0; plyr<Para_TeamSize; ++plyr)
    {
        team->setPlayerHomeRegion(plyr, NewRegions[plyr]);
    }
}

//************************************************************************ ATTACKING

Attacking* Attacking::instance()
{
    static Attacking instance;

    return &instance;
}


void Attacking::onEnter(SoccerTeam* team)
{
    AILOG("Attacking::onEnter, color=%d", team->getTeamColor());
    
    //these define the home regions for this state of each of the players
    const int BlueRegions[Para_TeamSize] = {1,12,14,6,4};
    const int RedRegions[Para_TeamSize] = {16,3,5,9,13};

    //set up the player's home regions
    if (team->getTeamColor() == SoccerTeam::blue)
    {
        changePlayerHomeRegions(team, BlueRegions);
    }
    else
    {
        changePlayerHomeRegions(team, RedRegions);
    }

    //if a player is in either the Wait or ReturnToHomeRegion states, its
    //steering target must be updated to that of its new home region to enable
    //it to move into the correct position.
    team->updateTargetsOfWaitingPlayers();
}


void Attacking::onExecute(SoccerTeam* team)
{
    //if this team is no longer in control change states
    if (!team->isInControl())
    {
        team->getFSM()->changeState(Defending::instance()); 
        return;
    }

    //calculate the best position for any supporting attacker to move to
    team->determineBestSupportingPosition();
}

void Attacking::onExit(SoccerTeam* team)
{
    //there is no supporting player for defense
    team->setSupportingPlayer(NULL);
}



//************************************************************************ DEFENDING

Defending* Defending::instance()
{
    static Defending instance;

    return &instance;
}

void Defending::onEnter(SoccerTeam* team)
{
    AILOG("Defending::onEnter");

    //these define the home regions for this state of each of the players
    const int BlueRegions[Para_TeamSize] = {1,6,8,3,5};
    const int RedRegions[Para_TeamSize] = {16,9,11,12,14};

    //set up the player's home regions
    if (team->getTeamColor() == SoccerTeam::blue)
    {
        changePlayerHomeRegions(team, BlueRegions);
    }
    else
    {
        changePlayerHomeRegions(team, RedRegions);
    }
  
    //if a player is in either the Wait or ReturnToHomeRegion states, its
    //steering target must be updated to that of its new home region
    team->updateTargetsOfWaitingPlayers();
}

void Defending::onExecute(SoccerTeam* team)
{
    //if in control change states
    if (team->isInControl())
    {
        team->getFSM()->changeState(Attacking::instance()); 
        return;
    }
}


void Defending::onExit(SoccerTeam* team){}


//************************************************************************ KICKOFF
PrepareForKickOff* PrepareForKickOff::instance()
{
    static PrepareForKickOff instance;

    return &instance;
}

void PrepareForKickOff::onEnter(SoccerTeam* team)
{
    AILOG("PrepareForKickOff::onEnter");
    
    //reset key player pointers
    team->setControllingPlayer(NULL);
    team->setSupportingPlayer(NULL);
    team->setReceiver(NULL);
    team->setPlayerClosestToBall(NULL);

    //send Msg_GoHome to each player.
    team->returnAllFieldPlayersToHome();
}

void PrepareForKickOff::onExecute(SoccerTeam* team)
{
    //if both teams in position, start the game
    if (team->isAllPlayersAtHome() && team->getOpponents()->isAllPlayersAtHome())
    {
        team->getFSM()->changeState(Defending::instance());
    }
}

void PrepareForKickOff::onExit(SoccerTeam* team)
{
    team->getPitch()->setGameOn();
}


