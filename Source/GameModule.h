#ifndef GAMEMODULE_H
#define GAMEMODULE_H

#include "MainWindow.h"
#include "GL/glew.h"
#include "Game/GameDemo.h"
#include <wx/glcanvas.h>
#include <wx/timer.h>

class GameModule : public wxGLCanvas
{
    GameDemo* game;
    wxGLContext* context;
    wxTimer updateTimer;
    MainWindow* mainWindow;

public:
    explicit GameModule(MainWindow* mainWindow);
    ~GameModule();

    void paintGL(wxPaintEvent& event);
    void resizeGL(wxSizeEvent& event);

    void keyDown(wxKeyEvent& event);
    void keyUp(wxKeyEvent& event);
    void keyEvent(wxKeyEvent& event, bool keyDown);

    void update(wxTimerEvent& event);

    void leftDown(wxMouseEvent& event);
    void leftUp(wxMouseEvent& event);
    void mouseEnter(wxMouseEvent& event);
    void mouseExit(wxMouseEvent& event);

    DECLARE_EVENT_TABLE()
};

#endif // GAMEMODULE_H
