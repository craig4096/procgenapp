#ifndef VORONOISTENCIL_H
#define VORONOISTENCIL_H

#include "math3d.h"
#include "TerrainOperation.h"

namespace terrain_ops {

class VoronoiStencil : public TerrainOperation {
    // determines the probability of a voronoi region being white as
    // opposed to black (i.e. if this value is 1 then the image will be white
    // if it is 0 then it will be black)
    float   probabilityFactor;
    int     numFeaturePoints;
    int     seed;
    float   whiteValue;
    float   blackValue;
public:
    VoronoiStencil();

    void operate(HeightmapStack*, Progress*);
    void setProbabilityFactor(float);
    float getProbabilityFactor() const { return probabilityFactor; }
    void setSeed(int);
    int getSeed() const { return seed; }
    void setNumFeaturePoints(int);
    int getNumFeaturePoints() const { return numFeaturePoints; }
    void setWhiteValue(float);
    float getBlackValue() const { return blackValue; }
    void setBlackValue(float);
    float getWhiteValue() const { return whiteValue; }
};

}

#endif // VORONOISTENCIL_H
