#ifndef INVERSETHERMALEROSION_H
#define INVERSETHERMALEROSION_H

#include "TerrainOperation.h"

namespace terrain_ops {

class InverseThermalErosion : public TerrainOperation {
    int numIterations;
    // used to calculate the talus angle
    float talusAngle;
public:
    InverseThermalErosion();
    
    void operate(HeightmapStack*, Progress*);
    void setNumIterations(int);
    void setTalusAngle(float);

    int getNumIterations() const { return numIterations; }
    float getTalusAngle() const { return talusAngle; }
};

}

#endif // INVERSETHERMALEROSION_H
