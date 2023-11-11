#include "InverseThermalErosion.h"
#include <fstream>
#include "defs.h"
#include <wx/stopwatch.h>
#include <iostream>
using namespace std;

namespace terrain_ops
{
    InverseThermalErosion::InverseThermalErosion()
        : numIterations(100)
        , talusAngle(2.0f)
    {
    }


    void InverseThermalErosion::operate(HeightmapStack* stack, Progress* progress)
    {
        // pop heightmap
        Heightmap* h = stack->pop();

        progress->setPercent(0.0f);

#ifdef INVTHERMAL_OUTPUT_STATS
        std::ofstream stats("stats/inverse_thermal_stats.txt");
        stats.setf(ios::fixed, ios::floatfield);
        stats.precision(6);
        stats << "# ErosionScore\t\tWalkableArea\t\tDisconnected Regions" << endl;
#endif

        wxStopWatch timer;
        timer.Start();
        for (int j = 0; j < numIterations; ++j)
        {
#ifdef INVTHERMAL_OUTPUT_STATS
            stats << h->CalculateErosionScore() << "\t\t";
            stats << h->CalculateTotalWalkableArea(45.0f) << "\t\t";
            stats << h->CalculateNumDisconnectedRegions(45.0f) << endl;
#endif

            for (int x = 0; x < h->GetWidth(); ++x)
            {
                for (int y = 0; y < h->GetHeight(); ++y)
                {
                    // cached neighour indices
                    static const int NUM_NEIGHBOURS = 8; // Moore(8) or Von Neumann(4) neighbourhood
                    // neighbour indices
                    const int nx[8] = { x - 1, x + 1, x, x, x + 1, x - 1, x + 1, x - 1 };
                    const int ny[8] = { y, y, y - 1, y + 1, y + 1, y + 1, y - 1, y - 1 };
                    float	dmax = 0.0f;	// maximum of all valid height differences
                    int	lowest = -1;

                    const float height = h->GetScaledHeight(x, y);

                    // find the lowest sloping neighbour index
                    for (int i = 0; i < NUM_NEIGHBOURS; ++i)
                    {
                        if (h->IndexValid(nx[i], ny[i]))
                        {
                            // calculate height difference between this height and neighbour height
                            float di = height - h->GetScaledHeight(nx[i], ny[i]);

                            if (di > dmax)
                            {
                                dmax = di;
                                lowest = i;
                            }
                        }
                    }
                    if (dmax > 0.0f && dmax <= (talusAngle * h->GetScaleX()))
                    {
                        float dh = 0.5f * (dmax / h->GetScaleY());
                        (*h)(x, y) -= dh;
                        (*h)(nx[lowest], ny[lowest]) += dh;
                    }
                }
            }

            progress->setPercent((j + 1) / (float)numIterations * 100.0f);
        }
        cout << "Inverse Thermal Total Time(Ms): " << timer.Time() << endl;

        stack->push(h);
    }

    void InverseThermalErosion::inspect(IPropertyInspector& inspector)
    {
        inspector.property("Num Iterations", numIterations);
        inspector.property("Talus Angle", talusAngle);
    }

    void InverseThermalErosion::setNumIterations(int numIterations)
    {
        this->numIterations = numIterations;
    }

    void InverseThermalErosion::setTalusAngle(float angle)
    {
        this->talusAngle = angle;
    }
}
