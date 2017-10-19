
#ifndef VECTOR2D_H
#define VECTOR2D_H 
#include <math.h>
#include <memory>
#include "common/game/MathBase.h"


class Vector2D 
{
public:

    /**
     * The x coordinate.
     */
    float x;

    /**
     * The y coordinate.
     */
    float y;

    /**
     * Constructs a new vector initialized to all zeros.
     */
    Vector2D();

    /**
     * Constructs a new vector initialized to the specified values.
     *
     * @param xx The x coordinate.
     * @param yy The y coordinate.
     */
    Vector2D(float xx, float yy);
    
    /**
     * Constructs a vector that describes the direction between the specified points.
     *
     * @param p1 The first point.
     * @param p2 The second point.
     */
    Vector2D(const Vector2D& p1, const Vector2D& p2);


    /**
     * Destructor.
     */
    ~Vector2D();

    void zero(){x=0.0; y=0.0;}
    
    bool isZero() const;

    /**
     * Indicates whether this vector contains all ones.
     *
     * @return true if this vector contains all ones, false otherwise.
     */
    bool isOne() const;

    /**
     * Returns the angle (in radians) between the specified vectors.
     *
     * @param v1 The first vector.
     * @param v2 The second vector.
     * 
     * @return The angle between the two vectors (in radians).
     */
    static float angle(const Vector2D& v1, const Vector2D& v2);

    /**
     * Adds the elements of the specified vector to this one.
     *
     * @param v The vector to add.
     */
    void add(const Vector2D& v);

    /**
     * Adds the specified vectors and stores the result in dst.
     *
     * @param v1 The first vector.
     * @param v2 The second vector.
     * @param dst A vector to store the result in.
     */
    static void add(const Vector2D& v1, const Vector2D& v2, Vector2D* dst);

    /**
     * Clamps this vector within the specified range.
     *
     * @param min The minimum value.
     * @param max The maximum value.
     */
    void clamp(const Vector2D& min, const Vector2D& max);


    /**
     * Returns the distance between this vector and v.
     *
     * @param v The other vector.
     * 
     * @return The distance between this vector and v.
     * 
     * @see distanceSquared
     */
    float distance(const Vector2D& v) const;

    /**
     * Returns the squared distance between this vector and v.
     *
     * When it is not necessary to get the exact distance between
     * two vectors (for example, when simply comparing the
     * distance between different vectors), it is advised to use
     * this method instead of distance.
     *
     * @param v The other vector.
     * 
     * @return The squared distance between this vector and v.
     * 
     * @see distance
     */
    float distanceSquared(const Vector2D& v) const;

    /**
     * Returns the dot product of this vector and the specified vector.
     *
     * @param v The vector to compute the dot product with.
     * 
     * @return The dot product.
     */
    float dot(const Vector2D& v) const;

    /**
     * Returns the dot product between the specified vectors.
     *
     * @param v1 The first vector.
     * @param v2 The second vector.
     * 
     * @return The dot product between the vectors.
     */
    static float dot(const Vector2D& v1, const Vector2D& v2);

    /**
     * Returns positive if v2 is clockwise of this vector,
     * minus if anticlockwise (Y axis pointing down, X axis to right)
     */
    int sign(const Vector2D& v2) const;
      
    //returns the vector that is perpendicular to this one.
    Vector2D getPerp() const;

    //truncates a vector so that its length does not exceed max
    void truncate(float max);
    
    /**
     * Computes the length of this vector.
     *
     * @return The length of the vector.
     * 
     * @see lengthSquared
     */
    float length() const;

    /**
     * Returns the squared length of this vector.
     *
     * When it is not necessary to get the exact length of a
     * vector (for example, when simply comparing the lengths of
     * different vectors), it is advised to use this method
     * instead of length.
     *
     * @return The squared length of the vector.
     * 
     * @see length
     */
    float lengthSq() const;

    /**
     * Negates this vector.
     */
    void negate();

    /**
     * Normalizes this vector.
     *
     * This method normalizes this Vector2D so that it is of
     * unit length (in other words, the length of the vector
     * after calling this method will be 1.0f). If the vector
     * already has unit length or if the length of the vector
     * is zero, this method does nothing.
     * 
     * @return This vector, after the normalization occurs.
     */
    void normalize();

