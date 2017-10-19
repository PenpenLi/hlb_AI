#ifndef GRAVE_MARKERS_H
#define GRAVE_MARKERS_H
#pragma warning (disable : 4786)
//-----------------------------------------------------------------------------
//
//  Name:   GraveMarkers.h
//
//
//  Desc:   Class to record and render graves at the site of a bot's death
//
//-----------------------------------------------------------------------------
#include <list>
#include <vector>
#include "common/2D/Vector2D.h"
#include <chrono>

class GraveMarkers
{
private:
    struct GraveRecord
    {
        Vector2D Position;
        std::chrono::steady_clock::time_point TimeCreated;

        GraveRecord(Vector2D pos):Position(pos),TimeCreated(std::chrono::steady_clock::now())
        {
        }
    };

    typedef std::list<GraveRecord> GraveList;

    float m_dLifeTime;

    GraveList m_GraveList;

public:
    GraveMarkers(float lifetime);

    void update();
    
    void addGrave(Vector2D pos);
};

#endif