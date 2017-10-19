#ifndef FUZZYSET_SINGLETON_H
#define FUZZYSET_SINGLETON_H
//-----------------------------------------------------------------------------
//
//  Name:   FuzzySet_Singleton.h
//
//
//  Desc:   This defines a fuzzy set that is a singleton (a range
//          over which the DOM is always 1.0)
//-----------------------------------------------------------------------------
#include "common/fuzzy/FuzzySet.h"
#include "common/misc/UtilsEx.h"


class FuzzySet_Singleton : public FuzzySet
{
public:
    FuzzySet_Singleton(float mid,
                                            float lft,
                                            float rgt):FuzzySet(mid),
                                                          m_dMidPoint(mid),
                                                          m_dLeftOffset(lft),
                                                          m_dRightOffset(rgt)
      {
      }

    //this method calculates the degree of membership for a particular value
    float calculateDOM(float val)const; 
    
 private:
    //the values that define the shape of this FLV
    float m_dMidPoint;
    float m_dLeftOffset;
    float m_dRightOffset;
};


#endif