#include "GraveMarkers.h"


//------------------------------- ctor ----------------------------------------
//-----------------------------------------------------------------------------
GraveMarkers::GraveMarkers(float lifetime):m_dLifeTime(lifetime)
{

}


void GraveMarkers::update()
{
    GraveList::iterator it = m_GraveList.begin();
    std::chrono::steady_clock::time_point curTime = std::chrono::steady_clock::now();
    while (it != m_GraveList.end())
    {
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(curTime - it->TimeCreated).count()/1000;
        if (duration > m_dLifeTime)
        {
            it = m_GraveList.erase(it);
        }
        else
        {
            ++it;
        }
    }
}


void GraveMarkers::addGrave(Vector2D pos)
{
    m_GraveList.push_back(GraveRecord(pos));
}
