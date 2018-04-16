#ifndef GAMEMODULE_H
#define GAMEMODULE_H

#include "GL/glew.h"
#include <QObject>
#include <QtOpenGL/QGLWidget>
#include "Game/GameDemo.h"

namespace Ui {
    class Application;
}

class GameModule : public QGLWidget
{
    Q_OBJECT
    GameDemo* game;
public:
    explicit GameModule(Ui::Application*, QWidget *parent = 0);
    ~GameModule();

    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);

    // starts and ends the game session
    void begin();
    void end();

    void keyPressEvent(QKeyEvent*);
    void keyReleaseEvent(QKeyEvent*);

    void mousePressEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);
};

#endif // GAMEMODULE_H
