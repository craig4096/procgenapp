
#pragma once

class Color {
public:
    float red, green, blue;
    Color();
    Color(float red, float green, float blue);

    Color lerp(const Color& other, float t) const;
};
