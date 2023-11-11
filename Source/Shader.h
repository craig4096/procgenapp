
#pragma once
#include <string>
#include "GL/glew.h"

class Shader
{
    GLuint vertexShader, fragmentShader;
    GLuint program;
    void load(const std::string& vertexfile, const std::string& fragfile);
    std::string processIncludes(const std::string& shaderFile);
public:

    Shader(const std::string& filename);
    Shader(const std::string& vertexfile, const std::string& fragfile);
    ~Shader();

    void Set();
    static void Unset();
    GLuint GetProgram() const { return program; }
};
