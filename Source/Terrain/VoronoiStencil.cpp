#include "VoronoiStencil.h"
#include <cstdlib>
using namespace std;

namespace terrain_ops {

VoronoiStencil::VoronoiStencil()
    : probabilityFactor(0.5f)
    , seed(0)
    , whiteValue(1.0f)
    , blackValue(0.0f)
    , numFeaturePoints(100)
{
}

void VoronoiStencil::operate(HeightmapStack* stack, Progress* progress) {
    // create a new heightmap
    Heightmap* h = stack->createNewHeightmap();

    std::vector<vec2> points(numFeaturePoints);

    srand(seed);
    // generate random array of points between (0-width, 0-height)
    for(int i = 0; i < points.size(); ++i) {
        points[i].x = (rand()/(float)RAND_MAX) * h->GetWidth();
        points[i].y = (rand()/(float)RAND_MAX) * h->GetHeight();
    }

    vector<bool> pointColors(points.size());
    for(int i = 0; i < pointColors.size(); ++i) {
        pointColors[i] = randomf() < probabilityFactor ? true : false;
    }

    progress->setPercent(0.0f);
    // for each pixel of the image
    for(int x = 0; x < h->GetWidth(); ++x) {
        for(int y = 0; y < h->GetHeight(); ++y) {
            // find the closest point to (x,y)
            vec2 a((float)x, (float)y);
            float closestDist = 100000000.0f;
            int	closestPointIndex;
            for(int i = 0; i < points.size(); ++i) {
                const vec2& b = points[i];
                // calculate squared distance between the two points
                float dist = ((b.x-a.x)*(b.x-a.x))+((b.y-a.y)*(b.y-a.y));
                if(dist < closestDist) {
                    closestDist = dist;
                    closestPointIndex = i;
                }
            }
            // set the color either black or white
            h->SetValue(x, y, pointColors[closestPointIndex] ? 0.0f : 1.0f);
        }
        progress->setPercent((x+1)/((float)h->GetWidth()-1) * 100.0f);
    }
    stack->push(h);
}

void VoronoiStencil::setProbabilityFactor(float p) {
    probabilityFactor = p;
}

void VoronoiStencil::setSeed(int seed) {
    this->seed = seed;
}

void VoronoiStencil::setNumFeaturePoints(int numFeaturePoints) {
    this->numFeaturePoints = numFeaturePoints;
}

void VoronoiStencil::setWhiteValue(float d) {
    whiteValue = d;
}

void VoronoiStencil::setBlackValue(float d) {
    blackValue = d;
}

}
