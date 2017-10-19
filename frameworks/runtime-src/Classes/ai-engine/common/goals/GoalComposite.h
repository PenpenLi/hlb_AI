#ifndef GOAL_COMPOSITE_H
#define GOAL_COMPOSITE_H
//-----------------------------------------------------------------------------
//
//  Name:   GoalComposite.h      
//
//
//  Desc:   Base composite goal class
//-----------------------------------------------------------------------------
#include <list>
#include "Goal.h"


template <class entity_type>
class GoalComposite : public Goal<entity_type>
{
private:
    typedef std::list<Goal<entity_type>* > SubgoalList;

protected:
    //composite goals may have any number of subgoals
    SubgoalList m_SubGoals;

    //processes any subgoals that may be present
    int  processSubgoals();

    //passes the message to the front-most subgoal
    bool forwardMessageToFrontMostSubgoal(const Telegram& msg);

public:
    GoalComposite(entity_type* pE, int type):Goal<entity_type>(pE,type){}

    //when this object is destroyed make sure any subgoals are terminated and destroyed.
    virtual ~GoalComposite(){removeAllSubgoals();}

    //logic to run when the goal is activated.
    virtual void activate() = 0;

    //logic to run each update-step.
    virtual int process() = 0;

    //logic to run prior to the goal's destruction
    virtual void terminate() = 0;

    //if a child class of GoalComposite does not define a message handler
    //the default behavior is to forward the message to the front-most
    //subgoal
    virtual bool handleMessage(const Telegram& msg)
    { 
        return forwardMessageToFrontMostSubgoal(msg);
    }

    //adds a subgoal to the front of the subgoal list
    void addSubgoal(Goal<entity_type>* g);

    //this method iterates through the subgoals and calls each one's terminate
    //method before deleting the subgoal and removing it from the subgoal list
    void removeAllSubgoals();
};





//---------------------- removeAllSubgoals ------------------------------------
//-----------------------------------------------------------------------------
template <class entity_type>
void GoalComposite<entity_type>::removeAllSubgoals()
{
    for (SubgoalList::iterator it = m_SubGoals.begin(); it != m_SubGoals.end();++it)
    {  
        (*it)->terminate();
        
        delete *it;
    }

    m_SubGoals.clear();
}

//-------------------------- ProcessSubGoals ----------------------------------
//
//  this method first removes any completed goals from the front of the
//  subgoal list. It then processes the next goal in the list (if there is one)
//-----------------------------------------------------------------------------
template <class entity_type>
int GoalComposite<entity_type>::processSubgoals()
{ 
    //remove all completed and failed goals from the front of the subgoal list
    while (!m_SubGoals.empty() &&(m_SubGoals.front()->isComplete() || m_SubGoals.front()->hasFailed()))
    {    
        m_SubGoals.front()->terminate();
        delete m_SubGoals.front(); 
        m_SubGoals.pop_front();
    }

    //if any subgoals remain, process the one at the front of the list
    if (!m_SubGoals.empty())
    { 
        //grab the status of the front-most subgoal
        int StatusOfSubGoals = m_SubGoals.front()->process();

        //we have to test for the special case where the front-most subgoal
        //reports 'completed' *and* the subgoal list contains additional goals.When
        //this is the case, to ensure the parent keeps processing its subgoal list
        //we must return the 'active' status.
        if (StatusOfSubGoals == completed && m_SubGoals.size() > 1)
        {
            return active;
        }

        return StatusOfSubGoals;
    }
    else //no more subgoals to process - return 'completed'
    {
        return completed;
    }
}

//----------------------------- addSubgoal ------------------------------------
template <class entity_type>
void GoalComposite<entity_type>::addSubgoal(Goal<entity_type>* g)
{   
    //add the new goal to the front of the list
    m_SubGoals.push_front(g);
}

//---------------- forwardMessageToFrontMostSubgoal ---------------------------
//
//  passes the message to the goal at the front of the queue
//-----------------------------------------------------------------------------
template <class entity_type>
bool GoalComposite<entity_type>::forwardMessageToFrontMostSubgoal(const Telegram& msg)
{
    if (!m_SubGoals.empty())
    {
        return m_SubGoals.front()->handleMessage(msg);
    }

    //return false if the message has not been handled
    return false;
}

#endif

