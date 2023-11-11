#include "TerrainGenerator.h"

TerrainGenerator::TerrainGenerator(int heightmapWidth, int heightmapHeight)
    : heightmapWidth(heightmapWidth)
    , heightmapHeight(heightmapHeight)
    , stack(heightmapWidth, heightmapHeight)
{
}


void TerrainGenerator::addOperation(TerrainOperation* op)
{
    operations.push_back(op);
}

TerrainOperation* TerrainGenerator::removeOperation(int index)
{
    TerrainOperation* op = operations[index];
    operations.erase(operations.begin() + index);
    return op;
}

class NullProgress : public Progress
{
public:
    void setPercent(float) {}
    void nextOperation(int) {}
};

Heightmap* TerrainGenerator::generateTerrain(Progress* progress)
{
    NullProgress p;
    if (progress == nullptr)
    {
        progress = &p;
    }
    // apply all operations to the terrain
    for (int i = 0; i < operations.size(); ++i)
    {
        progress->nextOperation(i);
        //ui->terrain_active_oplist->repaint();
        operations[i]->operate(&stack, progress);
    }

    Heightmap* h = stack.pop();
    stack.clear(); // just as pre-caution
    return h;
}

TerrainOperation* TerrainGenerator::getOperation(int index)
{
    if (index >= 0 && index < operations.size())
    {
        return operations[index];
    }
    return nullptr;
}

void TerrainGenerator::switchOperations(int indexA, int indexB)
{
    std::swap(operations[indexA], operations[indexB]);
}

void TerrainGenerator::clearOperations(bool del)
{
    if (del)
    {
        for (int i = 0; i < operations.size(); ++i)
        {
            delete operations[i];
        }
    }
    operations.clear();
}
