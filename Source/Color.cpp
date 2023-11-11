
#include "Color.h"

Color::Color()
    : red(0.0f), green(0.0f), blue(0.0f)
{
}

Color::Color(float red, float green, float blue)
    : red(red), green(green), blue(blue)
{
}

Color Color::lerp(const Color& o, float t) const
{
    return Color(
        red + (o.red - red) * t,
        green + (o.green - green) * t,
        blue + (o.blue - blue) * t);
}
