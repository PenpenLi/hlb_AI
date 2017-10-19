#ifndef TRIGGER_REGION_H
#define TRIGGER_REGION_H
//-----------------------------------------------------------------------------
//
//  Name:   TriggerRegion.h
//
//  Desc:   class to define a region of influence for a trigger. A 
//          TriggerRegion has one method, isTouching, which returns true if
//          a given position is inside the region
//-----------------------------------------------------------------------------
#include "common/2D/Vector2D.h"
#include "common/2D/InvertedAABBox2D.h"

class TriggerRegion
{
public:
    virtual ~TriggerRegion(){}
    
    //returns true if an entity of the given size and position is intersecting
    //the trigger region.
    virtual bool isTouching(Vector2D EntityPos, float EntityRadius) const = 0;
};


//--------------------------- TriggerRegion_Circle ----------------------------
//
//  class to define a circular region of influence
//-----------------------------------------------------------------------------
class TriggerRegion_Circle : public TriggerRegion
{
public:
    TriggerRegion_Circle(Vector2D pos, float radius):m_dRadius(radius),m_vPos(pos){}

    bool isTouching(Vector2D pos, float EntityRadius)const
    {
        return Vec2DistanceSq(m_vPos, pos) < (EntityRadius + m_dRadius)*(EntityRadius + m_dRadius);
    }

private:
    //the center of the region
    Vector2D m_vPos;
    
    //the radius of the region
    float m_dRadius;
};


//--------------------------- TriggerRegion_Rectangle -------------------------
//
//  class to define a circular region of influence
//-----------------------------------------------------------------------------
class TriggerRegion_Rectangle : public TriggerRegion
{
public:

    TriggerRegion_Rectangle(Vector2D TopLeft, Vector2D BottomRight)
    {
        m_pTrigger = new InvertedAABBox2D(TopLeft, BottomRight);
    }

    ~TriggerRegion_Rectangle(){delete m_pTrigger;}

    //there's no need to do an accurate (and expensive) circle v
    //rectangle intersection test. Instead we'll just test the bounding box of
    //the given circle with the rectangle.
    bool isTouching(Vector2D pos, float EntityRadius)const
    {
        InvertedAABBox2D Box(Vector2D(pos.x-EntityRadius, pos.y-EntityRadius),
                                            Vector2D(pos.x+EntityRadius, pos.y+EntityRadius));

        return Box.isOverlappedWith(*m_pTrigger);
    }

private:
    InvertedAABBox2D* m_pTrigger;
    
};


#endif