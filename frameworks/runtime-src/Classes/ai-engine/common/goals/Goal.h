#ifndef GOAL_H
#define GOAL_H
//-----------------------------------------------------------------------------
//
//  Name:   Goal.h
//
//  Desc:   Base goal class.
//-----------------------------------------------------------------------------

struct Telegram;




template <class entity_type>
class Goal
{
public:
    enum {
        active, 
        inactive, 
        completed,
        failed
    };
  
protected:
    //an enumerated type specifying the type of goal
    int m_iType;

    //a pointer to the entity that owns this goal
    entity_type* m_pOwner;

    //an enumerated value indicating the goal's status (active, inactive, completed, failed)
    int m_iStatus;

    /* the following methods were created to factor out some of the commonality
    in the implementations of the process method() */

    //if m_iStatus = inactive this method sets it to active and calls activate()
    void activateIfInactive();

    //if m_iStatus is failed this method sets it to inactive so that the goal
    //will be reactivated (and therefore re-planned) on the next update-step.
    void reactivateIfFailed();

public:
    //note how goals start off in the inactive state
    Goal(entity_type* pE, int type):m_iType(type),m_pOwner(pE),m_iStatus(inactive)
    {
    }

    virtual ~Goal(){}

    //logic to run when the goal is activated.
    virtual void activate() = 0;

    //logic to run each update-step
    virtual int  process() = 0;

    //logic to run when the goal is satisfied. (typically used to switch
    //off any active steering behaviors)
    virtual void terminate() = 0;

    //goals can handle messages. Many don't though, so this defines a default
    //behavior
    virtual bool handleMessage(const Telegram& msg){return false;}


    //a Goal is atomic and cannot aggregate subgoals yet we must implement
    //this method to provide the uniform interface required for the goal
    //hierarchy.
    virtual void addSubgoal(Goal<entity_type>* g)
    {
        throw std::runtime_error("Cannot add goals to atomic goals");
    }

    bool isComplete()const{return m_iStatus == completed;} 
    bool isActive()const{return m_iStatus == active;}
    bool isInactive()const{return m_iStatus == inactive;}
    bool hasFailed()const{return m_iStatus == failed;}
    int getType()const{return m_iType;} 
};




//if m_iStatus is failed this method sets it to inactive so that the goal
//will be reactivated (replanned) on the next update-step.
template <class entity_type>
void Goal<entity_type>::reactivateIfFailed()
{
    if (hasFailed())
    {
        m_iStatus = inactive;
    }
}

  
template <class entity_type>
void Goal<entity_type>::activateIfInactive()
{
    if (isInactive())
    {
        activate();   
    }
}

#endif