#include "GameModule.h"

#include <wx/wx.h>

BEGIN_EVENT_TABLE(GameModule, wxGLCanvas)
EVT_PAINT(GameModule::paintGL)
EVT_SIZE(GameModule::resizeGL)
EVT_KEY_DOWN(GameModule::keyDown)
EVT_KEY_UP(GameModule::keyUp)
EVT_LEFT_DOWN(GameModule::leftDown)
EVT_LEFT_UP(GameModule::leftUp)
EVT_ENTER_WINDOW(GameModule::mouseEnter)
EVT_LEAVE_WINDOW(GameModule::mouseExit)
END_EVENT_TABLE()

GameModule::GameModule(MainWindow* mainWindow)
    : wxGLCanvas(mainWindow->game_viewport, wxID_ANY)
    , mainWindow(mainWindow)
    , updateTimer(this)
{
    context = new wxGLContext(this);
    SetCurrent(*context);

    game = new GameDemo();

    game->initGL();

    Connect(wxID_ANY, wxEVT_TIMER, wxTimerEventHandler(GameModule::update));

    mainWindow->simplebook->Bind(wxEVT_NOTEBOOK_PAGE_CHANGED, [this](wxCommandEvent& event)
        {
            // If we have switched into the game demo page...
            if (this->mainWindow->simplebook->GetSelection() == 3)
            {
                game->generateLevel(rand());

                // start the timer to regulate the drawing of the widget
                updateTimer.Start(33);

                this->SetFocus();
            }
            else
            {
                updateTimer.Stop();
            }
        });

    mainWindow->game_viewport_sizer->Add(this, 1, wxEXPAND);
}

GameModule::~GameModule()
{
    delete game;
    delete context;
}

void GameModule::paintGL(wxPaintEvent& event)
{
    SetCurrent(*context);
    wxPaintDC(this);

    const wxSize size = GetSize();
    const int w = size.GetWidth();
    const int h = size.GetHeight();

    glViewport(0, 0, (GLint)w, (GLint)h);
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

    game->update(1.0f / 30.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    game->draw();

    glFlush();
    SwapBuffers();
}

void GameModule::resizeGL(wxSizeEvent& event)
{
    Refresh();
}

void GameModule::leftDown(wxMouseEvent& event)
{
    game->mousePressed();

    SetFocus();
}

void GameModule::leftUp(wxMouseEvent& event)
{
    game->mouseReleased();
}

void GameModule::mouseEnter(wxMouseEvent& event)
{
    game->mouseEnter();
}

void GameModule::mouseExit(wxMouseEvent& event)
{
    game->mouseExit();
}

void GameModule::keyEvent(wxKeyEvent& event, bool keyDown)
{
    wxChar uc = event.GetUnicodeKey();
    if (uc != WXK_NONE)
    {
        switch (uc)
        {
        case 'W': game->move(0, keyDown); break;
        case 'S': game->move(1, keyDown); break;
        case 'A': game->move(2, keyDown); break;
        case 'D': game->move(3, keyDown); break;
        }
    }
    else
    {
        switch (event.GetKeyCode())
        {
        case WXK_UP: game->rotate(0, keyDown); break;
        case WXK_DOWN: game->rotate(1, keyDown); break;
        case WXK_LEFT: game->rotate(2, keyDown); break;
        case WXK_RIGHT: game->rotate(3, keyDown); break;
        }
    }
}

void GameModule::keyDown(wxKeyEvent& event)
{
    keyEvent(event, true);
}

void GameModule::keyUp(wxKeyEvent& event)
{
    keyEvent(event, false);
}

void GameModule::update(wxTimerEvent& event)
{
    Refresh();
}
