#pragma once

#include <wx/button.h>
#include <wx/colour.h>
#include <wx/frame.h>
#include <wx/gauge.h>
#include <wx/gdicmn.h>
#include <wx/listbox.h>
#include <wx/notebook.h>
#include <wx/panel.h>
#include <wx/propgrid/propgrid.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/spinctrl.h>
#include <wx/statusbr.h>
#include <wx/textctrl.h>

class MainWindow : public wxFrame
{
public:
    wxBoxSizer* bsg_view_panel_sizer;
    wxBoxSizer* game_viewport_sizer;
    wxBoxSizer* proctex_viewport_sizer;
    wxBoxSizer* terrain_viewport_sizer;
    wxGauge* terrain_progress;
    wxListBox* terrain_active_oplist;
    wxListBox* terrain_oplist;
    wxNotebook* simplebook;
    wxPanel* bsg_view_panel;
    wxPanel* game_viewport;
    wxPanel* proctex_viewport;
    wxPanel* terrain_viewport;
    wxPropertyGrid* propertyGrid;
    wxSpinCtrl* bsg_num_iterations;
    wxTextCtrl* bsg_textCtrl;
    wxTextCtrl* proctex_fragedit;
    wxTextCtrl* proctext_errors;

    MainWindow() {}
    MainWindow(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = "Procedural Generation App",
        const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_FRAME_STYLE, const wxString& name = wxFrameNameStr)
    {
        Create(parent, id, title, pos, size, style, name);
    }

    bool Create(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title =
        "Procedural Generation App", const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_FRAME_STYLE, const wxString& name = wxFrameNameStr);

    enum
    {
        ID_BSG_Apply = wxID_HIGHEST + 1,
        ID_BSG_Clear,
        ID_BSG_Iterations,
        ID_BSG_Load,
        ID_BSG_Save,
        ID_ProcTex_Load,
        ID_ProcTex_New,
        ID_ProcTex_SaveAndCompile,
        ID_Terrain_ActiveOpList,
        ID_Terrain_Add,
        ID_Terrain_Export,
        ID_Terrain_Generate,
        ID_Terrain_MoveDown,
        ID_Terrain_MoveUp,
        ID_Terrain_Remove
    };

protected:

    // Class member variables

    wxButton* bsg_apply;
    wxButton* bsg_clear;
    wxButton* bsg_load;
    wxButton* bsg_save;
    wxButton* proctex_load;
    wxButton* proctex_new;
    wxButton* proctex_sac;
    wxButton* terrain_add_op;
    wxButton* terrain_down;
    wxButton* terrain_export;
    wxButton* terrain_generate;
    wxButton* terrain_remove_op;
    wxButton* terrain_up;
    wxStatusBar* m_statusBar;
};
