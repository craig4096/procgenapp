#version 330 core

#include "shaders/util.glslf"

uniform float time;

in vec3 pos;
in vec3 normal;
in vec3 viewSpacePos;

vec4 blendd(vec4 a, vec4 b, float r)
{
    return a + (b - a) * r;
}

void main()
{
    float c = 0.0f;
    vec4 color = vec4(0.0,0.3,0.0,1.0);

    int numOctaves = 5;
    vec3 n = normalize(normal);
    float dp = dot(n, vec3(0, 1, 0));
    float interp = getrange(0.7, 0.9, dp);
    color = blendd(vec4(0.4, 0.4, 0.4, 1), vec4(0,0.3,0,1), interp);
    c = fractal_noise(pos*0.4, numOctaves, 0.8);
    vec4 cc = mix(vec4(c,c,c,1) * color, color, interp);

    cc = ((cc * 0.3) + (cc * calc_lighting(n)));
    cc.a = 1.0;
    gl_FragColor = cc;
}