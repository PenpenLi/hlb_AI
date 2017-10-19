#ifndef GOAL_H
#define GOAL_H
//------------------------------------------------------------------------
//
//Name:   SoccerGoal.h
//
//Desc:   class to define a goal for a soccer pitch. The goal is defined
//        by two 2D vectors representing the left and right posts.
//
//        Each time-step the method Scored should be called to determine
//        if a goal has been scored.
//
//
//------------------------------------------------------------------------
#include "SoccerBall.h"
#include "common/2D/Vector2D.h"
#include "common/2D/Geometry.h"
#include "common/game/EngineInterface.h"


class SoccerGoal :public BaseEntity
{
public:
    SoccerGoal(Vector2D left, Vector2D right, Vector2D facing):m_vLeftPost(left),
                                                                                       m_vRightPost(right),
                                                                                       m_vCenter((left+right)/2.0),
                                                                                       m_iNumGoalsScored(0),
                                                                                       m_vFacing(facing),
                                                                                       m_ui(nullptr)
    {
    }

    ~SoccerGoal()
    {
        if (m_ui)
        {
            removeChild(m_ui);
        }
    }
    
    //Given the current ball position and the previous ball position,
    //this method returns true if the ball has crossed the goal line 
    //and increments m_iNumGoalsScored
    inline bool isScored(SoccerBall* ball)
    {
        if (lineIntersection2D(ball->getPos(), ball->getOldPos(), m_vLeftPost, m_vRightPost))
        {
            ++m_iNumGoalsScored;
            
            return true;
        }
        
        return false;
    }
    
    Vector2D center()const{return m_vCenter;}
    Vector2D getFacing()const{return m_vFacing;}
    Vector2D getLeftPost()const{return m_vLeftPost;}
    Vector2D getRightPost()const{return m_vRightPost;}

    int getNumGoalsScored()const{return m_iNumGoalsScored;}
    void resetGoalsScored(){m_iNumGoalsScored = 0;}

    void update()
    {
        if (!m_ui)
        {
            m_ui = (pNode)getNewNode();
            addChildToUI(m_ui);

            Vector2D p1 = m_vRightPost+m_vFacing*40;
            Vector2D p2 = m_vLeftPost + m_vFacing*40;

            drawLine(m_ui, m_vLeftPost, m_vRightPost);
            drawLine(m_ui, m_vRightPost,  p1);
            drawLine(m_ui, p1, p2);
            drawLine(m_ui, p2, m_vLeftPost); 
        }
    }
    
private:
    Vector2D m_vLeftPost;
    Vector2D m_vRightPost;

    //a vector representing the facing direction of the goal
    Vector2D m_vFacing;

    //the position of the center of the goal line
    Vector2D m_vCenter;

    //each time Scored() detects a goal this is incremented
    int m_iNumGoalsScored;

    pNode m_ui;
   
};

#endif