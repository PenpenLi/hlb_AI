#ifndef SOCCERBALL_H
#define SOCCERBALL_H
#pragma warning (disable:4786)
//------------------------------------------------------------------------
//
//  Name: SoccerBall.h
//
//  Desc: Class to implement a soccer ball. This class inherits from
//        MovingEntity and provides further functionality for collision
//        testing and position prediction.
//
//
//------------------------------------------------------------------------
#include <vector>
#include "common/game/MovingEntity.h"


class Wall;
class PlayerBase;

class SoccerBall : public MovingEntity
{
public:
    SoccerBall(Vector2D pos, 
                         float mass,
                         std::vector<Wall*>& PitchBoundary);

    ~SoccerBall();

    //tests to see if the ball has collided with a ball and reflects 
    //the ball's velocity accordingly
    void testCollisionWithWalls(const std::vector<Wall*>& walls);
    
    //implement base class Update
    void update(float dt);

    //a soccer ball doesn't need to handle messages
    bool handleMessage(const Telegram& msg) {return false;}

    //this method applies a directional force to the ball (kicks it!)
    void kick(Vector2D direction, float force);

    //given a kicking force and a distance to traverse defined by start
    //and finish points, this method calculates how long it will take the
    //ball to cover the distance.
    float timeToCoverDistance(Vector2D from,Vector2D to, float force) const;

    //this method calculates where the ball will in 'time' seconds
    Vector2D futurePosition(float time)const;

    //this is used by players and goalkeepers to 'trap' a ball -- to stop
    //it dead. That player is then assumed to be in possession of the ball
    //and m_pOwner is adjusted accordingly
    void trap(){m_vVelocity.zero();}  

    Vector2D getOldPos()const{return m_vOldPos;}
  
    //this places the ball at the desired location and sets its velocity to zero
    void placeAtPosition(Vector2D NewPos);

private:
    //keeps a record of the ball's position at the last update
    Vector2D m_vOldPos;

    //a local reference to the Walls that make up the pitch boundary
    const std::vector<Wall *>& m_PitchBoundary;         

    pNode m_ui;

    void render();
    
};


//this can be used to vary the accuracy of a player's kick.
Vector2D addNoiseToKick(Vector2D ballPos, Vector2D ballTarget);

#endif