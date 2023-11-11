
#include "Shape.h"
#include "Mesh.h"
#include <stdexcept>
#include <iostream>
using namespace std;

void Shape::calculateBoundingBox(const SymbolMeshMap& symbolMeshMap)
{
    // now update the bounding box of the shape using the symbolMeshMap
    SymbolMeshMap::const_iterator msh = symbolMeshMap.find(symbol);
    if (msh == symbolMeshMap.end())
    {
        throw std::logic_error("could not find mesh in map: " + symbol);
    }

    // get the corners of the bounding box of the mesh
    vec3 corners[8];
    msh->second->getBounds().getCorners(corners);

    // translate and rotate them into world space
    for (int j = 0; j < 8; ++j)
    {
        corners[j] = RotateVectorY(corners[j], yRotation) + position;
    }

    // construct new bounding box from these corner points
    aabb.min = aabb.max = corners[0];
    for (int j = 1; j < 8; ++j)
    {
        aabb.addPoint(corners[j]);
    }
}

bool Shape::isTerminal() const
{
    return terminal;
}
