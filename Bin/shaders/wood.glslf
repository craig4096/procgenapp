
#version 330 core

#include "shaders/util.glslf"

uniform float time;
varying vec3 pos;
varying vec3 normal;
varying vec3 viewSpacePos;

void main()
{
    float c = 0.0f;

    c = fractal_noise(pos*0.5, 6, 0.8);
    vec4 cc =  vec4(c,c,c,1) * vec4(0.1, 0.05,0.0,1);

    cc = ((cc * 0.3) + (cc * calc_lighting(normal)));
    cc.a = 1.0;
    gl_FragColor = cc;
}
