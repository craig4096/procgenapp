#ifndef HEIGHTMAPSTACK_H
#define HEIGHTMAPSTACK_H

#include "Heightmap.h"
#include <vector>

class HeightmapStack
{
    std::vector<Heightmap*> stack;
    int heightmapWidth, heightmapHeight;
public:
    HeightmapStack(int heightmapWidth, int heightmapHeight);
    // pushes and pops a heightmap from the heightmap stack resp.
    void push(Heightmap*);
    Heightmap* pop();
    void clear();

    Heightmap* createNewHeightmap() const;
};

#endif // HEIGHTMAPSTACK_H
