#ifndef RENDERABLEHEIGHTMAP_H
#define RENDERABLEHEIGHTMAP_H

#include "GL/glew.h"
#include "Heightmap.h"

class RenderableHeightmap : public Heightmap
{
    vec3* normals;
    vec3* vertices;
    int* indices;

    GLuint	vertexBuffer;
    GLuint	normalBuffer;
    GLuint	indexBuffer;
    int	indexCount;
    int	vertexCount;
    bool renderDataCreated;

    void CalculateNormals();
    void CalculateVertices();
    void CalculateIndices();
public:
    // constructs this renderable heightmap given another heightmap
    RenderableHeightmap(Heightmap*);
    ~RenderableHeightmap();

    void Draw();
    // should be called after gl context has been initialized
    void CreateRenderData();
};

#endif // RENDERABLEHEIGHTMAP_H
