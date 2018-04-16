#ifndef PERLINNOISE_H
#define PERLINNOISE_H

#include "math3d.h"
#include "TerrainOperation.h"

namespace terrain_ops {

class PerlinNoise : public TerrainOperation {
    int     startOctave;
    int     endOctave;
    float	persistance;
    int     seed;

    // a static look up table used for optimization - to avoid
    // calculating psudo-random gradient vectors at every iteration
    static const int GRADIENT_TABLE_SIZE = 256;
    vec2 gradientTable[GRADIENT_TABLE_SIZE][GRADIENT_TABLE_SIZE];

    // given a x and y coord this function will generate a random
    // gradient vector at x and y
    vec2 GetGradientVector(int x, int y);

    // generates a random float between -1 and 1
    static float RandomFloat();

    void initializeGradientTable();

public:
    PerlinNoise();

    // generates a psudo-random float value at a specific (x,y) location
    float GenerateNoise(float x, float y);

    // fractally generates the final height-value taking into account the
    // number of octaves and persistance parameters
    float GenerateHeightValue(float x, float y);

    // generates a heightmap and pushes onto stack
    void operate(HeightmapStack*, Progress*);

    void setStartOctave(int);
    int getStartOctave() const { return startOctave; }
    void setEndOctave(int);
    int getEndOctave() const { return endOctave; }
    void setPersistance(float);
    float getPersistance() const { return persistance; }
    void setSeed(int);
    int getSeed() const { return seed; }
    
};

}

#endif // PERLINNOISE_H
