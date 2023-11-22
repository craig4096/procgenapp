#include "BSGModule.h"
#include <wx/wx.h>
#include <stdexcept>
#include <iostream>
#include <glm/ext.hpp>
using namespace std;

BSGModule::BSGModule(MainWindow* mainWindow, const wxGLAttributes& displayAttributes)
    : activeGrammar(nullptr)
    , numIterations(1)
    , mainWindow(mainWindow)
{
    // create the gl widget
    viewport = new Viewport3D(mainWindow->bsg_view_panel, displayAttributes, this, "skybox", 0.01f, 10.0f, 0.01);

    mainWindow->bsg_view_panel_sizer->Add(viewport, 1, wxEXPAND);

    mainWindow->Bind(wxEVT_BUTTON, [this](wxCommandEvent& event) { load(); }, MainWindow::ID_BSG_Load);
    mainWindow->Bind(wxEVT_BUTTON, [this](wxCommandEvent& event) { save(); }, MainWindow::ID_BSG_Save);
    mainWindow->Bind(wxEVT_BUTTON, [this](wxCommandEvent& event) { clear(); }, MainWindow::ID_BSG_Clear);
    mainWindow->Bind(wxEVT_BUTTON, [this](wxCommandEvent& event) { apply(); }, MainWindow::ID_BSG_Apply);
    mainWindow->Bind(wxEVT_SPINCTRL, [this](wxSpinEvent& event) { setNumIterations(event.GetInt()); }, MainWindow::ID_BSG_Iterations);

    mainWindow->bsg_num_iterations->SetValue(numIterations);
}

BSGModule::~BSGModule()
{
}

void BSGModule::viewportInit(Viewport3D*)
{
    // load a std global symbol map
    shapeDatabase.loadSymbolMeshMap("grammars/castle/mapping.txt");
    // axiom shape FIXME?
    Shape shape;
    shape.position.y = 1.0f;
    shape.yRotation = 0.0f;
    shape.symbol = "castle_entrance";
    shape.terminal = false;
    shape.calculateBoundingBox(shapeDatabase.getSymbolMeshMap());
    shapeDatabase.insert(shapeDatabase.begin(), shape);

    shader = std::make_unique<Shader>("shaders/std_vertex.glslv", "shaders/std_frag.glslf");
}

void BSGModule::viewportDraw(Viewport3D*)
{
    // enable lighting
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glShadeModel(GL_FLAT);

    GLfloat ambient[4] = { 0,0,0,0 };
    GLfloat diffuse[4] = { 1,1,1,1 };
    GLfloat specular[4] = { 0,0,0,0 };
    GLfloat emission[4] = { 0,0,0,0 };

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    glLightfv(GL_LIGHT0, GL_EMISSION, emission);

    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT, GL_EMISSION, emission);

    vec3 dir(0.5, 0.5, 1);
    dir.normalize();

    GLfloat pos[4] = { dir.x,dir.y,dir.z,0 };
    glLightfv(GL_LIGHT0, GL_POSITION, pos);

    // draw the model
    for (ShapeDatabase::iterator i = shapeDatabase.begin(); i != shapeDatabase.end(); ++i)
    {
        const Shape& shape = (*i);
        glPushMatrix();
        glTranslatef(shape.position.x, shape.position.y, shape.position.z);
        glRotatef(shape.yRotation, 0, 1, 0);
        viewport->updateMatricesFromGL();

        shader->Set();
        glUniformMatrix4fv(glGetUniformLocation(shader->GetProgram(), "viewMat"), 1, GL_FALSE, glm::value_ptr(viewport->getModelViewMatrix()));
        glUniformMatrix4fv(glGetUniformLocation(shader->GetProgram(), "modelViewProjectionMatrix"), 1, GL_FALSE, glm::value_ptr(viewport->getModelViewProjectionMatrix()));
        glUniformMatrix4fv(glGetUniformLocation(shader->GetProgram(), "normalMatrix"), 1, GL_FALSE, glm::value_ptr(viewport->getNormalMatrix()));

        shapeDatabase.getSymbolMeshMap().find(shape.symbol)->second->Draw(*shader);

        shader->Unset();

        glPopMatrix();
        viewport->updateMatricesFromGL();
    }

    glColor3f(1, 1, 1);
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);

