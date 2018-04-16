#ifndef HYDRAULICEROSION_H
#define HYDRAULICEROSION_H

#include "TerrainOperation.h"

namespace terrain_ops {

class HydraulicErosion : public TerrainOperation {

    float	waterConst;	// constant amount of water to be added at each iteration
    float	solubility;	// the fraction of rain water that turns into sediment
    float	evaporationCoef;// evaporation coefficient
    float	sedimentCapacity;// determines how much sediment can be carried by water (m_max = sedimentCapacity * w)
    int     numIterations;

public:
    HydraulicErosion();
    
    void operate(HeightmapStack*, Progress*);
    void setWaterConst(float);
    void setSolubility(float);
    void setEvaporationCoefficient(float);
    void setSedimentCapacity(float);
    void setNumIterations(int);

    float getWaterConst() const { return waterConst; }
    float getSolubility() const { return solubility; }
    float getEvaporationCoefficient() const { return evaporationCoef; }
    float getSedimentCapacity() const { return sedimentCapacity; }
    int getNumIterations() const { return numIterations; }
};

}

#endif // HYDRAULICEROSION_H
