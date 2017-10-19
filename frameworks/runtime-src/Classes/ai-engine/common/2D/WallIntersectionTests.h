//-----------------------------------------------------------------------------
//
//  Name:   WallIntersectionTests.h
//
//
//  Desc:   a few functions for testing line segments against containers of walls
//-----------------------------------------------------------------------------

#include "Vector2D.h"
#include "common/game/Wall.h"


//----------------------- doWallsObstructLineSegment --------------------------
//
//  given a line segment defined by the points from and to, iterate through all
//  the map objects and walls and test for any intersection. This method
//  returns true if an intersection occurs.
//-----------------------------------------------------------------------------
template <class ContWall>
inline bool doWallsObstructLineSegment(Vector2D from,Vector2D to, const ContWall& walls)
{
    //test against the walls
    ContWall::const_iterator curWall = walls.begin();

    for (curWall; curWall != walls.end(); ++curWall)
    {
        //do a line segment intersection test
        if (lineIntersection2D(from, to, (*curWall)->from(), (*curWall)->to()))
        {
            return true;
        }
    }
                                                               
    return false;
}


//----------------------- doWallsObstructCylinderSides -------------------------
//
//  similar to above except this version checks to see if the sides described
//  by the cylinder of length |AB| with the given radius intersect any walls.
//  (this enables the trace to take into account any the bounding radii of
//  entity objects)
//-----------------------------------------------------------------------------
template <class ContWall>
inline bool doWallsObstructCylinderSides(Vector2D A,
                                                                            Vector2D B,
                                                                            float BoundingRadius,
                                                                            const ContWall& walls)
{
    //the line segments that make up the sides of the cylinder must be created
    Vector2D toB = Vec2Normalize(B-A);

    //A1B1 will be one side of the cylinder, A2B2 the other.
    Vector2D A1, B1, A2, B2;

    Vector2D radialEdge = toB.getPerp() * BoundingRadius;

    //create the two sides of the cylinder
    A1 = A + radialEdge;
    B1 = B + radialEdge;

    A2 = A - radialEdge;
    B2 = B - radialEdge;

    //now test against them
    if (!doWallsObstructLineSegment(A1, B1, walls))
    {
        return doWallsObstructLineSegment(A2, B2, walls);
    }

    return true;
}

//------------------ findClosestPointOfIntersectionWithWalls ------------------
//
//  tests a line segment against the container of walls  to calculate
//  the closest intersection point, which is stored in the reference 'ip'. The
//  distance to the point is assigned to the reference 'distance'
//
//  returns false if no intersection point found
//-----------------------------------------------------------------------------

template <class ContWall>
inline bool findClosestPointOfIntersectionWithWalls(Vector2D A,
                                                                                                Vector2D B,
                                                                                                float& distance,
                                                                                                Vector2D& ip,
                                                                                                const ContWall& walls)
{
    distance = FloatMax;

    ContWall::const_iterator curWall = walls.begin();
    for (curWall; curWall != walls.end(); ++curWall)
    {
        float dist = 0.0;
        Vector2D point;

        if (lineIntersection2D(A, B, (*curWall)->from(), (*curWall)->to(), dist, point))
        {
            if (dist < distance)
            {
                distance = dist;
                ip = point;
            }
        }
    }

    if (distance < FloatMax) return true;

    return false;
}

//------------------------ doWallsIntersectCircle -----------------------------
//
//  returns true if any walls intersect the circle of radius at point p
//-----------------------------------------------------------------------------
template <class ContWall>
inline bool doWallsIntersectCircle(const ContWall& walls, Vector2D p, float r)
{
    //test against the walls
    ContWall::const_iterator curWall = walls.begin();
    
    for (curWall; curWall != walls.end(); ++curWall)
    {
        //do a line segment intersection test
        if (lineSegmentCircleIntersection((*curWall)->from(), (*curWall)->to(), p, r))
        {
            return true;
        }
    }
                                                               
    return false;
}


