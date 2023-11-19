
#version 330 core

#include "shaders/util.glslf"

uniform float time;

in vec3 pos;
in vec3 normal;
in vec3 viewSpacePos;

void main()
{
    float c = 0.0f;
    c = fractal_noise(pos*0.5, 6, 0.8);
    vec4 cc =  vec4(c,c,c,1) * vec4(0.4, 0.4,0.4,1);

    cc = ((cc * 0.3) + (cc * calc_lighting(normal)));
    cc.a = 1.0;
    gl_FragColor = cc;
}