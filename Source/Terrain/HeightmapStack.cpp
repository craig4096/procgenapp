#include "HeightmapStack.h"

HeightmapStack::HeightmapStack(int heightmapWidth, int heightmapHeight)
    : heightmapWidth(heightmapWidth)
    , heightmapHeight(heightmapHeight)
{
}


void HeightmapStack::push(Heightmap* h) {
    stack.push_back(h);
}

Heightmap* HeightmapStack::pop() {
    Heightmap* h = stack.back();
    stack.pop_back();
    return h;
}

Heightmap* HeightmapStack::createNewHeightmap() const {
    return new Heightmap(heightmapWidth, heightmapHeight);
}

void HeightmapStack::clear() {
    stack.clear();
}
