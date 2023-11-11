#ifndef CONSTPROBFUNC_H
#define CONSTPROBFUNC_H
#include "ProbabilityFunction.h"

class ConstProbFunc : public ProbabilityFunction
{
    float value;
public:
    ConstProbFunc(float value);
    float getProbability(const vec3& pos);
};

#endif // CONSTPROBFUNC_H
