
#include "ParaConfigSoccer.h"
#include "FieldPlayer.h"
#include "SoccerTeam.h"
#include "SteeringBehaviors_Soccer.h"
#include "common/2D/Geometry.h"
#include "common/2D/Matrix2D.h"
#include "common/misc/UtilsEx.h"
#include "common/game/Region.h"
#include "common/game/CommonFunction.h"
#include "common/misc/LogDebug.h"
#include <limits>

#include "SoccerBall.h"


using std::vector;


//----------------------------- ctor -------------------------------------
//------------------------------------------------------------------------
FieldPlayer::FieldPlayer( SoccerTeam* home_team,
                                            int home_region,
                                            State<FieldPlayer>* start_state,
                                            Vector2D heading,
                                            Vector2D velocity,
                                            float mass,
                                            float max_force,
                                            float max_speed,
                                            float max_turn_rate,
                                            player_role role):PlayerBase(home_team,
                                                                                    home_region,
                                                                                    heading,
                                                                                    velocity,
                                                                                    mass,
                                                                                    max_force,
                                                                                    max_speed,
                                                                                    max_turn_rate,
                                                                                    role),
                                                                                    m_ui(nullptr)
{
    //set up the state machine
    m_pStateMachine =  new StateMachine<FieldPlayer>(this);

    if (start_state)
    {    
        m_pStateMachine->setCurrentState(start_state);
        m_pStateMachine->setPreviousState(start_state);
        m_pStateMachine->setGlobalState(GlobalPlayerState::instance());

        m_pStateMachine->getCurrentState()->onEnter(this);
    } 
    
    m_pSteering->separationOn();
    
    //set up the kick regulator
    m_pKickLimiter = new Regulator(Para_PlayerKickFrequency);

}


FieldPlayer::~FieldPlayer()
{
    delete m_pKickLimiter;
    delete m_pStateMachine;

    if (m_ui)
    {
        removeChild(m_ui);
    }
}


void FieldPlayer::update(float dt) 
{ 
    //run the logic for the current state
    m_pStateMachine->update(dt);

    //calculate the combined steering force
    m_pSteering->calculate();

    //if no steering force is produced decelerate the player by applying a
    //braking force
    if (m_pSteering->getForce().isZero())
    {
        const float BrakingRate = 0.8f; 
        m_vVelocity = m_vVelocity * BrakingRate; 
    }
  
    //the steering force's side component is a force that rotates the 
    //player about its axis. We must limit the rotation so that a player
    //can only turn by PlayerMaxTurnRate rads per update.
    float TurningForce = m_pSteering->sideComponent();

    clamp(TurningForce, -Para_PlayerMaxTurnRate, Para_PlayerMaxTurnRate);

    //rotate the heading vector
    Vec2DRotateAroundOrigin(m_vHeading, TurningForce);

    //make sure the velocity vector points in the same direction as
    //the heading vector
    m_vVelocity = m_vHeading * m_vVelocity.length();

    //and recreate m_vSide
    m_vSide = m_vHeading.getPerp();


    //now to calculate the acceleration due to the force exerted by
    //the forward component of the steering force in the direction
    //of the player's heading
    Vector2D accel = m_vHeading * m_pSteering->forwardComponent() / m_dMass;

    m_vVelocity += accel;

    //make sure player does not exceed maximum velocity
    m_vVelocity.truncate(m_dMaxSpeed);

    //update the position
    m_vPosition += m_vVelocity;
    
    //enforce a non-penetration constraint if desired
    if(Invoid_Not_Overlap) 
    {
        invoidOverlap(this, AutoList<PlayerBase>::getAllMembers());
    }

    //update UI
    render();
    
}

//-------------------- handleMessage -------------------------------------
//  routes any messages appropriately
//------------------------------------------------------------------------
bool FieldPlayer::handleMessage(const Telegram& msg)
{
    return m_pStateMachine->handleMessage(msg);
}

void FieldPlayer::updatePortraitByHeading(Vector2D vHeading)
{
    //find the closest direction
    Vector2D cos_direction[move_max]={ Vector2D(0, 1), Vector2D(0, -1),
                                                                Vector2D(-1, 0),Vector2D(1, 0)};
    float dot_max = -1.0f;
    move_direction idx = move_right;
    float tmp;
    for (int i=0; i<move_max; i++)
    {
        tmp = cos_direction[i].dot(vHeading);
        if (tmp > dot_max)
        {
            dot_max = tmp;
            idx = (move_direction)i;
        }
    }
    //get the rec of texture
    Vector2D leftdown = Vector2D(0, 0);
    float iconSize = 85.0f;
    float x_offset = getTeam()->getTeamColor() == SoccerTeam::red ? 0.0f : iconSize;
    if (idx == move_up)
    {
        leftdown = Vector2D(x_offset, 3*iconSize);
    }
    else if (idx == move_down)
    {
        leftdown = Vector2D(x_offset, 0);
    }
    else if (idx == move_left)
    {
        leftdown = Vector2D(x_offset, iconSize);
    }
    else if (idx == move_right)
    {
        leftdown = Vector2D(x_offset, 2*iconSize);
    }    

    
    if (m_ui)
    {
        auto img = loadImgToNode(m_ui, "res/game_soccer/player.png", 1234, leftdown, Vector2D(leftdown.x+iconSize, leftdown.y+iconSize));
        setNodeAnchorPoint(img, Vector2D(0.5f, 0.15f));
        setSize(getNodeSize(img));

        std::string str = ttos(this->getID()) + "  " + m_pStateMachine->getNameOfCurrentState();
        showString(m_ui, 1235, str, Vector2D(0, getSize().y+5));
    }
    
}


void FieldPlayer::render()
{
    if (!m_ui) //update UI
    {
        //for ui
        m_ui = (pNode)getNewNode();
        addChildToUI(m_ui);
    } 
    updatePortraitByHeading(m_vHeading);    
    setNodePos(m_ui, m_vPosition);
}
