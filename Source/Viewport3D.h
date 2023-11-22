#ifndef VIEWPORT3D_H
#define VIEWPORT3D_H

#include "GL/glew.h"
#include <wx/glcanvas.h>
#include <glm/glm.hpp>
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
    void initialiseGL();

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
    bool        glInitialised;

    glm::mat4 modelViewMatrix;
    glm::mat4 projectionMatrix;
    glm::mat4 modelViewProjectionMatrix;
    glm::mat4 normalMatrix;

    wxGLContext* context;
public:
    explicit Viewport3D(wxWindow* parent, const wxGLAttributes& displayAttributes, Renderer* renderer, const std::string& skyboxDir, float zoomRate, float camMinDist, float camRotWeight);
    ~Viewport3D();

    const glm::mat4& getModelViewMatrix() const { return modelViewMatrix; }
    const glm::mat4& getProjectionMatrix() const { return projectionMatrix; }
    const glm::mat4& getModelViewProjectionMatrix() const { return modelViewProjectionMatrix; }
    const glm::mat4& getNormalMatrix() const { return normalMatrix; }
    void updateMatricesFromGL();

    void resizeGL(wxSizeEvent& event);
    void paintGL(wxPaintEvent& event);

    void mouseLeftDownEvent(wxMouseEvent& event);
    void mouseLeftUpEvent(wxMouseEvent& event);
    void mouseMoveEvent(wxMouseEvent& event);
    void wheelEvent(wxMouseEvent& event);

    DECLARE_EVENT_TABLE()
};

#endif // VIEWPORT3D_H
