#ifndef PROCTEXTURINGMODULE_H
#define PROCTEXTURINGMODULE_H

#include <string>
#include <GL/glew.h>
#include "Viewport3D.h"
#include "Shader.h"
#include "Mesh.h"
#include "MainWindow.h"

class ProcTexturingModule : public Viewport3D::Renderer
{
    Mesh* mesh;
    Shader* shader;
    // flag to determine whether the current shader needs to be re-loaded and re-compiled
    bool shaderInvalid;
    MainWindow* mainWindow;
    std::string activeFragShaderFilename;
    Viewport3D* viewport;
    float time;

public:
    explicit ProcTexturingModule(MainWindow* mainWindow, const wxGLAttributes& displayAttributes);
    ~ProcTexturingModule();

    Viewport3D* GetViewport() { return viewport; }

    void viewportInit(Viewport3D*);
    void viewportDraw(Viewport3D*);

    void loadShader();
    void newShader();
    void saveAndCompile();

};


#endif // PROCTEXTURINGMODULE_H
