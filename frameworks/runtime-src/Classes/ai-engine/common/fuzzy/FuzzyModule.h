#ifndef FUZZY_MODULE_H
#define FUZZY_MODULE_H
#pragma warning (disable:4786)
//-----------------------------------------------------------------------------
//
//  Name:   FuzzyModule.h
//
//
//  Desc:   this class describes a fuzzy module: a collection of fuzzy variables
//          and the rules that operate on them.
//
//-----------------------------------------------------------------------------
#include <vector>
#include <string>
#include <map>
#include "FuzzySet.h"
#include "FuzzyVariable.h"
#include "FuzzyRule.h"
#include "FuzzyOperators.h"
#include "FzSet.h"
#include "FuzzyHedges.h"



class FuzzyModule
{
private:
    typedef std::map<std::string, FuzzyVariable*> VarMap;
  
public:

    //you must pass one of these values to the defuzzify method. This module
    //only supports the MaxAv and centroid methods.
    enum DefuzzifyMethod{max_av, centroid};

    //when calculating the centroid of the fuzzy manifold this value is used
    //to determine how many cross-sections should be sampled
    enum {NumSamples = 15};

private:

    //a map of all the fuzzy variables this module uses
    VarMap m_Variables;

    //a vector containing all the fuzzy rules
    std::vector<FuzzyRule*> m_Rules;


    //zeros the DOMs of the consequents of each rule. Used by defuzzify()
    inline void setConfidencesOfConsequentsToZero();


public:

    ~FuzzyModule();

    //creates a new 'empty' fuzzy variable and returns a reference to it.
    FuzzyVariable& createFLV(const std::string& VarName);

    //adds a rule to the module
    void addRule(FuzzyTerm& antecedent, FuzzyTerm& consequence);

    //this method calls the fuzzify method of the named FLV 
    inline void fuzzify(const std::string& NameOfFLV, float val);

    //given a fuzzy variable and a deffuzification method this returns a crisp value
    inline float deFuzzify(const std::string& key, DefuzzifyMethod method = max_av);
   
};

///////////////////////////////////////////////////////////////////////////////

//----------------------------- fuzzify ---------------------------------------
//
//  this method calls the fuzzify method of the variable with the same name as the key
//-----------------------------------------------------------------------------
inline void FuzzyModule::fuzzify(const std::string& NameOfFLV, float val)
{
    //first make sure the key exists
    assert ( (m_Variables.find(NameOfFLV) != m_Variables.end()) &&
                "<FuzzyModule::fuzzify>:key not found");

    m_Variables[NameOfFLV]->fuzzify(val);
}

//---------------------------- deFuzzify --------------------------------------
//
//  given a fuzzy variable and a deffuzification method this returns a crisp value
//-----------------------------------------------------------------------------
inline float FuzzyModule::deFuzzify(const std::string& NameOfFLV, DefuzzifyMethod method)
{
  //first make sure the key exists
  assert ( (m_Variables.find(NameOfFLV) != m_Variables.end()) &&
          "<FuzzyModule::deFuzzifyMaxAv>:key not found");

    //clear the DOMs of all the consequents of all the rules
    setConfidencesOfConsequentsToZero();

    //process the rules
    std::vector<FuzzyRule*>::iterator curRule = m_Rules.begin();
    for (curRule; curRule != m_Rules.end(); ++curRule)
    {
        (*curRule)->calculate();
    }

    //now defuzzify the resultant conclusion using the specified method
    switch (method)
    {
        case centroid:
            return m_Variables[NameOfFLV]->deFuzzifyCentroid(NumSamples);

        case max_av:
            return m_Variables[NameOfFLV]->deFuzzifyMaxAv();
    }

    return 0;
}



//-------------------------- ClearConsequents ---------------------------------
//
//  zeros the DOMs of the consequents of each rule
//-----------------------------------------------------------------------------
inline void FuzzyModule::setConfidencesOfConsequentsToZero()
{
    std::vector<FuzzyRule*>::iterator curRule = m_Rules.begin();
    for (curRule; curRule != m_Rules.end(); ++curRule)
    {
        (*curRule)->setConfidenceOfConsequentToZero();
    }
}


#endif