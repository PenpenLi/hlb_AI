
#include "ParaConfigSoccer.h"
#include "SoccerBall.h"
#include "common/2D/Geometry.h"
#include "common/game/Wall.h"
#include "common/misc/UtilsEx.h"
#include "common/misc/LogDebug.h"

//----------------------------- addNoiseToKick --------------------------------
//
//  this can be used to vary the accuracy of a player's kick. Just call it 
//  prior to kicking the ball using the ball's position and the ball target as
//  parameters.
//-----------------------------------------------------------------------------
Vector2D addNoiseToKick(Vector2D ballPos, Vector2D ballTarget)
{
    float displacement = (_PI_- _PI_*Para_PlayerKickingAccuracy) * RandFloat_minus1_1();

    Vector2D toTarget = ballTarget - ballPos;

    Vec2DRotateAroundOrigin(toTarget, displacement);

    return toTarget + ballPos;
}

SoccerBall::SoccerBall(Vector2D pos, 
                                        float mass,
                                        std::vector<Wall*>& PitchBoundary):MovingEntity(Vector2D(0,0), //velocity
                                                                                                                    Vector2D(0,1), //heading
                                                                                                                    mass,
                                                                                                                    0, //turn rate - unused
                                                                                                                    -1.0, //max speed - unused
                                                                                                                    0), //max force - unused
                                                                                                m_PitchBoundary(PitchBoundary),
                                                                                                m_ui(nullptr)
{
    m_vPosition = pos;
}

SoccerBall::~SoccerBall()
{
    if(m_ui)
    {
        removeChild(m_ui);
    }
}


//-------------------------- Kick ----------------------------------------
//                                                                        
//  applys a force to the ball in the direction of heading. Truncates
//  the new velocity to make sure it doesn't exceed the max allowable.
//------------------------------------------------------------------------
void SoccerBall::kick(Vector2D direction, float force)
{  
    //ensure direction is normalized
    direction.normalize();

    //calculate the acceleration
    Vector2D acceleration = (direction * force) / m_dMass;

    //update the velocity
    m_vVelocity = acceleration;
}

//----------------------------- Update -----------------------------------
//
//  updates the ball physics, tests for any collisions and adjusts
//  the ball's velocity accordingly
//------------------------------------------------------------------------
void SoccerBall::update(float dt)
{
    //keep a record of the old position so the goal::scored method
    //can utilize it for goal testing
    m_vOldPos = getPos();
    
    //Test for collisions
    testCollisionWithWalls(m_PitchBoundary);

    //Simulate Para_BallFriction. Make sure the speed is positive 
    //first though
    if (m_vVelocity.lengthSq() > Para_BallFriction * Para_BallFriction)
    {
        m_vVelocity += Vec2Normalize(m_vVelocity) * Para_BallFriction;

        m_vPosition += m_vVelocity;
        
        //update heading
        m_vHeading = Vec2Normalize(m_vVelocity);
    }  

    render();
    
}

//---------------------- TimeToCoverDistance -----------------------------
//
//  Given a force and a distance to cover given by two vectors, this
//  method calculates how long it will take the ball to travel between
//  the two points
//------------------------------------------------------------------------
float SoccerBall::timeToCoverDistance(Vector2D A, Vector2D B, float force) const
{
    //this will be the velocity of the ball in the next time step *if*
    //the player was to make the pass. 
    float speed = force / m_dMass;

    //calculate the velocity at B using the equation
    //
    //  v^2 = u^2 + 2as
    //

    //first calculate s (the distance between the two positions)
    float DistanceToCover =  Vec2Distance(A, B);

    float term = speed*speed + 2.0*DistanceToCover*Para_BallFriction;

    //if  (u^2 + 2as) is negative it means the ball cannot reach point B.
    if (term <= 0.0)
    {
        AILOG("week kick to target...");
        return -1.0;
    }

    float v = std::sqrt(term);

    //it IS possible for the ball to reach B and we know its speed when it
    //gets there, so now it's easy to calculate the time using the equation
    //
    //    t = v-u
    //        ---
    //         a
    //
    return (v-speed)/Para_BallFriction;
}

