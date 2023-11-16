#include "ProcTexturingModule.h"
#include <cstdio>
#include <stdexcept>
#include <wx/filedlg.h>
#include <iostream>
#include <wx/msgdlg.h>
using namespace std;

ProcTexturingModule::ProcTexturingModule(MainWindow* mainWindow)
    : mainWindow(mainWindow)
    , shaderInvalid(true)
    , shader(nullptr)
    , time(0.0f)
    , mesh(nullptr)
{
    viewport = new Viewport3D(mainWindow->proctex_viewport, this, "skybox", 0.01f, 5, 0.005f);

    mainWindow->proctex_viewport_sizer->Add(viewport, 1, wxEXPAND);

    mainWindow->Bind(wxEVT_BUTTON, [this](wxCommandEvent& event) { saveAndCompile(); }, MainWindow::ID_ProcTex_SaveAndCompile);
    mainWindow->Bind(wxEVT_BUTTON, [this](wxCommandEvent& event) { newShader(); }, MainWindow::ID_ProcTex_New);
    mainWindow->Bind(wxEVT_BUTTON, [this](wxCommandEvent& event) { loadShader(); }, MainWindow::ID_ProcTex_Load);
}

ProcTexturingModule::~ProcTexturingModule()
{
    delete shader;
    delete mesh;
}

void ProcTexturingModule::viewportInit(Viewport3D*)
{
    mesh = new Mesh("meshes/proctex_mesh.txt");

    glEnable(GL_BLEND);
    //glBlendFunc(GL_ONE, GL_ONE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ProcTexturingModule::viewportDraw(Viewport3D*)
{
    if (shaderInvalid && activeFragShaderFilename != "")
    {
        // try to load new shader
        try
        {
            shaderInvalid = false;
            Shader* tmp = new Shader("shaders/std_vertex.glslv", activeFragShaderFilename);
            // delete previous shader and assign new
            delete shader;
            shader = tmp;
            mainWindow->proctext_errors->SetValue("");
        }
        catch (std::exception& e)
        {
            mainWindow->proctext_errors->SetValue(wxString(e.what()));
        }
    }
    if (shader != nullptr)
    {
        time += 0.033f;
        shader->Set();
        glUniform1f(glGetUniformLocation(shader->GetProgram(), "time"), time);
        glUniformMatrix4fv(glGetUniformLocation(shader->GetProgram(), "viewMat"), 1, GL_FALSE, (float*)&viewport->getViewMatrix());
    }
    mesh->DrawNoTextures();
    Shader::Unset();
}

void ProcTexturingModule::saveAndCompile()
{
    // save the edited fragment shader
    if (activeFragShaderFilename.empty())
    {
        // choose file location
        wxFileDialog saveFileDialog(mainWindow, "Choose fragment shader file location", "", "", "All files (*.*)|*.*", wxFD_SAVE | wxFD_FILE_MUST_EXIST);
        if (saveFileDialog.ShowModal() == wxID_OK)
        {
            activeFragShaderFilename = saveFileDialog.GetPath().ToStdString();
        }
    }

    if (activeFragShaderFilename.empty())
    {
        return;
    }

    // save file
    std::ofstream ofs(activeFragShaderFilename.c_str());
    if (ofs.good())
    {
        ofs << mainWindow->proctex_fragedit->GetValue();
        ofs.flush();
        ofs.close();
    }
    // invalidate the shader
    shaderInvalid = true;
    viewport->Refresh();
}

void ProcTexturingModule::loadShader()
{
    wxFileDialog openFileDialog(mainWindow, "Load Shader", "", "", "All files (*.*)|*.*", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    if (openFileDialog.ShowModal() == wxID_OK)
    {
        std::string filename = openFileDialog.GetPath().ToStdString();

        // load text from file into editor
        std::ifstream ifs(filename);
        if (ifs.good())
        {
            std::string fileContents(std::istreambuf_iterator<char>{ifs}, {});

            mainWindow->proctex_fragedit->SetValue(fileContents);

            // load the new shader
            shaderInvalid = true;
            activeFragShaderFilename = filename;
        }
        else
        {
            wxMessageBox("Could not open file", "Error");
        }
    }
}

void ProcTexturingModule::newShader()
{
    mainWindow->proctex_fragedit->SetValue("");
    activeFragShaderFilename = "";
}
