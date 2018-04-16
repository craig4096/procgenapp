#ifndef TERRAINOPERATION_H
#define TERRAINOPERATION_H

#include "HeightmapStack.h"
#include "Progress.h"

class TerrainOperation {
public:
    virtual ~TerrainOperation() {}

    virtual void operate(HeightmapStack*, Progress*) = 0;
};

#endif // TERRAINOPERATION_H
