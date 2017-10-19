
#include "ParaConfigSoccer.h"
#include "ParaConfigSoccer.h"
#include "GoalKeeper.h"
#include "SteeringBehaviors_Soccer.h"
#include "SoccerTeam.h"
#include "SoccerPitch.h"
#include "common/game/CommonFunction.h"
#include "GoalKeeperStates.h"
#include "SoccerGoal.h"




//----------------------------- ctor ------------------------------------
//-----------------------------------------------------------------------
GoalKeeper::GoalKeeper(SoccerTeam* home_team,
                                            int home_region,
                                            State<GoalKeeper>* start_state,
                                            Vector2D heading,
                                            Vector2D velocity,
                                            float mass,
                                            float max_force,
                                            float max_speed,
                                            float max_turn_rate): PlayerBase(home_team,
                                                                                                 home_region,
                                                                                                 heading,
                                                                                                 velocity,
                                                                                                 mass,
                                                                                                 max_force,
                                                                                                 max_speed,
                                                                                                 max_turn_rate,
                                                                                                 PlayerBase::goal_keeper),
                                                                                                 m_ui(nullptr)
                                         
                                        
{  

    
    //set up the state machine
    m_pStateMachine = new StateMachine<GoalKeeper>(this);

    m_pStateMachine->setCurrentState(start_state);
    m_pStateMachine->setPreviousState(start_state);
    m_pStateMachine->setGlobalState(GlobalKeeperState::instance());

    m_pStateMachine->getCurrentState()->onEnter(this);  
}

GoalKeeper::~GoalKeeper()
{
    delete m_pStateMachine;
    
    if (m_ui)
    {
        removeChild(m_ui);
    }    
}

//-------------------------- Update --------------------------------------

void GoalKeeper::update(float dt)
{ 
    //run the logic for the current state
    m_pStateMachine->update(dt);

    //calculate the combined force from each steering behavior 
    Vector2D SteeringForce = m_pSteering->calculate();

    //Acceleration = Force/Mass
    Vector2D Acceleration = SteeringForce / m_dMass;

    //update velocity
    m_vVelocity += Acceleration;

    //make sure player does not exceed maximum velocity
    m_vVelocity.truncate(m_dMaxSpeed);

    //update the position
    m_vPosition += m_vVelocity;

    //enforce a non-penetration constraint if desired
    if(Invoid_Not_Overlap)
    {
        invoidOverlap(this, AutoList<PlayerBase>::getAllMembers());
    }

    //update the heading if the player has a non zero velocity
    if ( !m_vVelocity.isZero())
    {    
        m_vHeading = Vec2Normalize(m_vVelocity);
        m_vSide = m_vHeading.getPerp();
    }

    //look-at vector always points toward the ball
    if (!getPitch()->isGoalKeeperHasBall())
    {
        m_vLookAt = Vec2Normalize(getBall()->getPos() - getPos());
    }

    //update UI
    render();
    
}


bool GoalKeeper::isBallWithinRangeForIntercept() const
{
    return (Vec2DistanceSq(getTeam()->getHomeGoal()->center(), getBall()->getPos()) <=
                                            Para_GoalKeeperInterceptRange*Para_GoalKeeperInterceptRange);
}

bool GoalKeeper::isTooFarFromGoalMouth() const
{
    return (Vec2DistanceSq(getPos(), getRearInterposeTarget()) >
                        Para_GoalKeeperInterceptRange*Para_GoalKeeperInterceptRange);
}

Vector2D GoalKeeper::getRearInterposeTarget() const
{
    float xPosTarget = getTeam()->getHomeGoal()->center().x;

    float yPosTarget = getPitch()->getPlayingArea()->center().y - Para_GoalWidth*0.5 + 
                        (getBall()->getPos().y*Para_GoalWidth) /getPitch()->getPlayingArea()->height();

    return Vector2D(xPosTarget, yPosTarget); 
}

//-------------------- handleMessage -------------------------------------
//  routes any messages appropriately
//------------------------------------------------------------------------
bool GoalKeeper::handleMessage(const Telegram& msg)
{
    return m_pStateMachine->handleMessage(msg);
}


void GoalKeeper::updatePortraitByHeading(Vector2D vHeading)
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
    x_offset += 2*iconSize;
    
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

void GoalKeeper::render()
{
    if(!m_ui)
    {
        //for ui
        m_ui = (pNode)getNewNode();
        addChildToUI(m_ui);
    }

    updatePortraitByHeading(m_vHeading);
    setNodePos(m_ui, m_vPosition);
}
