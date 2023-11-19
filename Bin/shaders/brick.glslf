#version 330
#include "shaders/util.glslf"

uniform float time;

in vec3 pos;
in vec3 normal;
in vec3 viewSpacePos;
in vec2 texCoord;

const float BM_WIDTH = 0.35;
const float BM_HEIGHT = 0.25;

const float MORTAR_THICKNESS = 0.1;
const float MMW = MORTAR_THICKNESS * 0.5;
const float MMH = MORTAR_THICKNESS * 0.5;

float pulse(float a, float b, float x)
{
    return step(a, x) - step(b, x);
}

const vec4 CMortar = vec4(0.5, 0.5, 0.5, 1);
const vec4 CBrick = vec4(0.5, 0.15, 0.14, 1);

vec4 brick_texture(vec2 coord)
{
    float rs, rt, bis, bit, w, h;

    rs = coord.s / BM_WIDTH;
    rt = coord.t / BM_HEIGHT;

    if(mod(rt*0.5, 1) > 0.5)
    {
        rs += 0.5;
    }

    bis = floor(rs);
    bit = floor(rt);

    rs = rs - bis;
    rt = rt - bit;

    w = pulse(MMW, 1-MMW, rs);
    h = pulse(MMH, 1-MMH, rt);

    vec4 color = mix(CMortar, CBrick, w*h);

    return color;
}

void main()
{
    vec4 color = fractal_noise(pos, 8, 0.8) * brick_texture(texCoord);

    color = (color * 0.3 + (color * calc_lighting(normal)));
    color.a = 1;
    gl_FragColor = color;
}