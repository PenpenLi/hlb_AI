
#ifndef MOVING_ENTITY_H
#define MOVING_ENTITY_H 

#include "BaseEntity.h"
#include "EngineInterface.h"
#include "common/misc/UtilsEx.h"
#include "common/2D/Matrix2D.h"

class MovingEntity:public BaseEntity
{

public:
    MovingEntity(Vector2D velocity,
                               Vector2D heading,
                               float mass,
                               float turn_rate,
                               float max_speed,
                               float max_force):m_vVelocity(velocity),
                                                            m_vHeading(heading),
                                                            m_dMass(mass),
                                                            m_vSide(m_vHeading.getPerp()),
                                                            m_dMaxSpeed(max_speed),
                                                            m_dMaxTurnRate(turn_rate),
                                                            m_dMaxForce(max_force)
    {
    }

    virtual ~MovingEntity(){}
    
    Vector2D getVelocity() {return m_vVelocity;}

    float getMass() {return m_dMass;}
    
    //a normalized vector pointing in the direction the entity is heading. 
    void setHeading(Vector2D new_heading);
    Vector2D getHeading() const {return m_vHeading;}

    bool rotateHeadingToFacePosition(Vector2D target);
    
    //a vector perpendicular to the heading vector
    Vector2D getSide() {return m_vSide;}
    
    float getSpeed()const {return m_vVelocity.length();}
    void setSpeed(Vector2D velocity) {m_vVelocity = velocity;};

    void setMaxSpeed(float maxSpeed) {m_dMaxSpeed = maxSpeed;}
    float getMaxSpeed() const{return m_dMaxSpeed;}

    void setMaxForce(float maxForce) {m_dMaxForce = maxForce;}
    float getMaxForce() { return m_dMaxForce;}

    void setMaxTurnRate(float maxTurnRate) {m_dMaxTurnRate = maxTurnRate;}
    float getMaxTurnRate() {return m_dMaxTurnRate;}

protected:
  
    Vector2D m_vVelocity;

    //a normalized vector pointing in the direction the entity is heading. 
    Vector2D m_vHeading;

    //a vector perpendicular to the heading vector
    Vector2D m_vSide; 
    
    float  m_dMass;

    //the maximum speed this entity may travel at.
    float m_dMaxSpeed;

    //the maximum force this entity can produce to power itself 
    float m_dMaxForce;

    //the maximum rate (radians per second)this vehicle can rotate         
    float m_dMaxTurnRate;
    
};


//------------------------- setHeading ----------------------------------------
//  first checks that the given heading is not a vector of zero length. If the
//  new heading is valid this fumction sets the entity's heading and side 
//  vectors accordingly
//-----------------------------------------------------------------------------
inline void MovingEntity::setHeading(Vector2D new_heading)
{
    assert( (new_heading.lengthSq() - 1.0) < 0.00001);

    m_vHeading = new_heading;

    //the side vector must always be perpendicular to the heading
    m_vSide = m_vHeading.getPerp();
}

//--------------------------- rotateHeadingToFacePosition ---------------------
//
//  given a target position, this method rotates the entity's heading and
//  side vectors by an amount not greater than m_dMaxTurnRate until it
//  directly faces the target.
//
//  returns true when the heading is facing in the desired direction
//-----------------------------------------------------------------------------
inline bool MovingEntity::rotateHeadingToFacePosition(Vector2D target)
{
    Vector2D toTarget = Vec2Normalize(target - getPos());

    float dt = getHeading().dot(toTarget);

    //some compilers lose acurracy so the value is clamped to ensure it
    //remains valid for the acos
    clamp(dt, -1, 1);

    //first determine the angle between the heading vector and the target
    float angle = std::acos(dt);

    //return true if the player is facing the target
    if (angle < 0.00001) return true;

    //clamp the amount to turn to the max turn rate
    if (angle > m_dMaxTurnRate) angle = m_dMaxTurnRate;

    //The next few lines use a rotation matrix to rotate the player's heading vector accordingly
    Matrix2D rotationMatrix;
  
    //notice how the direction of rotation has to be determined when creating
    //the rotation matrix
    rotationMatrix.Rotate(angle * m_vHeading.sign(toTarget));	
    rotationMatrix.TransformVector2Ds(m_vHeading);
    rotationMatrix.TransformVector2Ds(m_vVelocity);

    //finally recreate m_vSide
    m_vSide = m_vHeading.getPerp();

    return false;
}




#endif 

