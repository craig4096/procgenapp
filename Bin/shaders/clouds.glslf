#version 330 core

#include "shaders/util.glslf"

uniform float time;

in vec3 pos;
in vec3 viewSpacePos;

void main()
{
    float speed =0.3;
    float n = fractal_noise(vec3(pos.x*0.05+time*speed, time*0.1, pos.z*0.05+time*speed), 7, 0.6);


    if(n < 0.55f)
    {
        gl_FragColor = vec4(0.0,0.0,0.0,0);
    }
    else
    {
        float a = (n-0.5)*2;
        vec4 color = mix(vec4(0.7,0.7,0.7,0), vec4(1,1,1,1), a);
        color.a = a;
        gl_FragColor =  color;
    }
}
