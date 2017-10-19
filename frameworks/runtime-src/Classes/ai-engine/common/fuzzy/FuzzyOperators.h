#ifndef FUZZY_OPERATORS_H
#define FUZZY_OPERATORS_H
//-----------------------------------------------------------------------------
//
//  Name:   FuzzyOperators.h
//
//  Author: Mat Buckland (www.ai-junkie.com)
//
//  Desc:   classes to provide the fuzzy AND and OR operators to be used in
//          the creation of a fuzzy rule base
//-----------------------------------------------------------------------------
#include <vector>
#include <cassert>
#include "misc/utils.h"
#include "FuzzyTerm.h"

///////////////////////////////////////////////////////////////////////////////
//
//  a fuzzy AND operator class
//
///////////////////////////////////////////////////////////////////////////////
class FzAND : public FuzzyTerm
{
private:
    //an instance of this class may AND together up to 4 terms
    std::vector<FuzzyTerm*> m_Terms;

    //disallow assignment
    FzAND& operator=(const FzAND&);

public:
    ~FzAND();

    //copy ctor
    FzAND(const FzAND& fa);

    //ctors accepting fuzzy terms.
    FzAND(FuzzyTerm& op1, FuzzyTerm& op2);
    FzAND(FuzzyTerm& op1, FuzzyTerm& op2, FuzzyTerm& op3);
    FzAND(FuzzyTerm& op1, FuzzyTerm& op2, FuzzyTerm& op3, FuzzyTerm& op4);

    //virtual ctor
    FuzzyTerm* clone()const{return new FzAND(*this);}

    float getDOM()const;
    void clearDOM();
    void orWithDOM(float val);
};


///////////////////////////////////////////////////////////////////////////////
//
//  a fuzzy OR operator class
//
///////////////////////////////////////////////////////////////////////////////
class FzOR : public FuzzyTerm
{
private:
    //an instance of this class may AND together up to 4 terms
    std::vector<FuzzyTerm*> m_Terms;

    //no assignment op necessary
    FzOR& operator=(const FzOR&);

public:

    ~FzOR();

    //copy ctor
    FzOR(const FzOR& fa);

    //ctors accepting fuzzy terms.
    FzOR(FuzzyTerm& op1, FuzzyTerm& op2);
    FzOR(FuzzyTerm& op1, FuzzyTerm& op2, FuzzyTerm& op3);
    FzOR(FuzzyTerm& op1, FuzzyTerm& op2, FuzzyTerm& op3, FuzzyTerm& op4);

    //virtual ctor
    FuzzyTerm* clone()const{return new FzOR(*this);}

    float getDOM()const;

    //unused
    void clearDOM(){assert(0 && "<FzOR::ClearDOM>: invalid context");}
    
    void orWithDOM(float val){assert(0 && "<FzOR::ORwithDOM>: invalid context");}
    
};



#endif

