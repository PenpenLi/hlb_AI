#ifndef WALL_H
#define WALL_H

#include "common/game/BaseEntity.h"
#include "common/game/EngineInterface.h"

class Wall:public BaseEntity
{
public:
    Wall(Vector2D A, Vector2D B):m_vA(A), m_vB(B),m_ui(nullptr)
    {
        Vector2D temp = Vec2Normalize(m_vB - m_vA);
        m_vN.x = -temp.y;
        m_vN.y = temp.x;
    }

    Vector2D from() const  {return m_vA;}
    Vector2D to()const {return m_vB;}
    Vector2D normal() const {return m_vN;}
    Vector2D center() {return (m_vA+m_vB)/2.0;}

    Wall(Wall &w):m_ui(nullptr)
    {
        m_vA = w.m_vA;
        m_vB = w.m_vB;
        m_vN = w.m_vN;
    }
    
    ~Wall()
    {
        if(m_ui)
        {
            removeChild(m_ui);
        }
    }
    
    void update()
    {
        if (!m_ui)
        {
            m_ui = getNewNode();
            addChildToUI(m_ui);
            drawLine(m_ui, m_vA, m_vB);
        }
    }
private:
    Vector2D m_vA;
    Vector2D m_vB;
    Vector2D m_vN;
    pNode m_ui;
};

#endif 

