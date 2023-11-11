#include "Modulate.h"

namespace terrain_ops
{
    void Modulate::operate(HeightmapStack* stack, Progress* progress)
    {
        // pop two heightmaps from the stack
        Heightmap* a = stack->pop();
        Heightmap* b = stack->pop();

        progress->setPercent(100.0f);

        // assert (same size)
        for (int i = 0; i < a->GetSize(); ++i)
        {
            a->SetValue(i, a->GetValue(i) * b->GetValue(i));
        }
        stack->push(a);
        delete b;
    }

    void Modulate::inspect(IPropertyInspector& inspector)
    {
        // no properties
    }
}
