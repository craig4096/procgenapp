
#include "shaders/util.glslf"

uniform float time;
varying vec3 pos;
varying vec3 normal;
varying vec3 viewSpacePos;

void main() {
	float c = 0.0f;
/*
	float freq = 2.0f;

	float swidth = dFdx(gl_TexCoord[0].st.s);
	float sampleRate = 1.0f/swidth;

	int maxfreq =5;
	for(int i = 0; i < maxfreq; ++i) {
		c += smooth_rand(pos*freq);
		freq *= 2.0;
	}
	c /= (float)maxfreq;
*/
	c = fractal_noise(pos*0.5, 6, 0.8);
	vec4 cc =  vec4(c,c,c,1) * vec4(0.4, 0.4,0.4,1);
//vec4(c,c,c,1) *

	cc = ((cc * 0.3) + (cc * calc_lighting(normal)));
	cc.a = 1.0;
	gl_FragColor = cc;
}
