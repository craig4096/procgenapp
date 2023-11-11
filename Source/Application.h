#ifndef APPLICATION_H
#define APPLICATION_H

#include <wx/wx.h>
#include "MainWindow.h"
#include "TerrainModule.h"
#include "BSGModule.h"
#include "ProcTexturingModule.h"
#include "GameModule.h"

class Application : public wxApp
{
    TerrainModule* terrainModule;
    BSGModule* bsgModule;
    ProcTexturingModule* proctexModule;
    GameModule* gameModule;

public:
    explicit Application();
    ~Application();

    bool OnInit() override;

private:

    MainWindow* mainWindow;
};

#endif // APPLICATION_H
