#pragma warning (disable:4786)
#include <stdarg.h>
#include <cassert>

#include "common/fuzzy/FuzzyModule.h"

//------------------------------ dtor -----------------------------------------
FuzzyModule::~FuzzyModule()
{
    VarMap::iterator curVar = m_Variables.begin();
    for (curVar; curVar != m_Variables.end(); ++curVar)
    {
        delete curVar->second;
    }

    std::vector<FuzzyRule*>::iterator curRule = m_Rules.begin();
    for (curRule; curRule != m_Rules.end(); ++curRule)
    {
        delete *curRule;
    }
}

//----------------------------- addRule ---------------------------------------
void FuzzyModule::addRule(FuzzyTerm& antecedent, FuzzyTerm& consequence)
{
    m_Rules.push_back(new FuzzyRule(antecedent, consequence));
}
 
//-------------------------- createFLV ---------------------------
//
//  creates a new fuzzy variable and returns a reference to it
//-----------------------------------------------------------------------------
FuzzyVariable& FuzzyModule::createFLV(const std::string& VarName)
{
    m_Variables[VarName] = new FuzzyVariable();;

    return *m_Variables[VarName];
}

