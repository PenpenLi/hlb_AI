
#ifndef OBSTACLE_H
#define OBSTACLE_H

#include "common/game/BaseEntity.h"

class Obstacle : public BaseEntity
{
public:
    Obstacle(float x,float y)
    {
        setPos(Vector2D(x, y));
        
        auto node = loadImgToNode(getUINode(), "game_vehicle/obtacle.png");
        
        m_radius = getNodeSize(node).x;
        m_ui = dynamic_cast<pNode>(node);
    }
    
    ~Obstacle()
    {
        if(m_ui)
        {
            removeChild(m_ui);
        }
    }
    
    float getRadius() {return m_radius;}
    
private:
    float m_radius;
    pNode m_ui;
};


#endif

