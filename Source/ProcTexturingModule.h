#ifndef PROCTEXTURINGMODULE_H
#define PROCTEXTURINGMODULE_H

#include <QObject>
#include <string>
#include <GL/glew.h>
#include "Viewport3D.h"
#include "Shader.h"
#include "Mesh.h"

namespace Ui {
    class Application;
}

class ProcTexturingModule : public QObject, public Viewport3D::Renderer
{
    Q_OBJECT

    Mesh* mesh;
    Shader* shader;
    // flag to determine whether the current shader needs to be re-loaded and re-compiled
    bool shaderInvalid;
    Ui::Application* ui;
    std::string activeFragShaderFilename;
    Viewport3D* viewport;
    float time;

public:
    explicit ProcTexturingModule(Ui::Application* ui, QObject *parent = 0);
    ~ProcTexturingModule();

    void viewportInit(Viewport3D*);
    void viewportDraw(Viewport3D*);

public slots:

    void loadShader();
    void newShader();
    void saveAndCompile();
    
};

#endif // PROCTEXTURINGMODULE_H
