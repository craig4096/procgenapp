#pragma once

#define _USE_MATH_DEFINES
#include <math.h>
#include <ostream>


#define DEG_TO_RAD (M_PI/180.0f)
#define RAD_TO_DEG (180.0f/M_PI)

struct vec2
{
    float x, y;

    vec2();
    vec2(float x, float y);

    void normalize();

    vec2 operator-(const vec2& other) const;

    float dotProduct(const vec2& other) const;

    void print();
};

struct vec3
{
    float x, y, z;

    vec3();
    vec3(float x, float y, float z);
    vec3(float v[3]);

    vec3 operator +(const vec3& other) const;
    vec3 operator -(const vec3& other) const;
    vec3 operator *(float scalar) const;
    vec3 operator /(float scalar) const;

    vec3& operator +=(const vec3& other);
    vec3& operator -=(const vec3& other);

    vec3& operator *=(float scalar);
    vec3& operator /=(float scalar);


    // calculates the angle (in radians) between two vectors
    float calcAngle(const vec3& other);
    float dotProduct(const vec3& other);
    vec3 crossProduct(const vec3& other);
    float length() const;
    void normalize();
    vec3 normalized() const;
};

std::ostream& operator <<(std::ostream& cout, const vec3&);

struct BoundingBox
{
    vec3 min, max;

    bool intersects(const BoundingBox& bb) const;

    void addPoint(const vec3& point);
    void addBounds(const BoundingBox& bounds);
    bool pointInside(const vec3& point) const;
    BoundingBox translate(const vec3&) const;

    // gets the 8 corner points of the bounding box
    void getCorners(vec3[8]) const;

    bool inside(const BoundingBox& other) const;
};

float randomf();
vec3 CalculateNormal(const vec3& a, const vec3& b, const vec3& c);
vec3 CalculateViewDirection(float xRot, float yRot);

vec3 RotateVectorY(const vec3& v, float yRotation);
