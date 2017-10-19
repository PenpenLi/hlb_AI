#ifndef RAVEN_Raven_Door_H
#define RAVEN_Raven_Door_H
#pragma warning (disable:4786)
//-----------------------------------------------------------------------------
//
//  Name:   Raven_Door.h
//
//
//  Desc:   class to emulate a sliding door that can be opened by sending
//          it a msg_OpenSesame telegram. The door stays open for a user
//          specified amount of time before closing. 
//-----------------------------------------------------------------------------
#include <vector>
#include "common/2D/Vector2D.h"
#include "common/game/BaseEntity.h"


struct Telegram;
class Raven_Map;
class Wall;


class Raven_Door : public BaseEntity
{
protected:
    enum door_status
    {
        status_open, 
        status_opening, 
        status_closed, 
        status_closing
    };

protected:
    door_status m_Status;

    //a sliding door is created from two walls, back to back.These walls must
    //be added to a map's geometry in order for an agent to detect them
    Wall* m_pWall1;
    Wall* m_pWall2;

    //a container of the id's of the triggers able to open this door
    std::vector<unsigned int> m_Switches;

    //how long the door remains open before it starts to shut again
    int m_iNumTicksStayOpen;

    //how long the door has been open (0 if status is not open)
    int m_iNumTicksCurrentlyOpen;

    //the door's position and size when in the open position
    Vector2D m_vP1;
    Vector2D m_vP2;
    float m_dSize;
  
    //a normalized vector facing along the door. This is used frequently
    //by the other methods so we might as well just calculate it once in the ctor
    Vector2D m_vtoP2Norm;

    //the door's current size
    float m_dCurrentSize;

    void open();
    void close();

    void changePosition(Vector2D newP1, Vector2D newP2);
 
public:
    Raven_Door(Raven_Map* pMap, std::ifstream& is);
    ~Raven_Door();

    //the usual suspects
    void update();
    bool handleMessage(const Telegram& msg);

    //adds the ID of a switch
    void addSwitch(unsigned int id);
    
    std::vector<unsigned int> getSwitchIDs()const{return m_Switches;}
};


#endif
