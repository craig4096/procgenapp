
#include "ColorSpline.h"


void ColorSpline::AddColor(const Color& color) {
	colors.push_back(color);
}

Color ColorSpline::GetColor(float t) const {
	// assert(colors.size() >= 2)
	if(t >= 1.0f) {
		return colors[colors.size()-1];
	}
	if(t <= 0.0f) {
		return colors[0];
	}
	// use simple linear interpolation (for now)
	float pos = t * (colors.size()-1);
	int ipos = (int)pos;
	float fraction = pos - (float)ipos;

	return colors[ipos].lerp(colors[ipos+1], fraction);
}
