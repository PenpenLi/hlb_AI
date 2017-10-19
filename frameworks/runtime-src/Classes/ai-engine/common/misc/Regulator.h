#ifndef REGULATOR
#define REGULATOR
#include <chrono>
#include "common/misc/UtilsEx.h"
//------------------------------------------------------------------------
//  Name:   Regulator.h
//
//  Desc:   Use this class to regulate code flow (for an update function say)
//          Instantiate the class with the frequency you would like your code
//          section to flow (like 10 times per second) and then only allow 
//          the program flow to continue if Ready() returns true
//
//------------------------------------------------------------------------


class Regulator
{
public:
    Regulator(int numUpdatesPerSecond)
    {

        if (numUpdatesPerSecond > 0)
        {
            m_updatePeriod = int(1000 / numUpdatesPerSecond); 
        }
        else if (numUpdatesPerSecond < 0)
        {
            m_updatePeriod = -1;
        }
        else 
        {
            m_updatePeriod = 0;
        }

        m_nextUpdateTime = std::chrono::steady_clock::now() + std::chrono::milliseconds(m_updatePeriod+1);        
    }

    //returns true if the current time exceeds m_nextUpdateTime
    bool isReady()
    {
        //if a regulator is instantiated with a zero freq then it goes into
        //stealth mode (doesn't regulate)
        if (m_updatePeriod == 0) 
            return true;

        //if the regulator is instantiated with a negative freq then it will
        //never allow the code to flow
        if (m_updatePeriod < 0) 
            return false;

        auto curTime = std::chrono::steady_clock::now();
        if (curTime >= m_nextUpdateTime)
        {
            m_nextUpdateTime = curTime + std::chrono::milliseconds(m_updatePeriod); 
            return true;
        }

        return false;
    }
    
private:
    //the time period(ms) between updates 
    int m_updatePeriod;
    
    //the next time the regulator allows code flow
    std::chrono::steady_clock::time_point m_nextUpdateTime;
};



#endif