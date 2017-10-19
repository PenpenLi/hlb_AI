#ifndef FUZZY_HEDGES_H
#define FUZZY_HEDGES_H
//-----------------------------------------------------------------------------
//
//  Name:   FuzzyHedges.h
//
//
//  Desc:   classes to implement fuzzy hedges 
//-----------------------------------------------------------------------------
#include "FuzzySet.h"
#include "FuzzyTerm.h"
#include <math.h>

class FzVery : public FuzzyTerm
{
private:
    FuzzySet& m_Set;

    //prevent copying and assignment by clients
    FzVery(const FzVery& inst):m_Set(inst.m_Set){}
    FzVery& operator=(const FzVery&);
 
public:

    FzVery(FzSet& ft):m_Set(ft.m_Set){}

    float getDOM()const
    {
        return m_Set.getDOM() * m_Set.getDOM();
    }

    FuzzyTerm* clone()const{return new FzVery(*this);}

    void clearDOM(){m_Set.clearDOM();}
    
    void orWithDOM(float val){m_Set.orWithDOM(val * val);}
};

///////////////////////////////////////////////////////////////////////////////
class FzFairly : public FuzzyTerm
{
private:
    FuzzySet& m_Set;

    //prevent copying and assignment
    FzFairly(const FzFairly& inst):m_Set(inst.m_Set){}
    FzFairly& operator=(const FzFairly&);

public:

    FzFairly(FzSet& ft):m_Set(ft.m_Set){}

    float getDOM()const
    {
        return sqrt(m_Set.getDOM());
    }

    FuzzyTerm* clone()const{return new FzFairly(*this);}

    void clearDOM(){m_Set.clearDOM();}
    
    void orWithDOM(float val){m_Set.orWithDOM(sqrt(val));}
};


#endif