    /**
     Get the normalized vector.
     */
    Vector2D getNormalized() const;

    /**
     * Scales all elements of this vector by the specified value.
     *
     * @param scalar The scalar value.
     */
    void scale(float scalar);

    
    /**
     * Rotates this vector by angle (specified in radians) around the given point.
     *
     * @param point The point to rotate around.
     * @param angle The angle to rotate by (in radians).
     */
    void rotate(const Vector2D& point, float angle);


    /**
     * Subtracts this vector and the specified vector as (this - v)
     * and stores the result in this vector.
     *
     * @param v The vector to subtract.
     */
    void subtract(const Vector2D& v);

    /**
     * Subtracts the specified vectors and stores the result in dst.
     * The resulting vector is computed as (v1 - v2).
     *
     * @param v1 The first vector.
     * @param v2 The second vector.
     * @param dst The destination vector.
     */
    static void subtract(const Vector2D& v1, const Vector2D& v2, Vector2D* dst);

    /** Calculates midpoint between two points.
     @return Vector2D
     @since v3.0
     * @js NA
     * @lua NA
     */
    Vector2D getMidpoint(const Vector2D& other) const;

    //  returns the vector that is the reverse of this vector
    Vector2D getReverse() const;

    //  given a normalized vector this method reflects the vector it
    //  is operating upon. (like the path of a ball bouncing off a wall)
    void reflect(const Vector2D& norm);

    
    Vector2D operator+(const Vector2D &v);
    Vector2D operator-(const Vector2D &v);
    //Vector2D operator* (float s); 
    Vector2D operator/ (float s); 
    
    /**
     * Adds the given vector to this vector.
     * 
     * @param v The vector to add.
     * @return This vector, after the addition occurs.
     */
    Vector2D& operator+=(const Vector2D& v);


    /**
     * Subtracts the given vector from this vector.
     * 
     * @param v The vector to subtract.
     * @return This vector, after the subtraction occurs.
     */
    Vector2D& operator-=(const Vector2D& v);


    /**
     * Scales this vector by the given value.
     * 
     * @param s The value to scale by.
     * @return This vector, after the scale occurs.
     */
    Vector2D& operator*=(float s);

    Vector2D& operator/=(float s);

    /**
     * Determines if this vector is less than the given vector.
     * 
     * @param v The vector to compare against.
     * 
     * @return True if this vector is less than the given vector, false otherwise.
     */
    bool operator<(const Vector2D& v) const;
    
    /**
     * Determines if this vector is greater than the given vector.
     *
     * @param v The vector to compare against.
     *
     * @return True if this vector is greater than the given vector, false otherwise.
     */
    bool operator>(const Vector2D& v) const;

    /**
     * Determines if this vector is equal to the given vector.
     * 
     * @param v The vector to compare against.
     * 
     * @return True if this vector is equal to the given vector, false otherwise.
     */
    bool operator==(const Vector2D& v) const;

    bool operator!=(const Vector2D& rhs)const;
};


//overload the - operator
inline Vector2D operator-(const Vector2D &lhs, const Vector2D &rhs)
{
    Vector2D result(lhs);
    result.x -= rhs.x;
    result.y -= rhs.y;
    
    return result;
}


inline Vector2D operator*(const Vector2D &v, float f)
{
    Vector2D result(v);
    result *= f;
    return result;
}


inline Vector2D operator*(float f, const Vector2D &v)
{
    Vector2D result(v);
    result *= f;
    return result;
}


inline Vector2D Vec2Normalize(const Vector2D &v)
{
    Vector2D vec = v;

    return vec.getNormalized();
}

inline float Vec2Distance(const Vector2D &v1, const Vector2D &v2)
{
    float ySeparation = v2.y - v1.y;
    float xSeparation = v2.x - v1.x;

    return std::sqrt(ySeparation*ySeparation + xSeparation*xSeparation);
}

inline float Vec2DistanceSq(const Vector2D &v1, const Vector2D &v2)
{
    float ySeparation = v2.y - v1.y;
    float xSeparation = v2.x - v1.x;
    
    return ySeparation*ySeparation + xSeparation*xSeparation;
}

enum {
    clockwise = 1, 
    anticlockwise = -1
};

#endif 

