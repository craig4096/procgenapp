#ifndef THERMALEROSION_H
#define THERMALEROSION_H

#include <QObject>
#include "TerrainOperation.h"

namespace terrain_ops {

class ThermalErosion : public TerrainOperation {
    float c;
    int numIterations;
    float talusAngle;
public:
    ThermalErosion();
    
    void operate(HeightmapStack*, Progress*);
    void setNumIterations(int);
    void setTalusAngle(float);

    int getNumIterations() const { return numIterations; }
    float getTalusAngle() const { return talusAngle; }
};

}

#endif //THERMALEROSION_H
