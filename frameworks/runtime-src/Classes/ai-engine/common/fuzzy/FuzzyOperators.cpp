#include "FuzzyOperators.h"
 
///////////////////////////////////////////////////////////////////////////////
//
//  implementation of FzAND
//
///////////////////////////////////////////////////////////////////////////////
FzAND::FzAND(const FzAND& fa)
{
    std::vector<FuzzyTerm*>::const_iterator curTerm;
    for (curTerm = fa.m_Terms.begin(); curTerm != fa.m_Terms.end(); ++curTerm)
    {
        m_Terms.push_back((*curTerm)->clone());
    }
}
   
  //ctor using two terms
FzAND::FzAND(FuzzyTerm& op1, FuzzyTerm& op2)
{
    m_Terms.push_back(op1.clone());
    m_Terms.push_back(op2.clone());
}

//ctor using three terms
FzAND::FzAND(FuzzyTerm& op1, FuzzyTerm& op2, FuzzyTerm& op3)
{
    m_Terms.push_back(op1.clone());
    m_Terms.push_back(op2.clone());
    m_Terms.push_back(op3.clone());
}

      //ctor using four terms
FzAND::FzAND(FuzzyTerm& op1, FuzzyTerm& op2, FuzzyTerm& op3, FuzzyTerm& op4)
{
    m_Terms.push_back(op1.clone());
    m_Terms.push_back(op2.clone());
    m_Terms.push_back(op3.clone());
    m_Terms.push_back(op4.clone());
}


FzAND::~FzAND()
{
    std::vector<FuzzyTerm*>::iterator curTerm;
    for (curTerm = m_Terms.begin(); curTerm != m_Terms.end(); ++curTerm)
    {
        delete *curTerm;
    }
}
  

//--------------------------- getDOM ------------------------------------------
//
//  the AND operator returns the minimum DOM of the sets it is operating on
//-----------------------------------------------------------------------------
float FzAND::getDOM()const
{
    float smallest = FloatMax;

    std::vector<FuzzyTerm*>::const_iterator curTerm;
    for (curTerm = m_Terms.begin(); curTerm != m_Terms.end(); ++curTerm)
    {
        if ((*curTerm)->getDOM() < smallest)
        {
            smallest = (*curTerm)->getDOM();
        }
    }

    return smallest;
}


//------------------------- orWithDOM -----------------------------------------
void FzAND::orWithDOM(float val)
{
    std::vector<FuzzyTerm*>::iterator curTerm;
    for (curTerm = m_Terms.begin(); curTerm != m_Terms.end(); ++curTerm)
    {
        (*curTerm)->orWithDOM(val);
    }
}

//---------------------------- clearDOM ---------------------------------------
void FzAND::clearDOM()
{
    std::vector<FuzzyTerm*>::iterator curTerm;
    for (curTerm = m_Terms.begin(); curTerm != m_Terms.end(); ++curTerm)
    {
        (*curTerm)->clearDOM();
    }
}

///////////////////////////////////////////////////////////////////////////////
//
//  implementation of FzOR
//
///////////////////////////////////////////////////////////////////////////////
FzOR::FzOR(const FzOR& fa)
{
    std::vector<FuzzyTerm*>::const_iterator curTerm;
    for (curTerm = fa.m_Terms.begin(); curTerm != fa.m_Terms.end(); ++curTerm)
    {
        m_Terms.push_back((*curTerm)->clone());
    }
}
   
  //ctor using two terms
FzOR::FzOR(FuzzyTerm& op1, FuzzyTerm& op2)
{
    m_Terms.push_back(op1.clone());
    m_Terms.push_back(op2.clone());
}

    //ctor using three terms
FzOR::FzOR(FuzzyTerm& op1, FuzzyTerm& op2, FuzzyTerm& op3)
{
    m_Terms.push_back(op1.clone());
    m_Terms.push_back(op2.clone());
    m_Terms.push_back(op3.clone());
}

      //ctor using four terms
FzOR::FzOR(FuzzyTerm& op1, FuzzyTerm& op2, FuzzyTerm& op3, FuzzyTerm& op4)
{
    m_Terms.push_back(op1.clone());
    m_Terms.push_back(op2.clone());
    m_Terms.push_back(op3.clone());
    m_Terms.push_back(op4.clone());
}


FzOR::~FzOR()
{
    std::vector<FuzzyTerm*>::iterator curTerm;
    for (curTerm = m_Terms.begin(); curTerm != m_Terms.end(); ++curTerm)
    {
        delete *curTerm;
    }
}
  

//--------------------------- getDOM ------------------------------------------
//
//  the OR operator returns the maximum DOM of the sets it is operating on
//----------------------------------------------------------------------------- 
float FzOR::getDOM()const
{
    float largest = FloatMin;

    std::vector<FuzzyTerm*>::const_iterator curTerm;
    for (curTerm = m_Terms.begin(); curTerm != m_Terms.end(); ++curTerm)
    {
        if ((*curTerm)->getDOM() > largest)
        {
            largest = (*curTerm)->getDOM();
        }
    }

    return largest;
}

