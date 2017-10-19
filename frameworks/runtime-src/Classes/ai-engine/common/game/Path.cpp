#include "Path.h"
#include "common/misc/UtilsEx.h"
#include "common/2D/Transformations.h"
#include <algorithm>

std::list<Vector2D> Path::createRandomPath( int NumWaypoints,
                                                                               float MinX,
                                                                               float MinY,
                                                                               float MaxX,
                                                                               float MaxY)
{
    m_WayPoints.clear();

    float midX = (MaxX+MinX)/2.0;
    float midY = (MaxY+MinY)/2.0;

    float smaller = std::min(midX, midY);

    float spacing = 2*_PI_/(float)NumWaypoints;

    for (int i=0; i<NumWaypoints; ++i)
    {
        float RadialDist = RandFloatInRange(smaller*0.2f, smaller);

        Vector2D temp(RadialDist, 0.0f);

        Vec2DRotateAroundOrigin(temp, i*spacing);

        temp.x += midX; temp.y += midY;

        m_WayPoints.push_back(temp);
                            
    }

    curWaypoint = m_WayPoints.begin();

    return m_WayPoints;
}


void Path::setNextWaypoint() 
{
    assert (m_WayPoints.size() > 0);

    if (++curWaypoint == m_WayPoints.end())
    {
        if (m_bLooped)
        {
            curWaypoint = m_WayPoints.begin(); 
        }
    }
}  
