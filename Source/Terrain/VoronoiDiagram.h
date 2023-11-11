#ifndef VORONOIDIAGRAM_H
#define VORONOIDIAGRAM_H

#include "math3d.h"
#include "TerrainOperation.h"

namespace terrain_ops
{
    class VoronoiDiagram : public TerrainOperation
    {
        //std::vector<float> blendCoefficients;
        float blendWeightA;
        float blendWeightB;
        int seed;
        int numFeaturePoints;
    public:
        VoronoiDiagram();

        void operate(HeightmapStack*, Progress*) override;
        void inspect(IPropertyInspector& inspector) override;

        void setBlendWeightA(float);
        float getBlendWeightA() const { return blendWeightA; }
        void setBlendWeightB(float);
        float getBlendWeightB() const { return blendWeightB; }
        void setSeed(int);
        int getSeed() const { return seed; }
        void setNumFeaturePoints(int);
        int getNumFeaturePoints() const { return numFeaturePoints; }
    };
}

#endif // VORONOIDIAGRAM_H
