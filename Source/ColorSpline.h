
#pragma once
#include <vector>
#include "Color.h"

// maps a scalar value between (0 and 1) to a color
class ColorSpline
{
    std::vector<Color> colors;
public:
    void AddColor(const Color& color);
    // returns a color given a value between 0 and 1
    Color GetColor(float t) const;
};
