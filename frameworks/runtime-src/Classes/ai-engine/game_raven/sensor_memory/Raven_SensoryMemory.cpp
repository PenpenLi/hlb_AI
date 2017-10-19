#include "Raven_SensoryMemory.h"
#include "common/misc/UtilsEx.h"
#include <chrono>

//------------------------------- ctor ----------------------------------------
//-----------------------------------------------------------------------------
 Raven_SensoryMemory:: Raven_SensoryMemory(Raven_Bot* owner,
                                                                                      float MemorySpan):m_pOwner(owner),
                                                                                                    m_dMemorySpan(MemorySpan)
                                                          
{
}

//--------------------- MakeNewRecordIfNotAlreadyPresent ----------------------

void Raven_SensoryMemory::makeNewRecordIfNotAlreadyPresent(Raven_Bot* pOpponent)
{
    //else check to see if this Opponent already exists in the memory. If it doesn't,
    //create a new record
    if (m_MemoryMap.find(pOpponent) == m_MemoryMap.end())
    {
        m_MemoryMap[pOpponent] = MemoryRecord();
    }
}

//------------------------ RemoveBotFromMemory --------------------------------
//
//  this removes a bot's record from memory
//-----------------------------------------------------------------------------
void Raven_SensoryMemory::removeBotFromMemory(Raven_Bot* pBot)
{
    MemoryMap::iterator record = m_MemoryMap.find(pBot);

    if (record != m_MemoryMap.end())
    {
        m_MemoryMap.erase(record);
    }
}
  
//----------------------- UpdateWithSoundSource -------------------------------
//
// this updates the record for an individual opponent. Note, there is no need to
// test if the opponent is within the FOV because that test will be done when the
// UpdateVision method is called
//-----------------------------------------------------------------------------
void Raven_SensoryMemory::updateWithSoundSource(Raven_Bot* pNoiseMaker)
{
    //make sure the bot being examined is not this bot
    if (m_pOwner != pNoiseMaker)
    {
        //if the bot is already part of the memory then update its data, else
        //create a new memory record and add it to the memory
        makeNewRecordIfNotAlreadyPresent(pNoiseMaker);

        MemoryRecord& info = m_MemoryMap[pNoiseMaker];

        //test if there is LOS between bots 
        if (m_pOwner->getWorld()->isLOSOkay(m_pOwner->getPos(), pNoiseMaker->getPos()))
        {
            info.bShootable = true;

            //record the position of the bot
            info.vLastSensedPosition = pNoiseMaker->getPos();
        }
        else
        {
            info.bShootable = false;
        }

        //record the time it was sensed
        info.fTimeLastSensed = std::chrono::steady_clock::now();
    }
}

//----------------------------- UpdateVision ----------------------------------
//
//  this method iterates through all the bots in the game world to test if
//  they are in the field of view. Each bot's memory record is updated
//  accordingly
//-----------------------------------------------------------------------------
void Raven_SensoryMemory::updateVision()
{
    //for each bot in the world test to see if it is visible to the owner of this class
    const std::list<Raven_Bot*>& bots = m_pOwner->getWorld()->getAllBots();
    std::list<Raven_Bot*>::const_iterator curBot;
    for (curBot = bots.begin(); curBot!=bots.end(); ++curBot)
    {
        //make sure the bot being examined is not this bot
        if (m_pOwner != *curBot)
        {
            //make sure it is part of the memory map
            makeNewRecordIfNotAlreadyPresent(*curBot);

            //get a reference to this bot's data
            MemoryRecord& info = m_MemoryMap[*curBot];

            //test if there is LOS between bots 
            if (m_pOwner->getWorld()->isLOSOkay(m_pOwner->getPos(), (*curBot)->getPos()))
            {
                info.bShootable = true;
                
                //test if the bot is within FOV
                if (isSecondInFOVOfFirst(m_pOwner->getPos(),m_pOwner->getFacing(), 
                                                    (*curBot)->getPos(), m_pOwner->getFieldOfView()))
                {
                    auto curTime = std::chrono::steady_clock::now();
                    info.fTimeLastSensed = curTime;
                    info.vLastSensedPosition = (*curBot)->getPos();
                    info.fTimeLastVisible = curTime;

                    if (info.bWithinFOV == false)
                    {
                        info.bWithinFOV = true;
                        info.fTimeBecameVisible = info.fTimeLastSensed;
                    }
                }
                else
                {
                    info.bWithinFOV = false;         
                }
            }
            else
            {
                info.bShootable = false;
                info.bWithinFOV = false;
            }
        }
    }//next bot
}


