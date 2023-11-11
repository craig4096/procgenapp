
#pragma once

#include "GL/glew.h"
#include <fstream>
#include <string>
#include <vector>
#include "math3d.h"

class SubMesh
{
    GLuint vertices;
    GLuint indices;
    GLuint texcoords;
    GLuint normals;
    GLuint indexCount;
    std::string materialName;
    BoundingBox bounds;
public:

    const BoundingBox& getBounds() const { return bounds; }
    void Load(std::ifstream&);
    void Unload();
    void Draw();
    void DrawNoTextures();
    const std::string& getMaterialName() const { return materialName; }
};

class Mesh
{
    std::vector<SubMesh> meshes;
    BoundingBox bounds;
public:
    const BoundingBox& getBounds() const { return bounds; }
    Mesh(const std::string& filename);
    ~Mesh();
    void Draw();
    void DrawNoTextures();

    int getSubMeshCount() const { return meshes.size(); }
    SubMesh* getSubMesh(int index) { return &meshes[index]; }
};
