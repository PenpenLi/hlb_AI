#ifndef REGION_H
#define REGION_H
//------------------------------------------------------------------------
//
//  Name:   Region.h
//
//  Desc:   Defines a rectangular region. A region has an identifying
//          number, and four corners.
//
//
//------------------------------------------------------------------------

#include <math.h>
#include <algorithm> //max , min 
#include "common/2D/Vector2D.h"
#include "common/misc/UtilsEx.h"

class Region
{
public:

  enum region_modifier{halfsize, normal};
  
protected:

  float m_dTop;
  float m_dLeft;
  float m_dRight;
  float m_dBottom;

  float m_dWidth;
  float m_dHeight;

  Vector2D m_vCenter;
  
  int m_iID;

public:
    Region( float left, float top, float right, float bottom, int id = -1):m_dTop(top),
                                                                                                        m_dRight(right),
                                                                                                        m_dLeft(left),
                                                                                                        m_dBottom(bottom),
                                                                                                        m_iID(id)
    {
        //calculate center of region
        m_vCenter = Vector2D( (left+right)*0.5, (top+bottom)*0.5 );

        m_dWidth  = fabs(right-left);
        m_dHeight = fabs(bottom-top);
    }

    virtual ~Region(){}

    //returns true if the given position lays inside the region. The
    //region modifier can be used to contract the region bounderies
    bool isInside(Vector2D pos, region_modifier r)const;

    
    //returns a vector representing a random location within the region
    inline Vector2D getRandomPosition()const
    {
        return Vector2D(RandFloatInRange(m_dLeft, m_dRight), 
                                    RandFloatInRange(m_dTop, m_dBottom));
    }

  //-------------------------------
    float top()const{return m_dTop;}
    float bottom()const{return m_dBottom;}
    float left()const{return m_dLeft;}
    float right()const{return m_dRight;}
    float width()const{return fabs(m_dRight - m_dLeft);}
    float height()const{return fabs(m_dTop - m_dBottom);}
    float length()const{return std::max(width(), height());}
    float breadth()const{return std::min(width(), height());}
    Vector2D center()const{return m_vCenter;}
    int getID()const{return m_iID;}
};

inline bool Region::isInside(Vector2D pos, region_modifier r=normal)const
{
    if (r == normal)
    {
        return ((pos.x > m_dLeft) && (pos.x < m_dRight) &&
                        (pos.y > m_dTop) && (pos.y < m_dBottom));
    }
    else
    {
        const float marginX = m_dWidth * 0.25;
        const float marginY = m_dHeight * 0.25;

        return ((pos.x > (m_dLeft+marginX)) && (pos.x < (m_dRight-marginX)) &&
                        (pos.y > (m_dTop+marginY)) && (pos.y < (m_dBottom-marginY)));
    }
}
    
#endif
