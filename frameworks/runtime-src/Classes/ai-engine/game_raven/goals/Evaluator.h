#ifndef GOAL_EVALUATOR_H
#define GOAL_EVALUATOR_H
#pragma warning (disable : 4786)
//-----------------------------------------------------------------------------
//
//  Name:   Evaluator.h
//
//
//  Desc:   class template that defines an interface for objects that are
//          able to evaluate the desirability of a specific strategy level goal
//-----------------------------------------------------------------------------
class Raven_Bot;


class Evaluator
{
public:
    Evaluator(float CharacterBias):m_dCharacterBias(CharacterBias){}

    virtual ~Evaluator(){}

    //returns a score between 0 and 1 representing the desirability of the
    //strategy the concrete subclass represents
    virtual float calculateDesirability(Raven_Bot* pBot)=0; 

    //adds the appropriate goal to the given bot's brain
    virtual void setGoal(Raven_Bot* pBot) = 0; 
    
protected:
    //when the desirability score for a goal has been evaluated it is multiplied 
    //by this value. It can be used to create bots with preferences based upon
    //their personality
    float m_dCharacterBias;
    
};




#endif

