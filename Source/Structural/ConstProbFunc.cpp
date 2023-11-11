#include "ConstProbFunc.h"

ConstProbFunc::ConstProbFunc(float value)
    : value(value)
{
}

float ConstProbFunc::getProbability(const vec3& pos)
{
    return value;
}
