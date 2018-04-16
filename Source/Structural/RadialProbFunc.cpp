#include "RadialProbFunc.h"

RadialProbFunc::RadialProbFunc(const vec3& pos, float minProb, float maxProb, float radius)
    : position(pos)
    , minProb(minProb)
    , maxProb(maxProb)
    , radius(radius)
{
}

float RadialProbFunc::getProbability(const vec3& pos) {
    // get distance from 'position'
    float dist = (vec3(pos.x, 0, pos.z) - vec3(position.x, 0, position.z)).length();
    dist = std::max(0.0f, 1 - dist/radius);
    return minProb + (maxProb - minProb) * dist;
}
