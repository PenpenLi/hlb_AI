#include "Raven_Door.h"
#include "common/message/Telegram.h"
#include <algorithm>
#include "RavenMessages.h"
#include "Raven_Map.h"
#include "common/game/Wall.h"

//---------------------------- ctor -------------------------------------------
//-----------------------------------------------------------------------------
Raven_Door::Raven_Door(Raven_Map* pMap, Vector2D m_vP1, Vector2D m_vP2):m_Status(status_closed),
                                  m_iNumTicksStayOpen(60)                   //MGC
{
    m_vtoP2Norm =  Vec2Normalize(m_vP2 - m_vP1);
    m_dCurrentSize = m_dSize = Vec2Distance(m_vP2, m_vP1);

    Vector2D perp = m_vtoP2Norm.getPerp();

    //create the walls that make up the door's geometry
    m_pWall1 = pMap->addWall(m_vP1+perp, m_vP2+perp);
    m_pWall2 = pMap->addWall(m_vP2-perp, m_vP1-perp);
}

//---------------------------- dtor -------------------------------------------
//-----------------------------------------------------------------------------
Raven_Door::~Raven_Door()
{
}


//--------------------------- Update ------------------------------------------
void Raven_Door::update()
{
    switch (m_Status)
    {
        case status_opening:
            open(); 
            break;

        case status_closing:
            close(); 
            break;

        case status_open:
            {
                if (m_iNumTicksCurrentlyOpen-- < 0)
                {
                    m_Status = status_closing;
                }
            }
    }
}

//---------------------------- changePosition ---------------------------------
void Raven_Door::changePosition(Vector2D newP1, Vector2D newP2)
{
    m_vP1 = newP1;
    m_vP2 = newP2;

    m_pWall1->setFrom(m_vP1 + m_vtoP2Norm.getPerp());
    m_pWall1->setTo(m_vP2 + m_vtoP2Norm.getPerp());

    m_pWall2->setFrom(m_vP2 - m_vtoP2Norm.getPerp());
    m_pWall2->setTo(m_vP1 - m_vtoP2Norm.getPerp());
}

//---------------------------- open -------------------------------------------
void Raven_Door::open()
{
    if (m_Status == status_opening)
    {
        if (m_dCurrentSize < 2)
        {
            m_Status = status_open;
            m_iNumTicksCurrentlyOpen = m_iNumTicksStayOpen;
            return;
        }

        //reduce the current size
        m_dCurrentSize -= 1;

        clamp(m_dCurrentSize, 0, m_dSize);

        changePosition(m_vP1, m_vP1 + m_vtoP2Norm * m_dCurrentSize);
    }
}

//------------------------------- close ---------------------------------------
//-----------------------------------------------------------------------------
void Raven_Door::close()
{
    if (m_Status == status_closing)
    {
        if (m_dCurrentSize == m_dSize)
        {
            m_Status = status_closed;
            return;
        }

        //reduce the current size
        m_dCurrentSize += 1;
        clamp(m_dCurrentSize, 0, m_dSize);
        changePosition(m_vP1, m_vP1 + m_vtoP2Norm * m_dCurrentSize);
    }
}

//-------------------------- addSwitch ----------------------------------------
//
//  adds the id of a trigger for the door to notify when operating
//-----------------------------------------------------------------------------
void Raven_Door::addSwitch(unsigned int id)
{
    //only add the trigger if it isn't already present
    if (std::find(m_Switches.begin(),m_Switches.end(),id) == m_Switches.end())
    {
        m_Switches.push_back(id);
    }
}

//------------------------------- HandleMessage -------------------------------
//-----------------------------------------------------------------------------
bool Raven_Door::handleMessage(const Telegram& msg)
{
    if (msg.m_msgId == Msg_OpenSesame)
    {
        if (m_Status != status_open)
        {
            m_Status = status_opening;
        }
        
        return true;
    }

    return false;
}

