#ifndef BLEND_H
#define BLEND_H

#include "TerrainOperation.h"

namespace terrain_ops {

class Blend : public TerrainOperation
{
    float blendFactor;
public:
    Blend();

    void operate(HeightmapStack*, Progress*);
    void setBlendFactor(float);
    float getBlendFactor() const;
};

}

#endif // BLEND_H
