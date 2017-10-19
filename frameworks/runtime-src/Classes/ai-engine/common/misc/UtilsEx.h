#ifndef UTILS_EX_H
#define UTILS_EX_H 

#include <cstdlib>
#include <sstream>
#include <string>
#include <iomanip>
#include <cassert>

#ifndef _PI_
#define _PI_ (3.14159f)
#endif

//const int IntMax = (std::numeric_limits<int>::max)();
//const double DoubleMax = (std::numeric_limits<double>::max)();
//const double DoubleMin = (std::numeric_limits<double>::min)();
const float FloatMax = (std::numeric_limits<float>::max)();
const float FloatMin = (std::numeric_limits<float>::min)();


/**
 * Returns a random float between -1 and 1.
 * It can be seeded using std::srand(seed);
 */
inline float RandFloat_minus1_1() 
{
    // FIXME: using the new c++11 random engine generator
    // without a proper way to set a seed is not useful.
    // Resorting to the old random method since it can
    // be seeded using std::srand()
    return ((std::rand() / (float)RAND_MAX) * 2) -1;

//    return cocos2d::random(-1.f, 1.f);
};


/**
 * Returns a random float between 0 and 1.
 * It can be seeded using std::srand(seed);
 */
inline float RandFloat_0_1() 
{
    // FIXME: using the new c++11 random engine generator
    // without a proper way to set a seed is not useful.
    // Resorting to the old random method since it can
    // be seeded using std::srand()
    return std::rand() / (float)RAND_MAX;

//    return cocos2d::random(0.f, 1.f);
};

//returns a random bool
inline bool RandBool()
{
    if (RandFloat_0_1() > 0.5)
        return true;
    else 
        return false;
}

inline float RandFloatInRange(float x, float y)
{
    return x + RandFloat_0_1()*(y-x);
}

inline int RandIntInRange(int x, int y)
{
    assert(y>=x && "<RandInt>: y is less than x");
    return rand()%(y-x+1)+x;    
}

//compares two real numbers. Returns true if they are equal
inline bool isEqual(float a, float b)
{
    if (fabs(a-b) < 1E-12)
    {
        return true;
    }

    return false;
}

inline float degreeToRadians(float degree)
{
    return degree * 0.01745329252f; // PI / 180
}

inline float radiansToDegree(float rads)
{
    return rads * 57.29577951f; // PI * 180
}


//clamps the first argument between the second two
template <class T, class U, class V>
inline void clamp(T& arg, const U& minVal, const V& maxVal)
{
    if (arg < (T)minVal)
    {
        arg = (T)minVal;
    }

    if (arg > (T)maxVal)
    {
        arg = (T)maxVal;
    }
}

//------------------------------ ttos -----------------------------------------
//
//  convert a type to a string
//-----------------------------------------------------------------------------
template <class T>
inline std::string ttos(const T& t, int precision = 2)
{
    std::ostringstream buffer;

    buffer << std::fixed << std::setprecision(precision) << t;

    return buffer.str();
}

#endif 

