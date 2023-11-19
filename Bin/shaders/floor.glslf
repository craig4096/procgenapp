#version 330 core

in vec2 texCoord;

#define real float

const real TILE_WIDTH = 1.0f/50.0;
const real TILE_HEIGHT = 1.0f/16.0;

// intergral function of a set of strips
real integral1(real s)
{
    real ss = s / TILE_WIDTH;
    // get the strip index
    real index = floor(ss);
    real base = index*(TILE_WIDTH*0.5);
    real relratio = ss - index;
    real sint;
    if(relratio <= 0.5)
    {
        sint = base;
    }
    else
    {
        sint = base + ((TILE_WIDTH*0.5) * ((relratio-0.5)*2.0));
    }
    return sint;
}

real integral(real s, real t)
{
    return integral1(s) * t;
}

void main()
{
    vec2 st = texCoord;
    // st change for on pixel in x
    vec2 dvdx = dFdx(st);
    // st change for one pixel in y
    vec2 dvdy = dFdy(st);

    // filter size in s
    float sfs =0.006;// max(abs(dvdx.s), abs(dvdy.s)) * 0.5;
    // filter size in t
    float tfs = sfs;//max(abs(dvdx.t), abs(dvdy.t)) * 0.5;

    real a = integral(st.s - sfs, st.t - tfs);
    real b = integral(st.s + sfs, st.t - tfs);
    real c = integral(st.s - sfs, st.t + tfs);
    real d = integral(st.s + sfs, st.t + tfs);
    real color = (d + a - b - c) / ((sfs*2) * (tfs*2));
    gl_FragColor = vec4(color, color, color, 1);
}

