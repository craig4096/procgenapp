#include "VoronoiDiagram.h"
#include <cstdlib>
using namespace std;

namespace terrain_ops {

VoronoiDiagram::VoronoiDiagram()
    : blendWeightA(-1.0f)
    , blendWeightB(1.0f)
    , seed(0)
    , numFeaturePoints(100)
{
}


void VoronoiDiagram::operate(HeightmapStack* stack, Progress* progress) {
    // create a new heightmap
    Heightmap* h = stack->createNewHeightmap();

    std::vector<vec2> points(numFeaturePoints);

    srand(seed);
    // generate random array of points between (0-width, 0-height)
    for(int i = 0; i < points.size(); ++i) {
        points[i].x = (rand()/(float)RAND_MAX) * h->GetWidth();
        points[i].y = (rand()/(float)RAND_MAX) * h->GetHeight();
    }

    progress->setPercent(0.0f);
    // for each pixel of the image
    for(int x = 0; x < h->GetWidth(); ++x) {
        for(int y = 0; y < h->GetHeight(); ++y) {
            // calculate a sorted array of points based on how
            // far they are from the current cell (use insertion sort)
            vector<int> pointIndices;
            vector<float> pointDistances;
            vec2 a((float)x, (float)y);
            for(int i = 0; i < points.size(); ++i) {
                const vec2& b = points[i];
                // calculate manhatton distance between the two points
                float abx = b.x-a.x;
                float aby = b.y-a.y;
                float dist = sqrt((abx*abx)+(aby*aby));
                pointDistances.push_back(dist);
                pointIndices.push_back(i);
                // now sort the newly added element to the array
                for(int j = pointDistances.size()-2; j >= 0; --j) {
                    if(pointDistances[j] > pointDistances[j+1]) {
                        std::swap(pointDistances[j], pointDistances[j+1]);
                        std::swap(pointIndices[j], pointIndices[j+1]);
                    } else {
                        break;
                    }
                }
            }
            // now shade the point using linear combinations of the two closest points (i.e. h = c1d1 + c2d2 + c3d3 + ...)
            // where c1 = -1 and c2 = 1 and c3...cn = 0
            // assert(numPoints > 1)
            h->SetValue(x, y, (blendWeightB * pointDistances[1]) + (blendWeightA * pointDistances[0]));
        }
        progress->setPercent((x+1)/((float)h->GetWidth()-1) * 100.0f);
    }
    h->Normalize(0, 1);
    stack->push(h);
}

void VoronoiDiagram::inspect(IPropertyInspector& inspector)
{
    inspector.property("Weight A", blendWeightA);
    inspector.property("Weight B", blendWeightB);
    inspector.property("Seed", seed);
    inspector.property("Num Points", numFeaturePoints);
}

void VoronoiDiagram::setBlendWeightA(float a) {
    blendWeightA = a;
}

void VoronoiDiagram::setBlendWeightB(float b) {
    blendWeightB = b;
}

void VoronoiDiagram::setSeed(int seed) {
    this->seed = seed;
}

void VoronoiDiagram::setNumFeaturePoints(int numFeaturePoints) {
    this->numFeaturePoints = numFeaturePoints;
}

}
