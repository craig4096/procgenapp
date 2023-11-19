uniform mat4 viewMat;
uniform mat4 normalMatrix;

float rand(vec2 co)
{
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

float rand(vec3 co)
{
    return fract(sin(dot(co.xyz ,vec3(12.9898,78.233,102.128))) * 43758.5453);
}

float smooth_rand(vec2 co)
{
    vec2 a = floor(co);
    vec2 b = a+vec2(1,0);
    vec2 c = a+vec2(0,1);
    vec2 d = a+vec2(1,1);

    float ra = rand(a);
    float rb = rand(b);
    float rc = rand(c);
    float rd = rand(d);
    float xratio = co.x - a.x;
    float yratio = co.y - a.y;

    float ab = ra + (rb - ra) * xratio;
    float cd = rc + (rd - rc) * xratio;
    return ab + (cd - ab) * yratio;
}

float smooth_rand(vec3 co)
{
    // bottom points of grid cell
    vec3 a1 = floor(co);
    float ra1 = rand(a1);
    float rb1 = rand(a1+vec3(1,0,0));
    float rc1 = rand(a1+vec3(0,0,1));
    float rd1 = rand(a1+vec3(1,0,1));

    // top points of grid cell
    float ra2 = rand(a1+vec3(0,1,0));
    float rb2 = rand(a1+vec3(1,1,0));
    float rc2 = rand(a1+vec3(0,1,1));
    float rd2 = rand(a1+vec3(1,1,1));

    // tri-linear interpolation
    float xratio = co.x - a1.x;
    float zratio = co.z - a1.z;
    float yratio = co.y - a1.y;

    // find the bottom noise value
    float a1b1 = ra1 + (rb1 - ra1) * xratio;
    float c1d1 = rc1 + (rd1 - rc1) * xratio;
    float bottom = a1b1 + (c1d1 - a1b1) * zratio;

    // find the top noise value
    float a2b2 = ra2 + (rb2 - ra2) * xratio;
    float c2d2 = rc2 + (rd2 - rc2) * xratio;
    float top = a2b2 + (c2d2 - a2b2) * zratio;

    return bottom + (top - bottom) * yratio;
}

float fractal_noise(vec3 pos, int numOctaves, float ampDec)
{
    float noise = 0.0f;
    float amplitude = 0.5f;
    float freq = 2.0f;
    for(int i = 0; i < numOctaves; ++i)
    {
        noise += smooth_rand(pos*freq) * amplitude;
        amplitude *= ampDec;
        freq *= 2.0f;
    }
    return noise;
}

float calc_lighting(vec3 normal)
{
    vec3 n = vec3(normalMatrix * vec4(normal, 1.0));
    vec3 l = vec3(viewMat * vec4(1,0.5,0.7,0));

    return max(0.0f, dot(normalize(n), normalize(l)));
}

float getrange(float a, float b, float x)
{
    if(x < a) return 0.0;
    if(x > b) return 1.0;
    return (x - a) / (b - a);
}


float getFog(vec3 viewSpacePos)
{
    return 1.0f-getrange(0.0, 20.0, length(viewSpacePos));
}

