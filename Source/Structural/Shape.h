
#ifndef SHAPE_H
#define SHAPE_H

#include <string>
#include "math3d.h"
#include <map>

class Mesh;

typedef std::map<std::string, Mesh*> SymbolMeshMap;

struct Shape
{
    std::string symbol;
    // absolute position and rotation
    vec3 position;
    float yRotation;
    // every shape has its own bounding box (depends on orientation parameters)
    BoundingBox aabb;
    bool terminal;

    bool isTerminal() const;

    // calculate the shapes bounding box given the mapping between
    // shape symbol and mesh object
    void calculateBoundingBox(const SymbolMeshMap&);
};

#endif
