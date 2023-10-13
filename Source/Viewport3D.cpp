#include "Viewport3D.h"
#include <QMouseEvent>
#include <QWheelEvent>
#include "math3d.h"
#include <iostream>
#include <QTimer>
using namespace std;

Viewport3D::Viewport3D(const std::string& skyboxDir, float zoomRate, float camMinDist, float camRotWeight, QWidget* parent)
   : QOpenGLWidget(parent)
   , renderer(0)
   , wireframe(false)
   , cameraRadius(200.0f)
   , prevXPos(0)
   , camYRotation(0.0f)
   , camXRotation(M_PI/4.0f)
   , skybox(NULL)
   , skyboxDir(skyboxDir)
   , zoomRate(zoomRate)
   , camMinDist(camMinDist)
   , camRotWeight(camRotWeight)
{
}

void Viewport3D::initializeGL() {
    //cout << "INITIALIZE GL CALLED" << endl;

    glClearColor(0.0f, 0.0f, 1.0f, 0.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // initiate glew lib
    if(glewInit() != GLEW_OK) {
        //cout << "could not initiate glew" << endl;
        return;
    }

    if(renderer != NULL) {
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

    // create a timer to regulate the drawing of the widget
    //QTimer* timer = new QTimer();
    //connect(timer, SIGNAL(timeout()), this, SLOT(updateGL()));
   // timer->start(33);
}

void Viewport3D::resizeGL(int w, int h) {
    //cout << "RESIZE GL CALLED" << endl;
    glViewport(0, 0, (GLint)w, (GLint)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if(h != 0) {
        gluPerspective(60.0f, w / (float)h, 0.01f, 10000.0f);
    } else {
        gluPerspective(60.0f, 0.0f, 0.01f, 1000.0f);
    }
    glMatrixMode(GL_MODELVIEW);
}

void Viewport3D::paintGL() {
    //cout << "PAINT GL CALLED" << endl;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    vec3 tmp(sin(camYRotation), 0.0f, cos(camYRotation));

    cameraPosition = ((tmp * sin(camXRotation)) + (vec3(0,1,0) * cos(camXRotation))) * cameraRadius;

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(cameraPosition.x,cameraPosition.y,cameraPosition.z,0,0,0,0,1,0);
    glGetFloatv(GL_MODELVIEW_MATRIX, (float*)&viewMatrix);

    skybox->Draw(cameraPosition, 1000.0f);

    if(renderer != 0) {
        //cout << "viewportDraw() called" << endl;
        renderer->viewportDraw(this);
    }
}

void Viewport3D::setRenderer(Renderer* renderer) {
    this->renderer = renderer;
}

void Viewport3D::mousePressEvent(QMouseEvent* event) {
    if(event->button() == Qt::LeftButton) {
        prevXPos = event->x();
        prevYPos = event->y();
        draggingLeftMouse = true;
    }
}

void Viewport3D::mouseReleaseEvent(QMouseEvent* event) {
    if(event->button() == Qt::LeftButton) {
        draggingLeftMouse = false;
    }
}


void Viewport3D::wheelEvent(QWheelEvent* event) {
    cameraRadius -= event->angleDelta().y() * zoomRate;
    if(cameraRadius < camMinDist) {
        cameraRadius = camMinDist;
    }
    update();
}

void Viewport3D::mouseMoveEvent(QMouseEvent* event) {
    // calculate new camera orientation
    if(draggingLeftMouse) {
        int xdelta = event->x() - prevXPos;
        int ydelta = event->y() - prevYPos;
        camYRotation += xdelta * -camRotWeight;
        camXRotation += ydelta * -camRotWeight;
        if(camXRotation > M_PI-0.01f) {
            camXRotation = M_PI+0.01f;
        }
        if(camXRotation < 0.01f) {
            camXRotation = 0.01f;
        }
        update();
        prevXPos = event->x();
        prevYPos = event->y();
    }
}
