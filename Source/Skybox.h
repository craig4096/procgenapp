
#pragma once
#include "GL/glew.h"
#include "math3d.h"
#include <string>

struct Skybox {
    GLuint vertices;
    GLuint texcoords;
    GLuint textures[6];
public:
    Skybox(const std::string textures[6]);
    ~Skybox();
    void Draw(const vec3& campos, float scale = 1000.0f);
};
