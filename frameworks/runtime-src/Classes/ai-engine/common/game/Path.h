#ifndef PATH_H
#define PATH_H
//------------------------------------------------------------------------
//
//  Name:   Path.h
//
//  Desc:   class to define, manage, and traverse a path (defined by a series of 2D vectors)
//          
//
//
//------------------------------------------------------------------------
#include <list>
#include <cassert>
#include "common/2D/Vector2D.h"


class Path
{
public:
  
    Path():m_bLooped(false){}

    //constructor for creating a path with initial random waypoints. MinX/Y
    //& MaxX/Y define the bounding box of the path.
    Path(int NumWaypoints,
                float MinX,
                float MinY,
                float MaxX,
                float MaxY,
                bool looped):m_bLooped(looped)
    {
        createRandomPath(NumWaypoints, MinX, MinY, MaxX, MaxY);
        curWaypoint = m_WayPoints.begin();
    }


    //returns the current waypoint
    Vector2D currentWaypoint()const{return *curWaypoint;}

    //returns true if the end of the list has been reached
    bool isFinished(){return !(curWaypoint != m_WayPoints.end());}
  
    //moves the iterator on to the next waypoint in the list
    void setNextWaypoint();

    //creates a random path which is bound by rectangle described by
    //the min/max values
    std::list<Vector2D> createRandomPath( int NumWaypoints,
                                                                           float MinX,
                                                                           float MinY,
                                                                           float MaxX,
                                                                           float MaxY);


    void loopOn(){m_bLooped = true;}
    void loopOff(){m_bLooped = false;}
 
    //adds a waypoint to the end of the path
    void addWayPoint(Vector2D new_point);

    //methods for setting the path with either another Path or a list of vectors
    void setPath(std::list<Vector2D> new_path){m_WayPoints = new_path;curWaypoint = m_WayPoints.begin();}
    void setPath(const Path& path){m_WayPoints = path.getPath(); curWaypoint = m_WayPoints.begin();}
    std::list<Vector2D> getPath()const{return m_WayPoints;}

    void clear(){m_WayPoints.clear();} 

private:
    std::list<Vector2D> m_WayPoints;

    //points to the current waypoint
    std::list<Vector2D>::iterator curWaypoint;

    //flag to indicate if the path should be looped
    //(The last waypoint connected to the first)
    bool m_bLooped;
};

#endif