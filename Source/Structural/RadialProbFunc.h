#ifndef RADIALPROBFUNC_H
#define RADIALPROBFUNC_H
#include "ProbabilityFunction.h"

class RadialProbFunc : public ProbabilityFunction
{
    float minProb, maxProb;
    float radius;
    vec3 position;
public:
    RadialProbFunc(const vec3& pos, float minProb, float maxProb, float radius);
    float getProbability(const vec3& pos);
};

#endif // RADIALPROBFUNC_H
