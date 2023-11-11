#include "mainwindow.h"

bool MainWindow::Create(wxWindow* parent, wxWindowID id, const wxString& title,
    const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{

    if (!wxFrame::Create(parent, id, title, pos, size, style, name))
        return false;

    m_statusBar = CreateStatusBar();

    simplebook = new wxNotebook(this, wxID_ANY);

    auto* terrain_page = new wxPanel(simplebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    simplebook->AddPage(terrain_page, "Terrain");
    terrain_page->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));

    auto* terrain_page_sizer = new wxBoxSizer(wxHORIZONTAL);

    auto* terrain_left_panel = new wxPanel(terrain_page, wxID_ANY, wxDefaultPosition, wxDefaultSize,
        wxTAB_TRAVERSAL);
    terrain_page_sizer->Add(terrain_left_panel, wxSizerFlags(1).Expand().Border(wxALL));

    auto* terrain_left_panel_sizer = new wxBoxSizer(wxVERTICAL);

    propertyGrid = new wxPropertyGrid(terrain_left_panel, wxID_ANY);
    terrain_left_panel_sizer->Add(propertyGrid, wxSizerFlags(1).Expand().Border(wxALL));

    auto* box_sizer_2 = new wxBoxSizer(wxHORIZONTAL);

    auto* panel = new wxPanel(terrain_left_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    box_sizer_2->Add(panel, wxSizerFlags(1).Expand().Border(wxALL, 0));

    auto* box_sizer_3 = new wxBoxSizer(wxVERTICAL);

    terrain_oplist = new wxListBox(panel, wxID_ANY);
    box_sizer_3->Add(terrain_oplist, wxSizerFlags(1).Expand().Border(wxALL));

    terrain_add_op = new wxButton(panel, ID_Terrain_Add, "Add");
    box_sizer_3->Add(terrain_add_op, wxSizerFlags().Expand().Border(wxALL));

    terrain_remove_op = new wxButton(panel, ID_Terrain_Remove, "Remove");
    box_sizer_3->Add(terrain_remove_op, wxSizerFlags().Expand().Border(wxALL));

    terrain_generate = new wxButton(panel, ID_Terrain_Generate, "Generate terrain");
    box_sizer_3->Add(terrain_generate, wxSizerFlags().Expand().Border(wxALL));
    panel->SetSizerAndFit(box_sizer_3);

    auto* panel_2 = new wxPanel(terrain_left_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize,
        wxTAB_TRAVERSAL);
    box_sizer_2->Add(panel_2, wxSizerFlags(1).Expand().Border(wxALL, 0));

    auto* box_sizer_4 = new wxBoxSizer(wxVERTICAL);

    terrain_active_oplist = new wxListBox(panel_2, ID_Terrain_ActiveOpList);
    box_sizer_4->Add(terrain_active_oplist, wxSizerFlags(1).Expand().Border(wxALL));

    terrain_up = new wxButton(panel_2, ID_Terrain_MoveUp, "Up");
    box_sizer_4->Add(terrain_up, wxSizerFlags().Expand().Border(wxALL));

    terrain_down = new wxButton(panel_2, ID_Terrain_MoveDown, "Down");
    box_sizer_4->Add(terrain_down, wxSizerFlags().Expand().Border(wxALL));

    terrain_export = new wxButton(panel_2, ID_Terrain_Export, "Export");
    box_sizer_4->Add(terrain_export, wxSizerFlags().Expand().Border(wxALL));
    panel_2->SetSizerAndFit(box_sizer_4);

    terrain_left_panel_sizer->Add(box_sizer_2, wxSizerFlags(1).Expand());

    terrain_progress = new wxGauge(terrain_left_panel, wxID_ANY, 100);
    terrain_progress->SetValue(0);
    terrain_left_panel_sizer->Add(terrain_progress, wxSizerFlags().Expand().Border(wxALL));
    terrain_left_panel->SetSizerAndFit(terrain_left_panel_sizer);

    terrain_viewport = new wxPanel(terrain_page, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    terrain_page_sizer->Add(terrain_viewport, wxSizerFlags(2).Expand().Border(wxALL));

    terrain_viewport_sizer = new wxBoxSizer(wxVERTICAL);
    terrain_viewport->SetSizerAndFit(terrain_viewport_sizer);
    terrain_page->SetSizerAndFit(terrain_page_sizer);

    auto* bsg_page = new wxPanel(simplebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    simplebook->AddPage(bsg_page, "Structural");
    bsg_page->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));

    auto* bsg_page_sizer = new wxBoxSizer(wxHORIZONTAL);

    auto* bsg_text_panel_sizer = new wxBoxSizer(wxVERTICAL);

    bsg_textCtrl = new wxTextCtrl(bsg_page, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
        wxTE_MULTILINE);
    bsg_text_panel_sizer->Add(bsg_textCtrl, wxSizerFlags(1).Expand().Border(wxALL));

    auto* box_sizer = new wxBoxSizer(wxHORIZONTAL);

    bsg_load = new wxButton(bsg_page, ID_BSG_Load, "Load");
    box_sizer->Add(bsg_load, wxSizerFlags(1).Border(wxALL));

    bsg_save = new wxButton(bsg_page, ID_BSG_Save, "Save");
    box_sizer->Add(bsg_save, wxSizerFlags(1).Border(wxALL));

    bsg_clear = new wxButton(bsg_page, ID_BSG_Clear, "Clear");
    box_sizer->Add(bsg_clear, wxSizerFlags(1).Border(wxALL));

    bsg_apply = new wxButton(bsg_page, ID_BSG_Apply, "Apply");
    box_sizer->Add(bsg_apply, wxSizerFlags(1).Border(wxALL));

    bsg_num_iterations = new wxSpinCtrl(bsg_page, ID_BSG_Iterations);
    box_sizer->Add(bsg_num_iterations, wxSizerFlags(1).Border(wxALL));

    bsg_text_panel_sizer->Add(box_sizer, wxSizerFlags().Expand().Border(wxALL));

    bsg_page_sizer->Add(bsg_text_panel_sizer, wxSizerFlags(1).Expand().Border(wxALL));

    bsg_view_panel = new wxPanel(bsg_page, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    bsg_page_sizer->Add(bsg_view_panel, wxSizerFlags(1).Expand().Border(wxALL));

    bsg_view_panel_sizer = new wxBoxSizer(wxVERTICAL);
    bsg_view_panel->SetSizerAndFit(bsg_view_panel_sizer);
    bsg_page->SetSizerAndFit(bsg_page_sizer);

    auto* texturing_page = new wxPanel(simplebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    simplebook->AddPage(texturing_page, "Texturing");
    texturing_page->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));

    auto* texturing_page_sizer = new wxBoxSizer(wxHORIZONTAL);

    auto* box_sizer_6 = new wxBoxSizer(wxVERTICAL);

    proctex_fragedit = new wxTextCtrl(texturing_page, wxID_ANY, wxEmptyString, wxDefaultPosition,
        wxDefaultSize, wxTE_MULTILINE);
    box_sizer_6->Add(proctex_fragedit, wxSizerFlags(3).Expand().Border(wxALL));

    proctext_errors = new wxTextCtrl(texturing_page, wxID_ANY, wxEmptyString, wxDefaultPosition,
        wxDefaultSize, wxTE_MULTILINE);
    box_sizer_6->Add(proctext_errors, wxSizerFlags(1).Expand().Border(wxALL));

    auto* box_sizer_7 = new wxBoxSizer(wxHORIZONTAL);

    proctex_sac = new wxButton(texturing_page, ID_ProcTex_SaveAndCompile, "Save and Compile");
    box_sizer_7->Add(proctex_sac, wxSizerFlags(1).Border(wxALL));

    proctex_load = new wxButton(texturing_page, ID_ProcTex_Load, "Load");
    box_sizer_7->Add(proctex_load, wxSizerFlags(1).Border(wxALL));

    proctex_new = new wxButton(texturing_page, ID_ProcTex_New, "New");
    box_sizer_7->Add(proctex_new, wxSizerFlags(1).Border(wxALL));

    box_sizer_6->Add(box_sizer_7, wxSizerFlags().Expand().Border(wxALL));

    texturing_page_sizer->Add(box_sizer_6, wxSizerFlags(1).Expand().Border(wxALL));

    proctex_viewport = new wxPanel(texturing_page, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    texturing_page_sizer->Add(proctex_viewport, wxSizerFlags(1).Expand().Border(wxALL));

    proctex_viewport_sizer = new wxBoxSizer(wxVERTICAL);
    proctex_viewport->SetSizerAndFit(proctex_viewport_sizer);
    texturing_page->SetSizerAndFit(texturing_page_sizer);

    auto* game_page = new wxPanel(simplebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    simplebook->AddPage(game_page, "Game Demo");
    game_page->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));

    auto* box_sizer_5 = new wxBoxSizer(wxHORIZONTAL);

    game_viewport = new wxPanel(game_page, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    game_viewport->SetBackgroundColour(wxColour(0, 0, 0));
    box_sizer_5->Add(game_viewport, wxSizerFlags(1).Expand().Border(wxALL));

    game_viewport_sizer = new wxBoxSizer(wxVERTICAL);
    game_viewport->SetSizerAndFit(game_viewport_sizer);
    game_page->SetSizerAndFit(box_sizer_5);


    Centre(wxBOTH);

    return true;
}
