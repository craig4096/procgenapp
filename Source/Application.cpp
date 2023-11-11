#include "Application.h"
#include <iostream>
using namespace std;

wxIMPLEMENT_APP(Application);

Application::Application()
{
}

Application::~Application()
{
    delete terrainModule;
    delete bsgModule;
    delete proctexModule;
    //delete gameModule; // game module extends wxGLCanvas therefore is cleaned up automatically
}

bool Application::OnInit()
{
    wxInitAllImageHandlers();

    mainWindow = new MainWindow(nullptr);
    terrainModule = new TerrainModule(mainWindow);
    bsgModule = new BSGModule(mainWindow);
    proctexModule = new ProcTexturingModule(mainWindow);
    gameModule = new GameModule(mainWindow);

    const wxSize desktopSize = wxGetDisplaySize();
    const wxSize frameSize = wxSize(desktopSize.GetWidth() / 2, desktopSize.GetHeight() / 2);
    const int xPos = (desktopSize.GetWidth() - frameSize.GetWidth()) / 2;
    const int yPos = (desktopSize.GetHeight() - frameSize.GetHeight()) / 2;

    mainWindow->SetPosition(wxPoint(xPos, yPos));
    mainWindow->SetSize(frameSize);

    mainWindow->Show();
    return true;
}
