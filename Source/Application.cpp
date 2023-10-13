#include "Application.h"
#include "ui_application.h"
#include <iostream>
#include <QTime>
#include <QGuiApplication>
#include <QScreen>
using namespace std;

Application::Application(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Application)
{

    ui->setupUi(this);

    // centre and size the window
    QScreen* screen = QGuiApplication::primaryScreen();
    QRect r = screen->availableGeometry();
    int width = (0.7f * r.width());
    int height = (0.6f * r.height());

    this->setGeometry(QRect((r.width() - width) / 2, (r.height() - height) / 2,
                            width, height));

    // create the three different application modules
    terrainModule = new TerrainModule(ui);
    bsgModule = new BSGModule(ui);
    proctexModule = new ProcTexturingModule(ui);
    gameModule = new GameModule(ui);

    connect(ui->gameBegin, SIGNAL(triggered()), this, SLOT(gameBegin()));

    connect(ui->actionTerrain, SIGNAL(triggered()), this, SLOT(switchToTerrain()));
    connect(ui->actionBSG, SIGNAL(triggered()), this, SLOT(switchToBSG()));
    connect(ui->actionTexturing, SIGNAL(triggered()), this, SLOT(switchToTexturing()));
}

Application::~Application()
{
    delete ui;
    delete terrainModule;
    delete bsgModule;
    delete proctexModule;
    delete gameModule;
}


void Application::gameBegin() {
    ui->stackedWidget_2->setCurrentWidget(ui->game_page);
    gameModule->begin();
}

void Application::gameEnd() {
    ui->stackedWidget_2->setCurrentWidget(ui->terrain_page);
    gameModule->end();
}


void Application::switchToTerrain() {
    ui->stackedWidget_2->setCurrentWidget(ui->terrain_page);
}

void Application::switchToBSG() {
    ui->stackedWidget_2->setCurrentWidget(ui->bsg_page);
}

void Application::switchToTexturing() {
    ui->stackedWidget_2->setCurrentWidget(ui->texturing_page);
}
