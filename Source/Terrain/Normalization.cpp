#include "Normalization.h"

namespace terrain_ops {

Normalization::Normalization()
  : minRange(0.0)
  , maxRange(1.0)
{
}

void Normalization::operate(HeightmapStack* stack, Progress* progress) {
    Heightmap* h = stack->pop();
    progress->setPercent(100.0f);
    h->Normalize(minRange, maxRange);
    stack->push(h);
}

void Normalization::inspect(IPropertyInspector& inspector)
{
    inspector.property("Min Range", minRange);
    inspector.property("Max Range", maxRange);
}

void Normalization::setMinRange(float min) {
    minRange = min;
}

void Normalization::setMaxRange(float max) {
    maxRange = max;
}

}
