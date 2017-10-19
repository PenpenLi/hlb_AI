 #ifndef STATEMACHINE_H
#define STATEMACHINE_H

//------------------------------------------------------------------------
//
//  Name:   StateMachine.h
//
//  Desc:   State machine class. Inherit from this class and create some 
//          states to give your agents FSM functionality
//
//
//------------------------------------------------------------------------
#include <cassert>
#include <string>

#include "State.h"
#include "common/message/Telegram.h"


template <class entity>
class StateMachine
{
private:
    //a pointer to the agent that owns this instance
    entity* m_pOwner;

    State<entity>* m_pCurrentState;

    //a record of the last state the agent was in
    State<entity>* m_pPreviousState;

    //this is called every time the FSM is updated
    State<entity>* m_pGlobalState;
  

public:
    StateMachine(entity* owner):m_pOwner(owner),
                                                       m_pCurrentState(NULL),
                                                       m_pPreviousState(NULL),
                                                       m_pGlobalState(NULL)
    {
    }

    virtual ~StateMachine(){}

    //use these methods to initialize the FSM
    void setGlobalState(State<entity>* s) {m_pGlobalState = s;}
    void setCurrentState(State<entity>* s){m_pCurrentState = s;}
    void setPreviousState(State<entity>* s){m_pPreviousState = s;}
  
    //call this to update the FSM
    void update(float dt) const
    {
        //if a global state exists, call its execute method, else do nothing
        if(m_pGlobalState) 
            m_pGlobalState->onExecute(m_pOwner);

        //same for the current state
        if (m_pCurrentState) 
            m_pCurrentState->onExecute(m_pOwner);
    }

    bool handleMessage(const Telegram& msg) const
    {
        //first see if the current state is valid and that it can handle the message
        if (m_pCurrentState && m_pCurrentState->onMessage(m_pOwner, msg))
        {
            return true;
        }

        //if not, and if a global state has been implemented, send the message to the global state
        if (m_pGlobalState && m_pGlobalState->onMessage(m_pOwner, msg))
        {
            return true;
        }

        return false;
    }

    //change to a new state
    void changeState(State<entity>* pNewState)
    {
        assert(pNewState && "<StateMachine::changeState>:trying to assign null state to current");

        //keep a record of the previous state
        m_pPreviousState = m_pCurrentState;

        //call the exit method of the existing state
        m_pCurrentState->onExit(m_pOwner);

        //change state to the new state
        m_pCurrentState = pNewState;

        //call the entry method of the new state
        m_pCurrentState->onEnter(m_pOwner);
    }

    //change state back to the previous state
    void revertToPreviousState()
    {
        changeState(m_pPreviousState);
    }

    //returns true if the current state's type is equal to the type of the
    //class passed as a parameter. 
    bool isInState(const State<entity>& st)const
    {
        if (typeid(*m_pCurrentState) == typeid(st)) 
            return true;
        return false;
    }

    State<entity>* getGlobalState()   const{return m_pGlobalState;}
    State<entity>* getCurrentState()  const{return m_pCurrentState;}
    State<entity>* getPreviousState() const{return m_pPreviousState;}

    //only ever used during debugging to grab the name of the current state
    std::string getNameOfCurrentState()const
    {
        std::string s(typeid(*m_pCurrentState).name());

        //remove the 'class ' part from the front of the string
        if (s.size() > 5)
        {
            s.erase(0, 6);
        }

        return s;
    }
};

#endif


