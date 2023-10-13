

#include "math3d.h"
#include <iostream>
#include <cstdlib>
using namespace std;

///// vec2 

vec2::vec2()
    : x(0.0f), y(0.0f)
{
}

vec2::vec2(float x, float y)
    : x(x), y(y)
{
}


void vec2::normalize() {
    float mag = sqrt(x*x+y*y);
    if(mag != 0.0f) {
        x /= mag;
        y /= mag;
    } else {
        x = y = 0.0f;
    }
}

void vec2::print() {
    cout << "vec2(" << x << ", " << y << ")" << endl;
}

vec2 vec2::operator-(const vec2& other) const {
    return vec2(x - other.x, y - other.y);
}

float vec2::dotProduct(const vec2& other) const {
    return x*other.x+y*other.y;
}

///// vec3 ////////////////////////////////////


vec3::vec3()
    : x(0.0f), y(0.0f), z(0.0f)
{
}

vec3::vec3(float x, float y, float z)
    : x(x), y(y), z(z)
{
}

vec3::vec3(float v[3])
    : x(v[0]), y(v[1]), z(v[2])
{
}

vec3 vec3::operator +(const vec3& o) const
{
    return vec3(x + o.x, y + o.y, z + o.z);
}

vec3 vec3::operator -(const vec3& o) const
{
    return vec3(x - o.x, y - o.y, z - o.z);
}

vec3 vec3::operator *(float s) const
{
    return vec3(x * s, y * s, z * s);
}

vec3 vec3::operator /(float scalar) const
{
    float recip = 1.0f / scalar;
    return vec3(x * recip, y * recip, z * recip);
}

vec3& vec3::operator +=(const vec3& o)
{
    x += o.x;
    y += o.y;
    z += o.z;
    return *this;
}

vec3& vec3::operator -=(const vec3& o)
{
    x -= o.x;
    y -= o.y;
    z -= o.z;
    return *this;
}

vec3& vec3::operator *=(float scalar)
{
    x *= scalar;
    y *= scalar;
    z *= scalar;
    return *this;
}

vec3& vec3::operator /=(float scalar)
{
    float recip = 1.0f / scalar;
    x *= recip;
    y *= recip;
    z *= recip;
    return *this;
}


float vec3::calcAngle(const vec3& o) {
    vec3 a(this->normalized());
    vec3 b(o.normalized());

    return acos(a.dotProduct(b));
    //this->dotProduct(o) / (this->length() + o.length())
}

float vec3::dotProduct(const vec3& o)
{
    return x * o.x + y * o.y + z * o.z;
}

vec3 vec3::crossProduct(const vec3& o)
{
    return vec3((y * o.z) - (o.y * z),
            (z * o.x) - (o.z * x),
            (x * o.y) - (o.x * y));
}

float vec3::length() const
{
    return sqrt(x*x + y*y + z*z);
}

void vec3::normalize()
{
    float mag = sqrt(x*x + y*y + z*z);
    if(mag == 0.0f) return;
    float recip = 1.0f / mag;
    x *= recip;
    y *= recip;
    z *= recip;
}

vec3 vec3::normalized() const
{
    float mag = sqrt(x*x + y*y + z*z);
    if(mag == 0.0f) return vec3(0,0,0);
    float recip = 1.0f / mag;
    return vec3(x * recip, y * recip, z * recip);
}

ostream& operator <<(ostream& cout, const vec3& v) {
    cout << "(" << v.x << "," << v.y << "," << v.z << ")";
    return cout;
}

//////////////////////////////////////////////


bool BoundingBox::intersects(const BoundingBox& other) const {
    return max.x >= other.min.x && min.x <= other.max.x &&
        max.y >= other.min.y && min.y <= other.max.y &&
        max.z >= other.min.z && min.z <= other.max.z;
}

void BoundingBox::addPoint(const vec3& p) {
    if(p.x < min.x) min.x = p.x;
    if(p.x > max.x) max.x = p.x;
    if(p.y < min.y) min.y = p.y;
    if(p.y > max.y) max.y = p.y;
    if(p.z < min.z) min.z = p.z;
    if(p.z > max.z) max.z = p.z;
}

bool BoundingBox::pointInside(const vec3& point) const {
    return point.x >= min.x && point.x <= max.x &&
        point.y >= min.y && point.y <= max.y &&
        point.z >= min.z && point.z <= max.z;
}

void BoundingBox::addBounds(const BoundingBox& bounds) {
    addPoint(bounds.min);
    addPoint(bounds.max);
}


BoundingBox BoundingBox::translate(const vec3& v) const {
    BoundingBox b;
    b.min = min + v;
    b.max = max + v;
    return b;
}

void BoundingBox::getCorners(vec3 corners[8]) const {
    corners[0] = vec3(min.x, min.y, min.z);
    corners[1] = vec3(min.x, min.y, max.z);
    corners[2] = vec3(min.x, max.y, min.z);
    corners[3] = vec3(min.x, max.y, max.z);

    corners[4] = vec3(max.x, min.y, min.z);
    corners[5] = vec3(max.x, min.y, max.z);
    corners[6] = vec3(max.x, max.y, min.z);
    corners[7] = vec3(max.x, max.y, max.z);
}

bool BoundingBox::inside(const BoundingBox& other) const {
    return min.x >= other.min.x &&
            min.y >= other.min.y &&
            min.z >= other.min.z &&
            max.x <= other.max.x &&
            max.y <= other.max.y &&
            max.z <= other.max.z;
}

//////////////////////////////////////////////


vec3 CalculateNormal(const vec3& a, const vec3& b, const vec3& c) {
    return (b - a).crossProduct(c - a);
}

vec3 CalculateViewDirection(float xRot, float yRot)
{
    vec3 viewDir;
    // x and z can be calculated from the y axis
    // rotation using sin and cos
    viewDir.x = sin(yRot * DEG_TO_RAD);
    viewDir.z = cos(yRot * DEG_TO_RAD);

    // y can be calculated from the
    // x rotation using cos
    viewDir.y = cos(xRot * DEG_TO_RAD);

    if(viewDir.y > 0.0f)
    {
        viewDir.x *= (1.0f-viewDir.y);
        viewDir.z *= (1.0f-viewDir.y);
    }
    else
    {
        viewDir.x *= (1.0f+viewDir.y);
        viewDir.z *= (1.0f+viewDir.y);
    }
    return viewDir;
}

float randomf() {
   return rand()/(float)RAND_MAX;
}

vec3 RotateVectorY(const vec3& v, float yRotation) {
    float cosa = cos(yRotation * DEG_TO_RAD);
    float sina = sin(yRotation * DEG_TO_RAD);

    return vec3(cosa * v.x + sina * v.z, v.y, -sina * v.x + cosa * v.z);
}

