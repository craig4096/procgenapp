#ifndef NORMALIZATION_H
#define NORMALIZATION_H

#include "TerrainOperation.h"

namespace terrain_ops
{
    class Normalization : public TerrainOperation
    {
        float minRange, maxRange;
    public:
        Normalization();

        void operate(HeightmapStack*, Progress*) override;
        void inspect(IPropertyInspector& inspector) override;

        void setMinRange(float);
        void setMaxRange(float);
        float getMinRange() const { return minRange; }
        float getMaxRange() const { return maxRange; }
    };
}

#endif // NORMALIZATION_H
