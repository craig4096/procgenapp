#include "ThermalErosion.h"
#include <fstream>
#include <QTime>
#include <iostream>
#include "defs.h"
using namespace std;

namespace terrain_ops {

ThermalErosion::ThermalErosion()
    : numIterations(100)
    , c(0.5f)
    , talusAngle(2.0f)
{
}

void ThermalErosion::operate(HeightmapStack* stack, Progress* progress) {
    // pop heightmap
    Heightmap* h = stack->pop();

    progress->setPercent(0.0f);

#ifdef THERMAL_OUTPUT_STATS
    std::ofstream stats("stats/thermal_stats.txt");
    stats.setf(ios::fixed, ios::floatfield);
    stats.precision(6);
    stats << "#ErosionScore\t\tWalkableArea\t\tDisconnectedRegions" << endl;
#endif
    QTime timer;
    timer.start();
    for(int j = 0; j < numIterations; ++j) {
#ifdef THERMAL_OUTPUT_STATS
        stats << h->CalculateErosionScore() << "\t\t";
        stats << h->CalculateTotalWalkableArea(45.0f) << "\t\t";
        stats << h->CalculateNumDisconnectedRegions(45.0) << endl;
#endif

        for(int x = 0; x < h->GetWidth(); ++x) {
            for(int y = 0; y < h->GetHeight(); ++y) {
                // cached neighour indices
                static const int NUM_NEIGHBOURS = 8; // either von-neuman or moore
                int nIndices[NUM_NEIGHBOURS][2] = {
                    x-1, y,
                    x+1, y,
                    x, y-1,
                    x, y+1,
                    x+1, y+1,
                    x-1, y+1,
                    x+1, y-1,
                    x-1, y-1
                };

                bool	isValid[NUM_NEIGHBOURS];
                float	dis[NUM_NEIGHBOURS];		// difference in heights for each neighbour
                float	dtotal = 0.0f;
                float	dmax = 0.0f;

                // determine if the neighbours are valid
                for(int i = 0; i < NUM_NEIGHBOURS; ++i) {
                    if(h->IndexValid(nIndices[i][0], nIndices[i][1])) {
                        // calculate height difference between this height and neighbour height
                        dis[i] = h->GetScaledHeight(x, y) -
                                h->GetScaledHeight(nIndices[i][0], nIndices[i][1]);

                        // if its greater than the talus angle
                        // (note: need to scale talusAngle to assure correct angle)
                        if(dis[i] > (talusAngle * h->GetScaleX())) {
                            dtotal += dis[i];
                            if(dis[i] > dmax) {
                                dmax = dis[i];
                            }
                            isValid[i] = true;
                        } else {
                            isValid[i] = false;
                        }
                    } else {
                        isValid[i] = false;
                    }
                }

                // now displace material to neighbours
                for(int i = 0; i < NUM_NEIGHBOURS; ++i) {
                    if(isValid[i]) {
                        (*h)(nIndices[i][0], nIndices[i][1]) +=
                                ((c * (dmax - talusAngle)) * (dis[i]/dtotal)) / h->GetScaleY();
                    }
                }
            }
        }
        progress->setPercent((j+1)/(float)numIterations * 100.0f);
    }
    cout << "Thermal Erosion Time(Ms): " << timer.elapsed() << endl;
    stack->push(h);
}

void ThermalErosion::setNumIterations(int numIterations) {
    this->numIterations = numIterations;
}


void ThermalErosion::setTalusAngle(float angle) {
    this->talusAngle = angle;
}

}


/*
// calculate slope between this cell and neighbour
// get the two heights first
float h = heightmap->GetHeight(x, y) * heightmap->GetScaleY();
float hi = heightmap->GetHeight(nIndices[i][0], nIndices[i][1]) * heightmap->GetScaleY();
// get the distance inbetween the heights
float dist = (i > 1) ? heightmap->GetScaleZ() : heightmap->GetScaleX();
if((h - hi) > 0.0f) {
// calculate the angle between these
float angle = atan((h - hi) / dist) * RAD_TO_DEG;
*/


// TODO: change heightmap to have a fixed world space size (x and z) and the size
// should be the number of samples inbetween this