//------------------------ GetListOfRecentlySensedOpponents -------------------
//
//  returns a list of the bots that have been sensed recently
//-----------------------------------------------------------------------------
std::list<Raven_Bot*> 
Raven_SensoryMemory::getListOfRecentlySensedOpponents()const
{
    //this will store all the opponents the bot can remember
    std::list<Raven_Bot*> opponents;

    float CurTime = std::chrono::steady_clock::now();

    MemoryMap::const_iterator curRecord = m_MemoryMap.begin();
    for (curRecord; curRecord!=m_MemoryMap.end(); ++curRecord)
    {
        //if this bot has been updated in the memory recently, add to list
        if (std::chrono::duration_cast<std::chrono::seconds>(CurTime - 
            curRecord->second.fTimeLastSensed).count() <= m_dMemorySpan)
        {
            opponents.push_back(curRecord->first);
        }
    }

    return opponents;
}

//----------------------------- isOpponentShootable --------------------------------
//
//  returns true if the bot given as a parameter can be shot (ie. its not
//  obscured by walls)
//-----------------------------------------------------------------------------
bool Raven_SensoryMemory::isOpponentShootable(Raven_Bot* pOpponent)const
{
    MemoryMap::const_iterator it = m_MemoryMap.find(pOpponent);

    if (it != m_MemoryMap.end())
    {
        return it->second.bShootable;
    }

    return false;
}

//----------------------------- isOpponentWithinFOV --------------------------------
//
//  returns true if the bot given as a parameter is within FOV
//-----------------------------------------------------------------------------
bool  Raven_SensoryMemory::isOpponentWithinFOV(Raven_Bot* pOpponent)const
{
    MemoryMap::const_iterator it = m_MemoryMap.find(pOpponent);

    if (it != m_MemoryMap.end())
    {
        return it->second.bWithinFOV;
    }

    return false;
}

//---------------------------- GetLastRecordedPositionOfOpponent -------------------
//
//  returns the last recorded position of the bot
//-----------------------------------------------------------------------------
Vector2D  Raven_SensoryMemory::getLastRecordedPositionOfOpponent(Raven_Bot* pOpponent)const
{
    MemoryMap::const_iterator it = m_MemoryMap.find(pOpponent);

    if (it != m_MemoryMap.end())
    {
        return it->second.vLastSensedPosition;
    }

    throw std::runtime_error("< Raven_SensoryMemory::getLastRecordedPositionOfOpponent>: Attempting to get position of unrecorded bot");
}

//----------------------------- GetTimeOpponentHasBeenVisible ----------------------
//
//  returns the amount of time the given bot has been visible
//-----------------------------------------------------------------------------
float  Raven_SensoryMemory::getTimeOpponentHasBeenVisible(Raven_Bot* pOpponent)const
{
    MemoryMap::const_iterator it = m_MemoryMap.find(pOpponent);

    if (it != m_MemoryMap.end() && it->second.bWithinFOV)
    {
        return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - 
                                                                                              it->second.fTimeBecameVisible).count();
    }

    return 0;
}

//-------------------- GetTimeOpponentHasBeenOutOfView ------------------------
//
//  returns the amount of time the given opponent has remained out of view
//  returns a high value if opponent has never been seen or not present
//-----------------------------------------------------------------------------
float Raven_SensoryMemory::getTimeOpponentHasBeenOutOfView(Raven_Bot* pOpponent)const
{
    MemoryMap::const_iterator it = m_MemoryMap.find(pOpponent);

    if (it != m_MemoryMap.end())
    {
        return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - 
                                                                                              it->second.fTimeLastVisible).count();    
    }

    return FloatMax;
}

//------------------------ GetTimeSinceLastSensed ----------------------
//
//  returns the amount of time the given bot has been visible
//-----------------------------------------------------------------------------
float  Raven_SensoryMemory::GetTimeSinceLastSensed(Raven_Bot* pOpponent)const
{
  MemoryMap::const_iterator it = m_MemoryMap.find(pOpponent);
 
  if (it != m_MemoryMap.end() && it->second.bWithinFOV)
  {
    return Clock->GetCurrentTime() - it->second.fTimeLastSensed;
  }

  return 0;
}

//---------------------- RenderBoxesAroundRecentlySensed ----------------------
//
//  renders boxes around the opponents it has sensed recently.
//-----------------------------------------------------------------------------
void  Raven_SensoryMemory::RenderBoxesAroundRecentlySensed()const
{
  std::list<Raven_Bot*> opponents = GetListOfRecentlySensedOpponents();
  std::list<Raven_Bot*>::const_iterator it;
  for (it = opponents.begin(); it != opponents.end(); ++it)
  {
    gdi->OrangePen();
    Vector2D p = (*it)->getPos();
    float   b = (*it)->getBoundingRadius();
      
    gdi->Line(p.x-b, p.y-b, p.x+b, p.y-b);
    gdi->Line(p.x+b, p.y-b, p.x+b, p.y+b);
    gdi->Line(p.x+b, p.y+b, p.x-b, p.y+b);
    gdi->Line(p.x-b, p.y+b, p.x-b, p.y-b);
  }

}