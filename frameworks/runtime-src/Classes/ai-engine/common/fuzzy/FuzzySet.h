#ifndef FUZZYSET_H
#define FUZZYSET_H
//-----------------------------------------------------------------------------
//
//  Name:   FuzzySet.h
//
//
//  Desc:   class to define an interface for a fuzzy set
//-----------------------------------------------------------------------------
#include <string>
#include <cassert>


class FuzzySet
{
protected:
  
    //this will hold the degree of membership of a given value in this set 
    float m_dDOM;
    
    //this is the maximum of the set's membership function. For instamce, if
    //the set is triangular then this will be the peak point of the triangular.
    //if the set has a plateau then this value will be the mid point of the 
    //plateau. This value is set in the constructor to avoid run-time
    //calculation of mid-point values.
    float m_dRepresentativeValue;

public:
    FuzzySet(float RepVal):m_dDOM(0.0), m_dRepresentativeValue(RepVal){}

    //return the degree of membership in this set of the given value. NOTE,
    //this does not set m_dDOM to the DOM of the value passed as the parameter.
    //This is because the centroid defuzzification method also uses this method
    //to determine the DOMs of the values it uses as its sample points.
    virtual float calculateDOM(float val)const = 0;

    //if this fuzzy set is part of a consequent FLV, and it is fired by a rule 
    //then this method sets the DOM (in this context, the DOM represents a
    //confidence level)to the maximum of the parameter value or the set's 
    //existing m_dDOM value
    void orWithDOM(float val){if (val > m_dDOM) m_dDOM = val;}

    //accessor methods
    float getRepresentativeVal()const{return m_dRepresentativeValue;}

    void clearDOM(){m_dDOM = 0.0f;}  
    float getDOM()const{return m_dDOM;}
    void setDOM(float val)
    {
        assert ((val <=1) && (val >= 0) && "<FuzzySet::SetDOM>: invalid value");
        m_dDOM = val;
    }
};


#endif