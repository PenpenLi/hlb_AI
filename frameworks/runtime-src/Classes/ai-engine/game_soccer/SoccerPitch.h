#ifndef SOCCERPITCH_H
#define SOCCERPITCH_H
#pragma warning (disable:4786)
//------------------------------------------------------------------------
//
//  Name:   SoccerPitch.h
//
//  Desc:   A SoccerPitch is the main game object. It owns instances of
//          two soccer teams, two goals, the playing area, the ball
//          etc. This is the root class for all the game updates and
//          renders etc
//
//
//------------------------------------------------------------------------
#include <vector>
#include <cassert>

#include "common/game/Wall.h"
#include "common/2D/Vector2D.h"
#include "common/game/BaseNode.h"

class Region;
class SoccerGoal;
class SoccerTeam;
class SoccerBall;
class SoccerTeam;
class PlayerBase;


class SoccerPitch :public BaseNode
{ 
public:
    SoccerPitch(int cxClient, int cyClient);
    ~SoccerPitch();

    static SoccerPitch* create(int width, int height);

    void onEnter();
    
    void update(float dt);

    bool isPaused()const {return m_bPaused;}

    int cxClient()const {return m_cxClient;}
    int cyClient()const {return m_cyClient;}

    bool isGoalKeeperHasBall()const {return m_bGoalKeeperHasBall;}
    void setGoalKeeperHasBall(bool b) {m_bGoalKeeperHasBall = b;}

    const Region*const getPlayingArea()const{return m_pPlayingArea;}
    const std::vector<Wall*>& getWalls(){return m_vecWalls;}                      
    SoccerBall*const getBall()const{return m_pBall;}

    const Region* const getRegionFromIndex(int idx)                                
    {
        assert ( (idx >= 0) && (idx < (int)m_Regions.size()) );

        return m_Regions[idx];
    }
    
    bool  isGameOn()const{return m_bGameOn;}
    void  setGameOn(){m_bGameOn = true;}
    void  setGameOff(){m_bGameOn = false;}


private:
    SoccerBall* m_pBall;
    
    SoccerTeam* m_pRedTeam;
    SoccerTeam* m_pBlueTeam;

    SoccerGoal* m_pRedGoal;
    SoccerGoal* m_pBlueGoal;
   
    //container for the boundary walls
    std::vector<Wall*> m_vecWalls;

    //defines the dimensions of the playing area
    Region* m_pPlayingArea;

    //the playing field is broken up into regions that the team
    //can make use of to implement strategies.
    std::vector<Region*> m_Regions;

    //true if a goal keeper has possession
    bool m_bGoalKeeperHasBall;

    //true if the game is in play. Set to false whenever the players
    //are getting ready for kickoff
    bool m_bGameOn;

    //set true to pause the motion
    bool m_bPaused;

    //local copy of client window dimensions
    int m_cxClient;
    int m_cyClient;  

    pNode m_ui;
    
    //this instantiates the regions the players utilize to  position themselves
    void createRegions(float width, float height);

    void render();
};

#endif