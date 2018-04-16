#ifndef PROBABILITYFUNCTION_H
#define PROBABILITYFUNCTION_H
#include "math3d.h"

class ProbabilityFunction {
public:
    virtual ~ProbabilityFunction(){}
    // returns the probability of a rule being selected depending
    // on the position of the lhs shape
    virtual float getProbability(const vec3& lhsPos) = 0;
};

#endif // PROBABILITYFUNCTION_H
