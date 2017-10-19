#ifndef RAVEN_SENSORY_SYSTEM_H
#define RAVEN_SENSORY_SYSTEM_H
#pragma warning (disable:4786)
//-----------------------------------------------------------------------------
//
//  Name:
//
//
//  Desc:
//
//-----------------------------------------------------------------------------
#include <map>
#include <list>
#include "common/2D/Vector2D.h"
#include <chrono>


class Raven_Bot;

class MemoryRecord
{
public:
    //records the time the opponent was last sensed (seen or heard). This
    //is used to determine if a bot can 'remember' this record or not. 
    //(if CurrentTime() - m_dTimeLastSensed is greater than the bot's
    //memory span, the data in this record is made unavailable to clients)
    std::chrono::steady_clock::time_point fTimeLastSensed;

    //it can be useful to know how long an opponent has been visible. This 
    //variable is tagged with the current time whenever an opponent first becomes
    //visible. It's then a simple matter to calculate how long the opponent has
    //been in view (CurrentTime - fTimeBecameVisible)
    std::chrono::steady_clock::time_point fTimeBecameVisible;

    //it can also be useful to know the last time an opponent was seen
    std::chrono::steady_clock::time_point fTimeLastVisible;

    //a vector marking the position where the opponent was last sensed. This can
    // be used to help hunt down an opponent if it goes out of view
    Vector2D vLastSensedPosition;

    //set to true if opponent is within the field of view of the owner
    bool bWithinFOV;

    //set to true if there is no obstruction between the opponent and the owner, 
    //permitting a shot.
    bool bShootable;


    MemoryRecord():bWithinFOV(false), bShootable(false)
    {
        auto tt = std::chrono::steady_clock::now() - std::chrono::seconds(999);
        fTimeLastSensed = tt;
        fTimeBecameVisible = tt;
        fTimeLastVisible = tt;
    }
};



class Raven_SensoryMemory
{
private:
    typedef std::map<Raven_Bot*, MemoryRecord> MemoryMap;

private:
  
    //the owner of this instance
    Raven_Bot* m_pOwner;

    //this container is used to simulate memory of sensory events. A MemoryRecord
    //is created for each opponent in the environment. Each record is updated 
    //whenever the opponent is encountered. (when it is seen or heard)
    MemoryMap m_MemoryMap;

    //a bot has a memory span (seconds) equivalent to this value. When a bot requests a 
    //list of all recently sensed opponents this value is used to determine if 
    //the bot is able to remember an opponent or not.
    int m_dMemorySpan;

    //this methods checks to see if there is an existing record for pBot. If
    //not a new MemoryRecord record is made and added to the memory map.(called
    //by UpdateWithSoundSource & UpdateVision)
    void makeNewRecordIfNotAlreadyPresent(Raven_Bot* pBot);

public:
    Raven_SensoryMemory(Raven_Bot* owner, float MemorySpan);

  //this method is used to update the memory map whenever an opponent makes
  //a noise
  void updateWithSoundSource(Raven_Bot* pNoiseMaker);

  //this removes a bot's record from memory
  void removeBotFromMemory(Raven_Bot* pBot);

  //this method iterates through all the opponents in the game world and 
  //updates the records of those that are in the owner's FOV
  void updateVision();

  bool isOpponentShootable(Raven_Bot* pOpponent)const;
  bool isOpponentWithinFOV(Raven_Bot* pOpponent)const;
  Vector2D getLastRecordedPositionOfOpponent(Raven_Bot* pOpponent)const;
  float getTimeOpponentHasBeenVisible(Raven_Bot* pOpponent)const;
  float getTimeSinceLastSensed(Raven_Bot* pOpponent)const;
  float getTimeOpponentHasBeenOutOfView(Raven_Bot* pOpponent)const;

  //this method returns a list of all the opponents that have had their
  //records updated within the last m_dMemorySpan seconds.
  std::list<Raven_Bot*> getListOfRecentlySensedOpponents()const;
};


#endif