#ifndef FUZZYSET_RIGHTSHOULDER_H
#define FUZZYSET_RIGHTSHOULDER_H
//-----------------------------------------------------------------------------
//
//  Name:   FuzzySet_RightShoulder.h
//
//
//  Desc:   definition of a fuzzy set that has a right shoulder shape. (the
//          maximum value this variable can accept is *any* value greater than 
//          the midpoint.
//-----------------------------------------------------------------------------
#include "common/fuzzy/FuzzySet.h"
#include "misc/utils.h"



class FuzzySet_RightShoulder : public FuzzySet
{
public:
    FuzzySet_RightShoulder(float peak,
                                                    float LeftOffset,
                                                    float RightOffset):FuzzySet( ((peak + RightOffset) + peak) / 2),
                                                                                m_dPeakPoint(peak),
                                                                                m_dLeftOffset(LeftOffset),
                                                                                m_dRightOffset(RightOffset)
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