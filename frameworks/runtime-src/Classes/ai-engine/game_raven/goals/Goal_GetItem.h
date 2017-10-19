#ifndef GOAL_GET_ITEM_H
#define GOAL_GET_ITEM_H
#pragma warning (disable:4786)

#include "Goals/GoalComposite.h"
#include "Raven_Goal_Types.h"
#include "../Raven_Bot.h"
#include "common/triggers/Trigger.h"


//helper function to change an item type enumeration into a goal type
int itemTypeToGoalType(int gt);


class Goal_GetItem : public GoalComposite<Raven_Bot>
{
private:

  int m_iItemToGet;

  Trigger<Raven_Bot>* m_pGiverTrigger;

  //true if a path to the item has been formulated
  bool m_bFollowingPath;

  //returns true if the bot sees that the item it is heading for has been
  //picked up by an opponent
  bool hasItemBeenStolen()const;

public:

    Goal_GetItem(Raven_Bot* pBot,int item):GoalComposite<Raven_Bot>(pBot,
                                                                        ItemTypeToGoalType(item)),
                                                                        m_iItemToGet(item),
                                                                        m_pGiverTrigger(0),
                                                                        m_bFollowingPath(false)
    {
    }


    void activate();

    int  process();

    bool handleMessage(const Telegram& msg);

    void terminate(){m_iStatus = completed;}
};

#endif
