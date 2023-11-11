#ifndef VIEWPORT3D_H
#define VIEWPORT3D_H

#include "GL/glew.h"
#include <wx/glcanvas.h>
#include "math3d.h"
#include "Skybox.h"

class Viewport3D : public wxGLCanvas
{
public:
    class Renderer
    {
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
    int         prevXPos;
    int         prevYPos;
    Skybox*     skybox;
    std::string skyboxDir;
    float       zoomRate;
    float       camMinDist;
    float       camRotWeight;

    matrix4     viewMatrix;

    wxGLContext* context;
public:
    explicit Viewport3D(wxWindow* parent, Renderer* renderer, const std::string& skyboxDir, float zoomRate, float camMinDist, float camRotWeight);
    ~Viewport3D();

    const matrix4& getViewMatrix() const { return viewMatrix; }

    void resizeGL(wxSizeEvent& event);
    void paintGL(wxPaintEvent& event);

    void mouseLeftDownEvent(wxMouseEvent& event);
    void mouseLeftUpEvent(wxMouseEvent& event);
    void mouseMoveEvent(wxMouseEvent& event);
    void wheelEvent(wxMouseEvent& event);

    DECLARE_EVENT_TABLE()
};

#endif // VIEWPORT3D_H
