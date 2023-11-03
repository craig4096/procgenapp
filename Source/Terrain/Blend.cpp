#include "Blend.h"

namespace terrain_ops {

Blend::Blend()
    : blendFactor(0.5f)
{
}

void Blend::operate(HeightmapStack* stack, Progress* progress) {
    // pop two height maps from the stack
    Heightmap* b = stack->pop();
    Heightmap* a = stack->pop();
    // assert(same size)
    progress->setPercent(0.0f);
    for(int i = 0; i < a->GetSize(); ++i) {
        a->SetValue(i, (a->GetValue(i) * blendFactor) + (b->GetValue(i) * (1.0f - blendFactor)));
        progress->setPercent((i/(float)(a->GetSize()-1))*100.0f);
    }
    a->Normalize(0, 1);
    stack->push(a);
    delete b;
}

void Blend::inspect(IPropertyInspector& inspector)
{
    inspector.property("Blend Factor", blendFactor); // range 0-1
}

void Blend::setBlendFactor(float b) {
    blendFactor = b;
}

float Blend::getBlendFactor() const {
    return blendFactor;
}

}
