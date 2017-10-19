#ifndef FUZZYSET_TRIANGLE_H
#define FUZZYSET_TRIANGLE_H
//-----------------------------------------------------------------------------
//
//  Name:   FuzzySetTriangle.h
//
//
//  Desc:   This is a simple class to define fuzzy sets that have a triangular 
//          shape and can be defined by a mid point, a left displacement and a
//          right displacement. 
//-----------------------------------------------------------------------------
#include "common/fuzzy/FuzzySet.h"



class FuzzySet_Triangle : public FuzzySet
{
public:
    FuzzySet_Triangle(float mid,float lft,float rgt):FuzzySet(mid), 
                                                                                m_dPeakPoint(mid),
                                                                                m_dLeftOffset(lft),
                                                                                m_dRightOffset(rgt)
  {
  }

  //this method calculates the degree of membership for a particular value
  float calculateDOM(float val)const;

private:
    //the values that define the shape of this FLV
    float m_dPeakPoint;
    float m_dLeftOffset;
    float m_dRightOffset;  
};



#endif