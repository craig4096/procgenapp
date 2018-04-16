#ifndef VIEWPORT3D_H
#define VIEWPORT3D_H

#include "GL/glew.h"
#include <QtOpenGL/QGLWidget>
#include "math3d.h"
#include "Skybox.h"

class Viewport3D : public QGLWidget {
    Q_OBJECT
public:
    class Renderer {
    public:
        virtual void viewportInit(Viewport3D*) = 0;
        virtual void viewportDraw(Viewport3D*) = 0;
    };

private:
    Renderer*   renderer;
    float       cameraRadius;
    vec3        cameraPosition;
    float       camYRotation;
    float       camXRotation;
    bool        draggingLeftMouse;
    bool        wireframe;
    int         prevXPos;
    int         prevYPos;
    Skybox*     skybox;
    std::string skyboxDir;
    float       zoomRate;
    float       camMinDist;
    float       camRotWeight;

    matrix4     viewMatrix;

public:
    explicit Viewport3D(const std::string& skyboxDir, float zoomRate, float camMinDist, float camRotWeight, QWidget *parent = 0);
    

    const matrix4& getViewMatrix() const { return viewMatrix; }

    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();
    void setRenderer(Renderer*);
    void mousePressEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);
    void mouseMoveEvent(QMouseEvent*);
    void wheelEvent(QWheelEvent*);
};

#endif // VIEWPORT3D_H
