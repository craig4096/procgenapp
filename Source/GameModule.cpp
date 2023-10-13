#include "GameModule.h"
#include "ui_application.h"
#include <QTimer>
#include <QKeyEvent>

GameModule::GameModule(Ui::Application* ui, QWidget *parent)
    : QOpenGLWidget(parent)
{

    game = new GameDemo();
       // GameModule is a controller for the game model - it also acts as
    // a rendering widget
    ui->game_viewport->addWidget(this);
    //updateGL();

    this->setFocusPolicy(Qt::StrongFocus);
}

GameModule::~GameModule()
{
    delete game;
}

void GameModule::initializeGL() {
    game->initGL();

    // create a timer to regulate the drawing of the widget
    QTimer* timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(33);
}

void GameModule::paintGL() {
    game->update(1.0f/30.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    game->draw();
}

void GameModule::mousePressEvent(QMouseEvent *) {
    game->mousePressed();
}

void GameModule::mouseReleaseEvent(QMouseEvent *) {
    game->mouseReleased();
}

void GameModule::keyPressEvent(QKeyEvent* event) {
    switch(event->key()) {
    case Qt::Key_W: game->move(0, true); break;
    case Qt::Key_S: game->move(1, true); break;
    case Qt::Key_A: game->move(2, true); break;
    case Qt::Key_D: game->move(3, true); break;

    case Qt::Key_Up: game->rotate(0, true); break;
    case Qt::Key_Down: game->rotate(1, true); break;
    case Qt::Key_Left: game->rotate(2, true); break;
    case Qt::Key_Right: game->rotate(3, true); break;
    }
}

void GameModule::keyReleaseEvent(QKeyEvent* event) {
    switch(event->key()) {
    case Qt::Key_W: game->move(0, false); break;
    case Qt::Key_S: game->move(1, false); break;
    case Qt::Key_A: game->move(2, false); break;
    case Qt::Key_D: game->move(3, false); break;

    case Qt::Key_Up: game->rotate(0, false); break;
    case Qt::Key_Down: game->rotate(1, false); break;
    case Qt::Key_Left: game->rotate(2, false); break;
    case Qt::Key_Right: game->rotate(3, false); break;
    }
}

void GameModule::resizeGL(int w, int h) {
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


void GameModule::begin() {
    game->generateLevel(rand());
}

void GameModule::end() {

}
