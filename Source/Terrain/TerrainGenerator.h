#ifndef TERRAINGENERATOR_H
#define TERRAINGENERATOR_H

#include <vector>
#include "TerrainOperation.h"
#include "HeightmapStack.h"

class TerrainGenerator
{
    std::vector<TerrainOperation*> operations;
    HeightmapStack stack;
    int heightmapWidth, heightmapHeight;
public:
    TerrainGenerator(int heightmapWidth, int heightmapHeight);

    void addOperation(TerrainOperation*);
    TerrainOperation* removeOperation(int index);
    TerrainOperation* getOperation(int index);
    void clearOperations(bool deleteOps);
    Heightmap* generateTerrain(Progress*);
    int getOperationCount() const { return operations.size(); }

    void switchOperations(int indexA, int indexB);

    int getHeightmapWidth() const { return heightmapWidth; }
    int getHeightmapHeight() const { return heightmapHeight; }
};

#endif // TERRAINGENERATOR_H
