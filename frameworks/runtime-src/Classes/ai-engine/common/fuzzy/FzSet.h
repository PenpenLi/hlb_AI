#ifndef PROXY_FUZZY_SET
#define PROXY_FUZZY_SET
//-----------------------------------------------------------------------------
//
//  Name:   FzSet.h
//
//
//  Desc:   class to provide a proxy for a fuzzy set. The proxy inherits from
//          FuzzyTerm and therefore can be used to create fuzzy rules
//-----------------------------------------------------------------------------
#include "FuzzyTerm.h"
#include "FuzzySet.h"

class FzAND;

class FzSet : public FuzzyTerm
{
private:
    //let the hedge classes be friends 
    friend class FzVery;
    friend class FzFairly;

private:
    //a reference to the fuzzy set this proxy represents
    FuzzySet& m_Set;

public:
    FzSet(FuzzySet& fs):m_Set(fs){}

    FuzzyTerm* clone()const{return new FzSet(*this);}
    float getDOM()const {return m_Set.getDOM();}
    void clearDOM(){m_Set.clearDOM();}
    void orWithDOM(float val){m_Set.orWithDOM(val);}
};


#endif

