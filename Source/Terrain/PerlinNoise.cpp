#include "PerlinNoise.h"
#include <iostream>
#include <wx/stopwatch.h>
#include <cstdlib>
using namespace std;

namespace terrain_ops
{
    PerlinNoise::PerlinNoise()
        : persistance(0.5f)
        , startOctave(2)
        , endOctave(8)
        , seed(0)
    {
        initializeGradientTable();
    }

    float PerlinNoise::RandomFloat()
    {
        return (rand() / (float)RAND_MAX) * 2.0f - 1.0f;
    }

    void PerlinNoise::initializeGradientTable()
    {
        wxStopWatch timer;
        timer.Start();
        srand(seed);
        for (int x = 0; x < GRADIENT_TABLE_SIZE; ++x)
        {
            for (int y = 0; y < GRADIENT_TABLE_SIZE; ++y)
            {
                vec2 tmp;
                tmp.x = RandomFloat();
                tmp.y = RandomFloat();
                tmp.normalize();
                gradientTable[x][y] = tmp;
            }
        }
        cout << "Perlin Noise Permutation Table Generation Time(Ms): " << timer.Time() << endl;
    }

#define PERLIN_OPTIMIZED

    vec2 PerlinNoise::GetGradientVector(int x, int y)
    {
#ifndef PERLIN_OPTIMIZED
        // unoptimized version
        srand(seed + ((short)x << 16 | (short)y));
        vec2 rval;
        rval.x = RandomFloat();
        rval.y = RandomFloat();
        rval.normalize();
        return rval;
#else
        return gradientTable[x % GRADIENT_TABLE_SIZE][y % GRADIENT_TABLE_SIZE];
#endif
    }

    float lerp(float a, float b, float t)
    {
        //return a + (b - a) * t;
        return a - (((3 * (t * t)) - 2 * (t * t * t)) * (a - b));
    }

    float PerlinNoise::GenerateNoise(float x, float y)
    {
        vec2 point(x, y);

        // get the 4 surrounding integer coordinates
        int left = floor(x);
        int right = left + 1;
        int bottom = floor(y);
        int top = bottom + 1;

        vec2 gradients[4] = {
            GetGradientVector(left, bottom),
            GetGradientVector(right, bottom),
            GetGradientVector(right, top),
            GetGradientVector(left, top)
        };

        // calculate the noise values at each corner (G.(P-Q))
        float noise[4] = {
            gradients[0].dotProduct(point - vec2(left, bottom)),
            gradients[1].dotProduct(point - vec2(right, bottom)),
            gradients[2].dotProduct(point - vec2(right, top)),
            gradients[3].dotProduct(point - vec2(left, top))
        };

        // now use bi-linear interpolation to interpolate the values
        float xfraction = x - (float)left;
        float yfraction = y - (float)bottom;
        // interpolate in x
        float a = lerp(noise[0], noise[1], xfraction);
        float b = lerp(noise[3], noise[2], xfraction);
        return lerp(a, b, yfraction);
    }

    float PerlinNoise::GenerateHeightValue(float x, float y)
    {
        float height = 0.0f;
        for (int i = startOctave; i < endOctave; ++i)
        {
            float frequency = pow(2, i);
            float amplitude = pow(persistance, i);
            height += GenerateNoise(x * frequency, y * frequency) * amplitude;
        }
        return height;
    }


    void PerlinNoise::operate(HeightmapStack* stack, Progress* progress)
    {
        wxStopWatch timer;
        timer.Start();
        // create a new heightmap
        Heightmap* h = stack->createNewHeightmap();

        progress->setPercent(0.0f);

        for (int x = 0; x < h->GetWidth(); ++x)
        {
            for (int y = 0; y < h->GetHeight(); ++y)
            {
                (*h)(x, y) = GenerateHeightValue(x / (float)h->GetWidth(), y / (float)h->GetHeight());
            }
            progress->setPercent((x + 1) / (float)h->GetWidth() * 100.0f);
        }
        //h->Normalize(0, 1);
        cout << "Perlin Noise Total Time(Ms): " << timer.Time() << endl;

        stack->push(h);
    }

    void PerlinNoise::inspect(IPropertyInspector& inspector)
    {
        inspector.property("Start Octave", startOctave);
        inspector.property("End Octave", endOctave);
        inspector.property("Persistance", persistance); // range 0-1
        inspector.property("Seed", seed);
    }

    void PerlinNoise::setStartOctave(int startOctave)
    {
        this->startOctave = startOctave;
    }

    void PerlinNoise::setEndOctave(int endOctave)
    {
        this->endOctave = endOctave;
    }

    void PerlinNoise::setPersistance(float persistance)
    {
        this->persistance = persistance;
    }

    void PerlinNoise::setSeed(int seed)
    {
        this->seed = seed;
        // need to create a new gradient table for a new seed
        initializeGradientTable();
    }
}
