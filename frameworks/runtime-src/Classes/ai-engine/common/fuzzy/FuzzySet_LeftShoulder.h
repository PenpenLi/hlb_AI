#ifndef FUZZYSET_LEFTSHOULDER_H
#define FUZZYSET_LEFTSHOULDER_H
//-----------------------------------------------------------------------------
//
//  Name:   FuzzySet_LeftShoulder.h
//
//
//  Desc:   definition of a fuzzy set that has a left shoulder shape. (the
//          minimum value this variable can accept is *any* value less than the
//          midpoint.
//-----------------------------------------------------------------------------
#include "common/fuzzy/FuzzySet.h"
#include "misc/utils.h"



class FuzzySet_LeftShoulder : public FuzzySet
{
public:
    FuzzySet_LeftShoulder( float peak,
                                                    float LeftOffset,
                                                    float RightOffset):FuzzySet( ((peak - LeftOffset) + peak) / 2),
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
    float m_dRightOffset;
    float m_dLeftOffset;  
};



#endif