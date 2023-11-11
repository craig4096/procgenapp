#ifndef THERMALEROSION_H
#define THERMALEROSION_H

#include "TerrainOperation.h"

namespace terrain_ops
{
    class ThermalErosion : public TerrainOperation
    {
        float c;
        int numIterations;
        float talusAngle;
    public:
        ThermalErosion();

        void operate(HeightmapStack*, Progress*) override;
        void inspect(IPropertyInspector& inspector) override;

        void setNumIterations(int);
        void setTalusAngle(float);

        int getNumIterations() const { return numIterations; }
        float getTalusAngle() const { return talusAngle; }
    };
}

#endif //THERMALEROSION_H
