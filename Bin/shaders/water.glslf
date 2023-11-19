#version 330 core

#include "shaders/util.glslf"

uniform float time;

in vec3 pos;
in vec3 viewSpacePos;

void main()
{
    float speed =0.5;
    float freq = 1.5;
    float animSpeed = 0.5;
    int numOctaves = max(0, 6 - int(floor(length(viewSpacePos) / 100.0)));
    float n = fractal_noise(vec3(pos.x*freq+time*speed, time*animSpeed, pos.z*freq+time*speed), numOctaves, 0.6);
    if(n < 0.4f)
    {
        gl_FragColor = vec4(0,0.4,0.5,0.4);
    }
    else
    {
        gl_FragColor =  n * vec4(0,0.8,1,1);
    }
}
