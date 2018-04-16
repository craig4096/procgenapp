#ifndef MODULATE_H
#define MODULATE_H

#include "TerrainOperation.h"

namespace terrain_ops {

class Modulate : public TerrainOperation {
public:
    void operate(HeightmapStack*, Progress*);
};

}

#endif // TERRAINMODULATE_H
