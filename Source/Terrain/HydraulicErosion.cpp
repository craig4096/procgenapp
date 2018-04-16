#include "HydraulicErosion.h"
#include <QTime>
#include <fstream>
#include <iostream>
#include "defs.h"
using namespace std;

namespace terrain_ops {

HydraulicErosion::HydraulicErosion()
    : waterConst(0.01f)
    , solubility(0.01f)
    , evaporationCoef(0.5f)
    , sedimentCapacity(0.01f)
    , numIterations(100)
{
}

void HydraulicErosion::operate(HeightmapStack* stack, Progress* progress) {
    progress->setPercent(100.0f);
    // pop heightmap
    Heightmap* h = stack->pop();

    Array2D<float>	water(h->GetWidth(), h->GetHeight());		// 2d array holds water values
    Array2D<float>	sediment(h->GetWidth(), h->GetHeight());	// 2d array holds sediment values

    QTime timer;
    timer.start();
    // initialize water and sediment to 0
    for(int i = 0; i < water.GetSize(); ++i) {
        water[i] = sediment[i] = 0.0f;
    }

#ifdef HYDRAULIC_OUTPUT_STATS
    std::ofstream stats("stats/hydraulic_stats.txt");
    stats.setf(ios::fixed, ios::floatfield);
    stats.precision(6);
    stats << "# ErosionScore\t\tWalkableArea\t\tDisconnectedRegions" << endl;
#endif

    for(int kk = 0; kk < numIterations; ++kk) {
#ifdef HYDRAULIC_OUTPUT_STATS
        stats << h->CalculateErosionScore() << "\t\t";
        stats << h->CalculateTotalWalkableArea(45.0f) << "\t\t";
        stats << h->CalculateNumDisconnectedRegions(45.0f) << endl;
#endif

        // Step 1: add the waterConst to water content (i.e. rain)
        for(int i = 0; i < water.GetSize(); ++i) {
            water[i] += waterConst;
        }

        // Step 2: dissolve the material into sediment
        for(int i = 0; i < water.GetSize(); ++i) {
            float sed = solubility * water[i];
            (*h)[i] -= sed;
            sediment[i] += sed;
        }

        // Step 3: loop through all cells and distribute the water by using: dw = min(w, da) * di/dtotal
        for(int x = 0; x < h->GetWidth(); ++x) {
            for(int y = 0; y < h->GetHeight(); ++y) {
                static const int NUM_NEIGHBOURS = 8;
                // neighbour indices (Moore or Von Neuman)
                int nIndices[8][2] = {
                    {x-1, y},
                    {x+1, y},
                    {x, y-1},
                    {x, y+1},
                    {x+1, y+1},
                    {x-1, y+1},
                    {x+1, y-1},
                    {x-1, y-1}
                };

                float	nAltitudes[NUM_NEIGHBOURS];	// neighbour altitudes
                float	averageA = 0.0f;            // the average of all the neighbour altitudes
                int     count = 0;                  // average count
                float	altitude = h->GetValue(x, y) + water(x, y);	// total altitude of the current cell (height + water)
                bool	isValid[NUM_NEIGHBOURS];	// determines if neighbour indices are valid
                float	dis[NUM_NEIGHBOURS];		// difference in altitude for each neighbour
                float	dtotal = 0.0f;              // total difference for all valid neighbours

                for(int i = 0; i < NUM_NEIGHBOURS; ++i) {
                    // is neighbour valid
                    if(h->IndexValid(nIndices[i][0], nIndices[i][1])) {
                        nAltitudes[i] = h->GetValue(nIndices[i][0], nIndices[i][1]) + water(nIndices[i][0], nIndices[i][1]);
                        dis[i] = altitude - nAltitudes[i];
                        if(dis[i] > 0.0f) {
                            dtotal += dis[i];
                            averageA += nAltitudes[i];
                            count++;
                            isValid[i] = true;
                        } else {
                            isValid[i] = false;
                        }
                    } else {
                        isValid[i] = false;
                    }
                }

                // this cells altitude is greater than all neighbours
                if(count == 0) {
                    continue;
                }

                averageA /= (float)count;

                // difference in altitude of this cell and average of neighbours
                float deltaA = altitude - averageA;

                // now loop through all neighbours and adjust the water level based on equation from above
                for(int i = 0; i < NUM_NEIGHBOURS; ++i) {
                    if(isValid[i]) {
                        int nx = nIndices[i][0];
                        int ny = nIndices[i][1];

                        // calculate water distribution
                        float dWater = std::min(water(x, y), deltaA) * (dis[i] / dtotal);
                        // calculate sediment distribution (proportianal to water distribution)
                        float dSediment = sediment(x, y) * (dWater / water(x, y));
                        // transfer sediment to neighbour
                        sediment(x, y) -= dSediment;
                        sediment(nx, ny) += dSediment;
                        // transfer water to neighbour
                        water(x, y) -= dWater;
                        water(nx, ny) += dWater;
                    }
                }
            }
        }

        // Step 4: evaporate a percentage of the water content
        for(int i = 0; i < water.GetSize(); ++i) {
            // evaporate water
            water[i] = water[i] * (1.0f - evaporationCoef);
            // once part of the water has evaporated, the amount of sediment exceeding
            // the maximum capacity is transfered back to h
            float mMax = sedimentCapacity * water[i];
            float dSediment = std::max(0.0f, sediment[i] - mMax);
            sediment[i] -= dSediment;
            (*h)[i] += dSediment;
        }
        progress->setPercent((kk+1)/(float)numIterations * 100.0f);
    }
    cout << "Hydraulic Erosion Total Time(Ms): " << timer.elapsed() << endl;
    stack->push(h);
}


void HydraulicErosion::setWaterConst(float waterConst) {
    this->waterConst = waterConst;
}

void HydraulicErosion::setSolubility(float solubility) {
    this->solubility = solubility;
}

void HydraulicErosion::setEvaporationCoefficient(float evaporationCoef) {
    this->evaporationCoef = evaporationCoef;
}

void HydraulicErosion::setSedimentCapacity(float sedimentCapacity) {
    this->sedimentCapacity = sedimentCapacity;
}

void HydraulicErosion::setNumIterations(int numIterations) {
    this->numIterations = numIterations;
}

}
