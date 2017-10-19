
#include "GameConfig.h"
#include "ParaConfigSoccer.h"
#include "SoccerPitch.h"
#include "SoccerBall.h"
#include "SoccerGoal.h"
#include "SoccerTeam.h"
#include "PlayerBase.h"
#include "TeamStates.h"
#include "common/game/Region.h"
#include "common/game/EntityManager.h"
#include "common/2D/Transformations.h"
#include "common/2D/Geometry.h"
#include "common/misc/LogDebug.h"



const int NumRegionsHorizontal = 6; 
const int NumRegionsVertical   = 3; 


SoccerPitch::SoccerPitch(int cx, int cy):m_cxClient(cx),
                                                                 m_cyClient(cy),
                                                                 m_bPaused(false),
                                                                 m_bGoalKeeperHasBall(false),
                                                                 m_Regions(NumRegionsHorizontal*NumRegionsVertical),
                                                                 m_bGameOn(true),
                                                                 m_ui(nullptr)
{
    AILOG("SoccerPitch");
    
    //define the playing area
    m_pPlayingArea = new Region(20, 20, cx-20, cy-20);

    //create the regions  
    createRegions(getPlayingArea()->width() / (float)NumRegionsHorizontal,
                          getPlayingArea()->height() / (float)NumRegionsVertical);

    //create the goals
    m_pRedGoal  = new SoccerGoal(Vector2D( m_pPlayingArea->left(), (cy-Para_GoalWidth)/2),
                                              Vector2D(m_pPlayingArea->left(), cy - (cy-Para_GoalWidth)/2),
                                              Vector2D(1,0));
    
    m_pBlueGoal = new SoccerGoal( Vector2D( m_pPlayingArea->right(), (cy-Para_GoalWidth)/2),
                                              Vector2D(m_pPlayingArea->right(), cy - (cy-Para_GoalWidth)/2),
                                              Vector2D(-1,0));
  
    //create and draw walls
    Vector2D TopLeft(m_pPlayingArea->left(), m_pPlayingArea->top());                                        
    Vector2D TopRight(m_pPlayingArea->right(), m_pPlayingArea->top());
    Vector2D BottomRight(m_pPlayingArea->right(), m_pPlayingArea->bottom());
    Vector2D BottomLeft(m_pPlayingArea->left(), m_pPlayingArea->bottom());

    Vector2D points[] = {Vector2D(m_pPlayingArea->left(), m_pPlayingArea->top()),        //top left
                                     Vector2D(m_pPlayingArea->right(), m_pPlayingArea->top()),      //top right 
                                     Vector2D(m_pPlayingArea->right(), m_pPlayingArea->bottom()),   //bottom right 
                                     Vector2D(m_pPlayingArea->left(), m_pPlayingArea->bottom())};   //bottom left
    for (int i=0; i<4; i++)
    {
        int j = i+1 < 4 ? i+1 : 0;
        Wall *w = new Wall(points[i], points[j]);
        m_vecWalls.push_back(w);
    }
    
    //create the soccer ball
    m_pBall = new SoccerBall(Vector2D((float)m_cxClient/2.0, (float)m_cyClient/2.0),
                                           Para_BallMass,
                                           m_vecWalls);

    //create and draw the teams 
    m_pRedTeam  = new SoccerTeam(m_pRedGoal, m_pBlueGoal, this, SoccerTeam::red);
    m_pBlueTeam = new SoccerTeam(m_pBlueGoal, m_pRedGoal, this, SoccerTeam::blue);

    //make sure each team knows who their opponents are
    m_pRedTeam->setOpponents(m_pBlueTeam);
    m_pBlueTeam->setOpponents(m_pRedTeam);  
}

//-------------------------------- dtor ----------------------------------
//------------------------------------------------------------------------
SoccerPitch::~SoccerPitch()
{
    delete m_pPlayingArea;
    
    delete m_pRedGoal;
    delete m_pBlueGoal;

    for (auto it=m_vecWalls.begin(); it != m_vecWalls.end(); ++it)
    {
        delete *it;
    }
    
    delete m_pBall;
    
    delete m_pRedTeam;
    delete m_pBlueTeam;
    
    for (unsigned int i=0; i<m_Regions.size(); ++i)
    {
        delete m_Regions[i];
    }

    if(m_ui)
    {
        removeChild(m_ui);
    }
}

SoccerPitch* SoccerPitch::create(int width, int height)
{
    SoccerPitch *ret = new (std::nothrow)SoccerPitch(width, height);
    return ret;
}

void SoccerPitch::onEnter()
{
    BaseNode::onEnter();
    enableScheduleUpdate(this);
}

//----------------------------- Update -----------------------------------
//
//  this demo works on a fixed frame rate (60 by default) so we don't need
//  to pass a time_elapsed as a parameter to the game entities
//------------------------------------------------------------------------
void SoccerPitch::update(float dt)
{
    if (m_bPaused) return;

    m_pRedGoal->update();
    m_pBlueGoal->update();

    for (unsigned int w=0; w<m_vecWalls.size(); ++w)
    {
        m_vecWalls[w]->update();
    }

#if 1
    //update the balls
    m_pBall->update(dt);

    //update the teams
    m_pRedTeam->update(dt);
    m_pBlueTeam->update(dt);

    //if a goal has been detected reset the pitch ready for kickoff
    if (m_pBlueGoal->isScored(m_pBall) || m_pRedGoal->isScored(m_pBall))
    {
        m_bGameOn = false;

        //reset the ball                                                      
        m_pBall->placeAtPosition(Vector2D((float)m_cxClient/2.0, (float)m_cyClient/2.0));

        //get the teams ready for kickoff
        m_pRedTeam->getFSM()->changeState(PrepareForKickOff::instance());
        m_pBlueTeam->getFSM()->changeState(PrepareForKickOff::instance());
    }
#endif

    render();
}

//------------------------- CreateRegions --------------------------------
void SoccerPitch::createRegions(float width, float height)
{
    //index into the vector
    int idx = m_Regions.size()-1;

    for (int col=0; col<NumRegionsHorizontal; ++col)
    {
        for (int row=0; row<NumRegionsVertical; ++row)
        {
            m_Regions[idx--] = new Region(getPlayingArea()->left()+col*width,
                                                               getPlayingArea()->top()+row*height,
                                                               getPlayingArea()->left()+(col+1)*width,
                                                               getPlayingArea()->top()+(row+1)*height,
                                                               idx);
        }
    }
}

void SoccerPitch::render()
{
    if(!m_ui)
    {
        m_ui = getNewNode();
        addChildToUI(m_ui);

        Vector2D points[] = {Vector2D(m_pPlayingArea->left(), m_pPlayingArea->top()),        //top left
                                         Vector2D(m_pPlayingArea->right(), m_pPlayingArea->top()),      //top right 
                                         Vector2D(m_pPlayingArea->right(), m_pPlayingArea->bottom()),   //bottom right 
                                         Vector2D(m_pPlayingArea->left(), m_pPlayingArea->bottom())};   //bottom left        
        //create and draw center circle
        Vector2D centerTop((points[0]+points[1])/2);
        Vector2D centerBot((points[2]+points[3])/2);
        drawLine(m_ui, centerTop, centerBot);
        drawCircle(m_ui, (centerTop+centerBot)/2, 120);
    }
}
