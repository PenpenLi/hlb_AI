
#include "Vector2D.h"



Vector2D::Vector2D(): x(0.0f), y(0.0f)
{
}

Vector2D::Vector2D(float xx, float yy): x(xx), y(yy)
{
}

Vector2D::Vector2D(const Vector2D& p1, const Vector2D& p2)
{    
    x = p2.x - p1.x;    
    y = p2.y - p1.y;    
}

Vector2D::~Vector2D()
{
}

bool Vector2D::isZero() const
{    
    return x == 0.0f && y == 0.0f;
}

bool Vector2D::isOne() const
{    
    return x == 1.0f && y == 1.0f;
}

float Vector2D::angle(const Vector2D& v1, const Vector2D& v2)
{
    float dz = v1.x * v2.y - v1.y * v2.x;
    return atan2f(fabsf(dz) + MATH_FLOAT_SMALL, dot(v1, v2));
}

void Vector2D::add(const Vector2D& v)
{ 
    x += v.x;    
    y += v.y;
}

void Vector2D::add(const Vector2D& v1, const Vector2D& v2, Vector2D* dst)
{
    dst->x = v1.x + v2.x;
    dst->y = v1.y + v2.y;
}

void Vector2D::clamp(const Vector2D& min, const Vector2D& max)
{
    // Clamp the x value.
    if (x < min.x)
        x = min.x;
    if (x > max.x)
        x = max.x;

    // Clamp the y value.
    if (y < min.y)
        y = min.y;
    if (y > max.y)
        y = max.y;
}

float Vector2D::distance(const Vector2D& v) const
{
    float dx = v.x - x;
    float dy = v.y - y;

    return std::sqrt(dx * dx + dy * dy);
}

float Vector2D::distanceSquared(const Vector2D& v) const
{   
    float dx = v.x - x;    
    float dy = v.y - y;   
    return (dx * dx + dy * dy);
}

float Vector2D::dot(const Vector2D& v) const
{    
    return (x * v.x + y * v.y);
}

float Vector2D::dot(const Vector2D& v1, const Vector2D& v2)
{
    return (v1.x * v2.x + v1.y * v2.y);
}

int Vector2D::sign(const Vector2D& v2) const
{
    if (y*v2.x > x*v2.y)
    { 
        return anticlockwise;
    }
    else 
    {
        return clockwise;
    } 
}

Vector2D Vector2D::getPerp() const 
{
    return Vector2D(-y, x);
}

void Vector2D::truncate(float max)
{
    if (this->length() > max) 
    {
        this->normalize();
        *this *= max;
    } 
}

float Vector2D::length() const
{
    return std::sqrt(x * x + y * y);
}

float Vector2D::lengthSq() const
{    
    return (x * x + y * y);
}

void Vector2D::negate()
{   
    x = -x;    
    y = -y;
}

void Vector2D::normalize()
{
    float n = x * x + y * y;
    // Already normalized.
    if (n == 1.0f)
        return;
    
    n = std::sqrt(n);
    // Too close to zero.
    if (n < MATH_TOLERANCE)
        return;
    
    n = 1.0f / n;
    x *= n;
    y *= n;
}

Vector2D Vector2D::getNormalized() const
{
    Vector2D v(x, y);
    v.normalize();
    return v;
}

void Vector2D::scale(float scalar)
{   
    x *= scalar;    
    y *= scalar;
}



void Vector2D::rotate(const Vector2D& point, float angle)
{
    float sinAngle = std::sin(angle);
    float cosAngle = std::cos(angle);

    if (point.isZero())
    {
        float tempX = x * cosAngle - y * sinAngle;
        y = y * cosAngle + x * sinAngle;
        x = tempX;
    }
    else
    {
        float tempX = x - point.x;
        float tempY = y - point.y;

        x = tempX * cosAngle - tempY * sinAngle + point.x;
        y = tempY * cosAngle + tempX * sinAngle + point.y;
    }
}

void Vector2D::subtract(const Vector2D& v)
{    
    x -= v.x;    
    y -= v.y;
}

void Vector2D::subtract(const Vector2D& v1, const Vector2D& v2, Vector2D* dst)
{
    dst->x = v1.x - v2.x;
    dst->y = v1.y - v2.y;
}

Vector2D Vector2D::getMidpoint(const Vector2D& other) const
{
    return Vector2D((x + other.x) / 2.0f, (y + other.y) / 2.0f);
}

Vector2D Vector2D::getReverse() const
{
    return Vector2D(-this->x, -this->y);
}

void Vector2D::reflect(const Vector2D& norm)
{
    *this += norm.getReverse() * 2.0 * this->dot(norm);
}

Vector2D Vector2D::operator+(const Vector2D &v)
{
    Vector2D result(*this);
    result.add(v);
    
    return result;
}

Vector2D Vector2D::operator-(const Vector2D &v)
{
    Vector2D result(*this);    
    result.subtract(v); 
    
    return result;
}

/*
Vector2D Vector2D::operator*(float s)
{
    Vector2D result(*this); 
    result.scale(s); 
    
    return result;
}
*/

Vector2D Vector2D::operator/(float s)
{
    return Vector2D(this->x / s, this->y / s);
}

Vector2D& Vector2D::operator+=(const Vector2D& v)
{    
    add(v);    
    return *this;
}

Vector2D& Vector2D::operator-=(const Vector2D& v)
{    
    subtract(v);    
    return *this;
}

Vector2D& Vector2D::operator*=(float s)
{    
    scale(s);    
    return *this;
}

Vector2D& Vector2D::operator/=(float s)
{    
    x /= s;
    y /= s;
    return *this;
}

bool Vector2D::operator<(const Vector2D& v) const
{    
    if (x == v.x)    
    {        
        return y < v.y;    
    }   
    return x < v.x;
}

bool Vector2D::operator>(const Vector2D& v) const
{   
    if (x == v.x) 
    {        
        return y > v.y;   
    }   
    return x > v.x;
}

bool Vector2D::operator==(const Vector2D& v) const
{    
    return x==v.x && y==v.y;
}

bool Vector2D::operator!=(const Vector2D& rhs)const
{
    return (x != rhs.x) || (y != rhs.y);
}



