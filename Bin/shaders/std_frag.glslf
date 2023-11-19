
#version 330 core

#include "shaders/util.glslf"

in vec3 pos;
in vec3 viewSpacePos;
in vec3 normal;
in vec2 texCoord;

void main()
{
    float d = calc_lighting(normal);

    gl_FragColor = vec4(d, d, d, 1.0);
}
