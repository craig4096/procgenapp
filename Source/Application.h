#ifndef APPLICATION_H
#define APPLICATION_H

#include <QMainWindow>
#include "TerrainModule.h"
#include "BSGModule.h"
#include "ProcTexturingModule.h"
#include "GameModule.h"

namespace Ui {
class Application;
}

class Application : public QMainWindow
{
    Q_OBJECT

    TerrainModule*       terrainModule;
    BSGModule*           bsgModule;
    ProcTexturingModule* proctexModule;
    GameModule*          gameModule;

    // need to save the central widget whenever the
    // user switches to the game demo mode so that it
    // can be retreived when they switch back
    //QWidget* centralWidget;
    
public:
    explicit Application(QWidget *parent = 0);
    ~Application();

public slots:

    // when the user begins or ends a game from
    // the menu bar the central widget will need
    // to get switched in and out
    void gameBegin();
    void gameEnd();

    void switchToTerrain();
    void switchToBSG();
    void switchToTexturing();
    
private:
    Ui::Application *ui;
};

#endif // APPLICATION_H
