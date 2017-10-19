#ifndef INVERTEDAABBOX2D_H
#define INVERTEDAABBOX2D_H

#include "Vector2D.h"

class InvertedAABBox2D
{

public:
  InvertedAABBox2D(Vector2D top_left,Vector2D bottom_right):m_vTopLeft(top_left), 
                                        m_vBottomRight(bottom_right),m_vCenter((top_left+bottom_right)/2.0)
  {
  }

  //returns true if the bbox described by other intersects with this one
    bool isOverlappedWith(const InvertedAABBox2D& other) const
    {
        return !((other.top() > this->bottom()) ||
                       (other.bottom() < this->top()) ||
                       (other.left() > this->right()) ||
                       (other.right() < this->left()));
    }
    
    float top()const{return m_vTopLeft.y;}
    float left()const{return m_vTopLeft.x;}
    float bottom()const{return m_vBottomRight.y;}
    float right()const{return m_vBottomRight.x;}
    Vector2D center()const{return m_vCenter;}
    Vector2D topLeft() const{return m_vTopLeft;}
    Vector2D bottomRight()const{return m_vBottomRight;}
    
private:
    Vector2D  m_vTopLeft;
    Vector2D  m_vBottomRight;
    Vector2D  m_vCenter; 
    
};
  
#endif