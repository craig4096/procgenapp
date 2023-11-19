#include "Viewport3D.h"
#include <wx/wx.h>
#include "math3d.h"
#include <iostream>
#include <glm/ext.hpp>

using namespace std;

BEGIN_EVENT_TABLE(Viewport3D, wxGLCanvas)
EVT_PAINT(Viewport3D::paintGL)
EVT_SIZE(Viewport3D::resizeGL)
EVT_LEFT_DOWN(Viewport3D::mouseLeftDownEvent)
EVT_LEFT_UP(Viewport3D::mouseLeftUpEvent)
EVT_MOUSEWHEEL(Viewport3D::wheelEvent)
EVT_MOTION(Viewport3D::mouseMoveEvent)
END_EVENT_TABLE()

Viewport3D::Viewport3D(wxWindow* parent, Renderer* renderer, const std::string& skyboxDir, float zoomRate, float camMinDist, float camRotWeight)
    : wxGLCanvas(parent, wxID_ANY)
    , renderer(renderer)
    , cameraRadius(200.0f)
    , prevXPos(0)
    , prevYPos(0)
    , camYRotation(0.0f)
    , camXRotation(M_PI / 4.0f)
    , skybox(NULL)
    , skyboxDir(skyboxDir)
    , zoomRate(zoomRate)
    , camMinDist(camMinDist)
    , camRotWeight(camRotWeight)
    , draggingLeftMouse(false)
{
    context = new wxGLContext(this);
    SetCurrent(*context);

    glClearColor(0.0f, 0.0f, 1.0f, 0.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // initiate glew lib
    if (glewInit() != GLEW_OK)
    {
        throw std::runtime_error("Could not initialize OpenGL");
    }

    if (renderer != nullptr)
    {
        renderer->viewportInit(this);
    }

    std::string skytextures[6] = {
        skyboxDir + "/posx.jpg",
        skyboxDir + "/negx.jpg",
        skyboxDir + "/posy.jpg",
        skyboxDir + "/negy.jpg",
        skyboxDir + "/posz.jpg",
        skyboxDir + "/negz.jpg"
    };
    skybox = new Skybox(skytextures);
}

Viewport3D::~Viewport3D()
{
    delete context;
}

void Viewport3D::updateMatricesFromGL()
{
    glGetFloatv(GL_PROJECTION_MATRIX, glm::value_ptr(projectionMatrix));
    glGetFloatv(GL_MODELVIEW_MATRIX, glm::value_ptr(modelViewMatrix));

    // save model view projection matrix
    modelViewProjectionMatrix = projectionMatrix * modelViewMatrix;

    // calculate the normal matrix
    normalMatrix = glm::transpose(glm::inverse(modelViewMatrix));
}

void Viewport3D::resizeGL(wxSizeEvent& event)
{
    Refresh();
}

void Viewport3D::paintGL(wxPaintEvent& event)
{
    SetCurrent(*context);
    wxPaintDC(this);

    const wxSize size = GetSize();
    const int w = size.GetWidth();
    const int h = size.GetHeight();

    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (h != 0)
    {
        gluPerspective(60.0f, w / (float)h, 0.01f, 10000.0f);
    }
    else
    {
        gluPerspective(60.0f, 0.0f, 0.01f, 1000.0f);
    }
    glMatrixMode(GL_MODELVIEW);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    vec3 tmp(sin(camYRotation), 0.0f, cos(camYRotation));

    cameraPosition = ((tmp * sin(camXRotation)) + (vec3(0, 1, 0) * cos(camXRotation))) * cameraRadius;

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(cameraPosition.x, cameraPosition.y, cameraPosition.z, 0, 0, 0, 0, 1, 0);

    updateMatricesFromGL();

    skybox->Draw(cameraPosition, 1000.0f);

    if (renderer != nullptr)
    {
        renderer->viewportDraw(this);
    }

    glFlush();
    SwapBuffers();
}

void Viewport3D::mouseLeftDownEvent(wxMouseEvent& event)
{
    prevXPos = event.m_x;
    prevYPos = event.m_y;
    draggingLeftMouse = true;
}

void Viewport3D::mouseLeftUpEvent(wxMouseEvent& event)
{
    draggingLeftMouse = false;
}

void Viewport3D::wheelEvent(wxMouseEvent& event)
{
    const int delta = event.GetWheelRotation();

    cameraRadius -= delta * zoomRate;
    if (cameraRadius < camMinDist)
    {
        cameraRadius = camMinDist;
    }
    Refresh();
}

void Viewport3D::mouseMoveEvent(wxMouseEvent& event)
{
    // calculate new camera orientation
    if (draggingLeftMouse)
    {
        const int x = event.m_x;
        const int y = event.m_y;

        int xdelta = x - prevXPos;
        int ydelta = y - prevYPos;
        camYRotation += xdelta * -camRotWeight;
        camXRotation += ydelta * -camRotWeight;
        if (camXRotation > M_PI - 0.01f)
        {
            camXRotation = M_PI + 0.01f;
        }
        if (camXRotation < 0.01f)
        {
            camXRotation = 0.01f;
        }
        Refresh();
        prevXPos = x;
        prevYPos = y;
    }
}
