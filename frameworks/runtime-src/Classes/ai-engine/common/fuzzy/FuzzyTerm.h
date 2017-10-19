#ifndef FUZZYTERM_H
#define FUZZYTERM_H
//-----------------------------------------------------------------------------
//
//  Name:   FuzzyTerm.h
//
//
//  Desc:   abstract class to provide an interface for classes able to be
//          used as terms in a fuzzy if-then rule base.
//-----------------------------------------------------------------------------

class FuzzyTerm
{  
public:
    virtual ~FuzzyTerm(){}

    //all terms must implement a virtual constructor
    virtual FuzzyTerm* clone()const = 0;

    //retrieves the degree of membership of the term
    virtual float getDOM()const=0;

    //clears the degree of membership of the term
    virtual void clearDOM()=0;

    //method for updating the DOM of a consequent when a rule fires
    virtual void orWithDOM(double val)=0;
};



#endif