#ifndef STATE_H
#define STATE_H
//------------------------------------------------------------------------
//
//  Name:   State.h
//
//  Desc:   abstract base class to define an interface for a state
//
//
//------------------------------------------------------------------------
struct Telegram;

template <class entity_type>
class State
{
public:

  virtual ~State(){}

  //this will execute when the state is entered
  virtual void onEnter(entity_type*)=0;

  //this is the states normal update function
  virtual void onExecute(entity_type*)=0;

  //this will execute when the state is exited. 
  virtual void onExit(entity_type*)=0;

  //this executes if the agent receives a message from the 
  //message dispatcher
  virtual bool onMessage(entity_type*, const Telegram&)=0;
};

#endif