//--------------------- FuturePosition -----------------------------------
//
//  given a time this method returns the ball position at that time in the
//  future
//------------------------------------------------------------------------
Vector2D SoccerBall::futurePosition(float time)const
{
    //using the equation s = ut + 1/2at^2, where s = distance, a = friction
    //u=start velocity

    //calculate the ut term, which is a vector
    Vector2D ut = m_vVelocity * time;

    //calculate the 1/2at^2 term, which is scalar
    float half_a_t_squared = 0.5 * Para_BallFriction * time * time;

    //turn the scalar quantity into a vector by multiplying the value with
    //the normalized velocity vector (because that gives the direction)
    Vector2D ScalarToVector = half_a_t_squared * Vec2Normalize(m_vVelocity);

    //the predicted position is the balls position plus these two terms
    return getPos() + ut + ScalarToVector;
}

//----------------------- TestCollisionWithWalls -------------------------
//
void SoccerBall::testCollisionWithWalls(const std::vector<Wall *>& walls)
{  
    //test ball against each wall, find out which is closest
    int idxClosest = -1;

    Vector2D VelNormal = Vec2Normalize(m_vVelocity);

    Vector2D IntersectionPoint, CollisionPoint;

    float DistToIntersection = FloatMax;

    //iterate through each wall and calculate if the ball intersects.
    //If it does then store the index into the closest intersecting wall
    for (unsigned int w=0; w<walls.size(); ++w)
    {
        //assuming a collision if the ball continued on its current heading 
        //calculate the point on the ball that would hit the wall. This is 
        //simply the wall's normal(inversed) multiplied by the ball's radius
        //and added to the balls center (its position)
        Vector2D ThisCollisionPoint = getPos() - (walls[w]->normal() * getBoundingRadius());

        //calculate exactly where the collision point will hit the plane    
        if (whereIsPoint(ThisCollisionPoint, walls[w]->from(), walls[w]->normal()) == plane_backside)
        {
            float DistToWall = distanceToRayPlaneIntersection( ThisCollisionPoint,
                                                                                                walls[w]->normal(),
                                                                                                walls[w]->from(),
                                                                                                walls[w]->normal());

            IntersectionPoint = ThisCollisionPoint + (DistToWall * walls[w]->normal());
        }
        else
        {
            float DistToWall = distanceToRayPlaneIntersection(ThisCollisionPoint,
                                                                                               VelNormal,
                                                                                               walls[w]->from(),
                                                                                               walls[w]->normal());

            IntersectionPoint = ThisCollisionPoint + (DistToWall * VelNormal);
        }
    
        //check to make sure the intersection point is actually on the line segment
        bool OnLineSegment = false;

        if (lineIntersection2D(walls[w]->from(), walls[w]->to(),
                                           ThisCollisionPoint - walls[w]->normal()*20.0,
                                           ThisCollisionPoint + walls[w]->normal()*20.0))
        {
            OnLineSegment = true;                                               
        }

  
        //Note, there is no test for collision with the end of a line segment

        //now check to see if the collision point is within range of the
        //velocity vector. [work in distance squared to avoid sqrt] and if it
        //is the closest hit found so far. 
        //If it is that means the ball will collide with the wall sometime
        //between this time step and the next one.
        float distSq = Vec2DistanceSq(ThisCollisionPoint, IntersectionPoint);
        if ((distSq <= m_vVelocity.lengthSq()) && (distSq < DistToIntersection) && OnLineSegment)            
        {        
            DistToIntersection = distSq;
            idxClosest = w;
            CollisionPoint = IntersectionPoint;
        }     
  }//next wall

    
    //to prevent having to calculate the exact time of collision we
    //can just check if the velocity is opposite to the wall normal
    //before reflecting it. This prevents the case where there is overshoot
    //and the ball gets reflected back over the line before it has completely
    //reentered the playing area.
    if ( (idxClosest >= 0 ) && VelNormal.dot(walls[idxClosest]->normal()) < 0)
    {
        m_vVelocity.reflect(walls[idxClosest]->normal());   
    }
}

//----------------------- PlaceAtLocation -------------------------------------
//
//  positions the ball at the desired location and sets the ball's velocity to
//  zero
//-----------------------------------------------------------------------------
void SoccerBall::placeAtPosition(Vector2D NewPos)
{
    m_vPosition = NewPos;
    m_vOldPos = NewPos;
    m_vVelocity.zero();
}

void SoccerBall::render()
{
    if (!m_ui) //update UI
    {
        m_ui = loadImgToNode(getUINode(), "res/game_soccer/ball.png");
        setSize(getNodeSize(m_ui));
    } 
    setNodePos(m_ui, m_vPosition);
}
