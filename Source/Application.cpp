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

    wxGLAttributes displayAttributes;
    displayAttributes.RGBA().DoubleBuffer().Depth(24).EndList();

    terrainModule = new TerrainModule(mainWindow, displayAttributes);
    bsgModule = new BSGModule(mainWindow, displayAttributes);
    proctexModule = new ProcTexturingModule(mainWindow, displayAttributes);
    gameModule = new GameModule(mainWindow, displayAttributes);

    const wxSize desktopSize = wxGetDisplaySize();
    const wxSize frameSize = wxSize(desktopSize.GetWidth() / 2, desktopSize.GetHeight() / 2);
    const int xPos = (desktopSize.GetWidth() - frameSize.GetWidth()) / 2;
    const int yPos = (desktopSize.GetHeight() - frameSize.GetHeight()) / 2;

    mainWindow->SetPosition(wxPoint(xPos, yPos));
    mainWindow->SetSize(frameSize);

    mainWindow->Show();
    return true;
}