#ifdef WIREFRAME
    glColor3f(0, 0, 0);
    glLineWidth(2.0);
    glPolygonMode(GL_FRONT, GL_LINE);
    // draw the model
    for (ShapeDatabase::iterator i = shapeDatabase.begin(); i != shapeDatabase.end(); ++i)
    {
        const Shape& shape = (*i);
        glPushMatrix();
        glTranslatef(shape.position.x, shape.position.y, shape.position.z);
        glRotatef(shape.yRotation + 180, 0, 1, 0);
        viewport->updateMatricesFromGL();

        shader->Set();
        glUniformMatrix4fv(glGetUniformLocation(shader->GetProgram(), "viewMat"), 1, GL_FALSE, glm::value_ptr(viewport->getModelViewMatrix()));
        glUniformMatrix4fv(glGetUniformLocation(shader->GetProgram(), "modelViewProjectionMatrix"), 1, GL_FALSE, glm::value_ptr(viewport->getModelViewProjectionMatrix()));
        glUniformMatrix4fv(glGetUniformLocation(shader->GetProgram(), "normalMatrix"), 1, GL_FALSE, glm::value_ptr(viewport->getNormalMatrix()));

        shapeDatabase.getSymbolMeshMap().find(shape.symbol)->second->Draw(*shader);

        shader->Unset();

        glPopMatrix();
        viewport->updateMatricesFromGL();
    }

    glPolygonMode(GL_FRONT, GL_FILL);
    glColor3f(1, 1, 1);
    glLineWidth(1.0);
#endif
}

void BSGModule::load()
{
    wxFileDialog openFileDialog(mainWindow, "Open Grammar file", "", "", "All files (*.*)|*.*", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    if (openFileDialog.ShowModal() == wxID_OK)
    {
        //std::string filename = str.data();
        std::string filename = openFileDialog.GetPath().ToStdString();
        if (!filename.empty())
        {
            if (activeGrammar != nullptr)
            {
                delete activeGrammar;
            }
            // load text from file into editor
            std::ifstream ifs(filename.c_str());
            if (!ifs.good())
            {
                wxMessageBox("Could not open file", "Error");
            }

            std::string fileContents(std::istreambuf_iterator<char>{ifs}, {});

            mainWindow->bsg_textCtrl->SetValue(fileContents);

            // now the file text has been set, parse the actual file
            activeGrammar = new BasicShapeGrammar(filename);
            currentOpenFile = filename;
        }
    }
}

void BSGModule::save()
{
    // if there is no active save file then prompt the user for one
    std::string filename;
    if (currentOpenFile.empty())
    {
        wxFileDialog saveFileDialog(mainWindow, "Save Grammar file", "", "", "", wxFD_SAVE);

        if (saveFileDialog.ShowModal() == wxID_OK)
        {
            filename = saveFileDialog.GetPath().ToStdString();
            currentOpenFile = filename;
        }
    }
    else
    {
        filename = currentOpenFile;
    }

    std::ofstream file(filename.c_str());
    if (!file.good())
    {
        wxMessageBox("Could not open file", "Error");
    }
    else
    {
        file << mainWindow->bsg_textCtrl->GetValue().ToStdString();
        file.close();
    }
    // now the file has been saved we need to update the active grammar
    // delete old one
    delete activeGrammar;
    activeGrammar = new BasicShapeGrammar(currentOpenFile);
}

void BSGModule::clear()
{
    shapeDatabase.clear();

    // axiom shape FIXME
    Shape shape;
    shape.position.y = 1.0f;
    shape.yRotation = 0.0f;
    shape.symbol = "castle_entrance";
    shape.terminal = false;
    shape.calculateBoundingBox(shapeDatabase.getSymbolMeshMap());
    shapeDatabase.insert(shapeDatabase.begin(), shape);

    viewport->Refresh();
}

void BSGModule::apply()
{
    if (activeGrammar != nullptr)
    {
        activeGrammar->generate(shapeDatabase, numIterations);
        viewport->Refresh();
    }
}

void BSGModule::setNumIterations(int numIterations)
{
    this->numIterations = numIterations;
}
