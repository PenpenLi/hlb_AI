#include "fuzzyvariable.h"
#include "fuzzyoperators.h"
#include "FuzzySet_triangle.h"
#include "FuzzySet_LeftShoulder.h"
#include "FuzzySet_RightShoulder.h"
#include "FuzzySet_Singleton.h"
#include "FzSet.h"
#include "common/misc/UtilsEx.h"
#include <cassert>
#include <algorithm>

//------------------------------ dtor -----------------------------------------
//-----------------------------------------------------------------------------
FuzzyVariable::~FuzzyVariable()
{
    MemberSets::iterator it;
    for (it = m_MemberSets.begin(); it != m_MemberSets.end(); ++it)
    {
        delete it->second;
    }
}

//--------------------------- fuzzify -----------------------------------------
//
//  takes a crisp value and calculates its degree of membership for each set in the variable.
//-----------------------------------------------------------------------------
void FuzzyVariable::fuzzify(float val)
{    
    //make sure the value is within the bounds of this variable
    assert ( (val >= m_dMinRange) && (val <= m_dMaxRange) && "<FuzzyVariable::fuzzify>: value out of range");

    //for each set in the flv calculate the DOM for the given value
    MemberSets::const_iterator curSet;
    for (curSet = m_MemberSets.begin(); curSet != m_MemberSets.end(); ++curSet)
    {
        curSet->second->setDOM(curSet->second->calculateDOM(val));
    }
}


//--------------------------- deFuzzifyMaxAv ----------------------------------
//
// defuzzifies the value by averaging the maxima of the sets that have fired
//
// OUTPUT = sum (maxima * DOM) / sum (DOMs) 
//-----------------------------------------------------------------------------
float FuzzyVariable::deFuzzifyMaxAv()const
{
    float bottom = 0.0f;
    float top = 0.0f;

    MemberSets::const_iterator curSet;
    for (curSet = m_MemberSets.begin(); curSet != m_MemberSets.end(); ++curSet)
    {
        top += curSet->second->getRepresentativeVal() * curSet->second->getDOM();
        
        bottom += curSet->second->getDOM();
    }

    //make sure bottom is not equal to zero
    if (isEqual(0, bottom)) return 0.0;

    return top / bottom;   
}

//------------------------- deFuzzifyCentroid ---------------------------------
//
//  defuzzify the variable using the centroid method
//-----------------------------------------------------------------------------
float FuzzyVariable::deFuzzifyCentroid(int NumSamples)const
{
    //calculate the step size
    float StepSize = (m_dMaxRange - m_dMinRange)/(float)NumSamples;
    float TotalArea = 0.0f;
    float SumOfMoments = 0.0f;

    //step through the range of this variable in increments equal to StepSize
    //adding up the contribution (lower of calculateDOM or the actual DOM of this
    //variable's fuzzified value) for each subset. This gives an approximation of
    //the total area of the fuzzy manifold.(This is similar to how the area under
    //a curve is calculated using calculus... the heights of lots of 'slices' are
    //summed to give the total area.)
    //
    //in addition the moment of each slice is calculated and summed. Dividing
    //the total area by the sum of the moments gives the centroid. (Just like
    //calculating the center of mass of an object)
    for (int samp=1; samp<=NumSamples; ++samp)
    {
        //for each set get the contribution to the area. This is the lower of the 
        //value returned from calculateDOM or the actual DOM of the fuzzified 
        //value itself   
        MemberSets::const_iterator curSet = m_MemberSets.begin();
        for (curSet; curSet != m_MemberSets.end(); ++curSet)
        {
            float contribution = 
            std::min(curSet->second->calculateDOM(m_dMinRange + samp * StepSize),
                            curSet->second->getDOM());

            TotalArea += contribution;

            SumOfMoments += (m_dMinRange + samp * StepSize)  * contribution;
        }
    }

    //make sure total area is not equal to zero
    if (isEqual(0, TotalArea)) return 0.0;

    return (SumOfMoments / TotalArea);
}

//------------------------- addTriangularSet ----------------------------------
//
//  adds a triangular shaped fuzzy set to the variable
//-----------------------------------------------------------------------------
FzSet FuzzyVariable::addTriangularSet(std::string name,
                                                                        float minBound,
                                                                        float peak,
                                                                        float maxBound)
{
    m_MemberSets[name] = new FuzzySet_Triangle(peak, peak-minBound, maxBound-peak);
    //adjust range if necessary
    adjustRangeToFit(minBound, maxBound);
    
    return FzSet(*m_MemberSets[name]);
}

//--------------------------- AddLeftShoulder ---------------------------------
//
//  adds a left shoulder type set
//-----------------------------------------------------------------------------
FzSet FuzzyVariable::addLeftShoulderSet(std::string name,
                                                                            float minBound,
                                                                            float peak,
                                                                            float maxBound)
{
    m_MemberSets[name] = new FuzzySet_LeftShoulder(peak, peak-minBound, maxBound-peak);

    //adjust range if necessary
    adjustRangeToFit(minBound, maxBound);

    return FzSet(*m_MemberSets[name]);
}


//--------------------------- AddRightShoulder ---------------------------------
//
//  adds a left shoulder type set
//-----------------------------------------------------------------------------
FzSet FuzzyVariable::addRightShoulderSet(std::string name,
                                                                                 float minBound,
                                                                                 float peak,
                                                                                 float maxBound)
{
    m_MemberSets[name] = new FuzzySet_RightShoulder(peak, peak-minBound, maxBound-peak);

    //adjust range if necessary
    adjustRangeToFit(minBound, maxBound);

    return FzSet(*m_MemberSets[name]);
}


//--------------------------- addSingletonSet ---------------------------------
//
//  adds a singleton to the variable
//-----------------------------------------------------------------------------
FzSet FuzzyVariable::addSingletonSet(std::string name,
                                                                        float minBound,
                                                                        float peak,
                                                                        float maxBound)
{
    m_MemberSets[name] = new FuzzySet_Singleton(peak, peak-minBound, maxBound-peak);

    adjustRangeToFit(minBound, maxBound);

    return FzSet(*m_MemberSets[name]);
}

//---------------------------- adjustRangeToFit -------------------------------
//
//  this method is called with the upper and lower bound of a set each time a
//  new set is added to adjust the upper and lower range values accordingly
//-----------------------------------------------------------------------------
void FuzzyVariable::adjustRangeToFit(float minBound, float maxBound)
{
    if (minBound < m_dMinRange) m_dMinRange = minBound;
    if (maxBound > m_dMaxRange) m_dMaxRange = maxBound;
}
