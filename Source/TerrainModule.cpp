#include "TerrainModule.h"
#include <wx/filedlg.h>
#include <wx/msgdlg.h>
#include <fstream>
#include <glm/ext.hpp>

// include terrain operations
#include "Terrain/Normalization.h"
#include "Terrain/PerlinNoise.h"
#include "Terrain/VoronoiDiagram.h"
#include "Terrain/VoronoiStencil.h"
#include "Terrain/Blend.h"
#include "Terrain/Modulate.h"
#include "Terrain/ThermalErosion.h"
#include "Terrain/HydraulicErosion.h"
#include "Terrain/InverseThermalErosion.h"
#include <iostream>
using namespace std;


// Property Inspector class for populating the property grid
class PopulatePropertyGrid : public IPropertyInspector
{
public:
    PopulatePropertyGrid(wxPropertyGrid* grid) : grid(grid) {}

    void property(const std::string& name, int& value) override
    {
        grid->Append(new wxIntProperty(name, name, value));
    }
    void property(const std::string& name, float& value) override
    {
        grid->Append(new wxFloatProperty(name, name, value));
    }

    wxPropertyGrid* grid;
};

// A class for updating the properties of a terrain operation object from the UI (i.e QStandardItemModel)
class PropertyUpdate : public IPropertyInspector
{
public:
    PropertyUpdate(wxPropertyGrid* grid) : grid(grid) {}

    void property(const std::string& name, int& value) override
    {
        if (wxPGProperty* prop = grid->GetProperty(name))
        {
            value = prop->GetValue().GetInteger();
        }
    }
    void property(const std::string& name, float& value) override
    {
        if (wxPGProperty* prop = grid->GetProperty(name))
        {
            value = prop->GetValue().GetDouble();
        }
    }

    wxPropertyGrid* grid;
};


TerrainModule::TerrainModule(MainWindow* mainWindow)
    : mainWindow(mainWindow)
    , curSelected(-1)
    , activeHeightmap(nullptr)
    , generator(512, 512)
    , repopulating(false)
{
    // add operations to the ops list
    mainWindow->terrain_oplist->Append("Normalize");
    mainWindow->terrain_oplist->Append("Blend");
    mainWindow->terrain_oplist->Append("Modulate");
    mainWindow->terrain_oplist->Append("Perlin Noise");
    mainWindow->terrain_oplist->Append("Voronoi Diagram");
    mainWindow->terrain_oplist->Append("Voronoi Stencil");
    mainWindow->terrain_oplist->Append("Thermal Erosion");
    mainWindow->terrain_oplist->Append("Hydraulic Erosion");
    mainWindow->terrain_oplist->Append("Inverse Thermal Erosion");

    // create the gl widget
    viewport = new Viewport3D(mainWindow->terrain_viewport, this, "skybox", 0.2f, 10.0f, 0.01);

    mainWindow->terrain_viewport_sizer->Add(viewport, 1, wxEXPAND);

    mainWindow->Bind(wxEVT_LISTBOX, [this](wxCommandEvent& event) {
        opSelected(this->mainWindow->terrain_active_oplist->GetSelection());
        }, MainWindow::ID_Terrain_ActiveOpList);

    mainWindow->Bind(wxEVT_BUTTON, [this](wxCommandEvent& event) { addOperation(); }, MainWindow::ID_Terrain_Add);
    mainWindow->Bind(wxEVT_BUTTON, [this](wxCommandEvent& event) { removeOperation(); }, MainWindow::ID_Terrain_Remove);
    mainWindow->Bind(wxEVT_BUTTON, [this](wxCommandEvent& event) { moveOpUp(); }, MainWindow::ID_Terrain_MoveUp);
    mainWindow->Bind(wxEVT_BUTTON, [this](wxCommandEvent& event) { moveOpDown(); }, MainWindow::ID_Terrain_MoveDown);
    mainWindow->Bind(wxEVT_BUTTON, [this](wxCommandEvent& event) { generateTerrain(); }, MainWindow::ID_Terrain_Generate);
    mainWindow->Bind(wxEVT_BUTTON, [this](wxCommandEvent& event) { exportTerrain(); }, MainWindow::ID_Terrain_Export);

    mainWindow->Bind(wxEVT_PG_CHANGED, [this](wxPropertyGridEvent& event)
        {
            // Ignore any dataChanged events when repopulating the grid
            if (repopulating) return;

            TerrainOperation* terrainOp = generator.getOperation(this->mainWindow->terrain_active_oplist->GetSelection());
            if (!terrainOp) return;

            PropertyUpdate update(this->mainWindow->propertyGrid);

            terrainOp->inspect(update);
        });
}

TerrainModule::~TerrainModule()
{
}

void TerrainModule::selectOperation(TerrainOperation* op)
{
    repopulating = true;

    // Populate the properties grid using the IPropertyInspector::inspect method of the 'op' TerrainOperation
    mainWindow->propertyGrid->Clear();

    mainWindow->propertyGrid->Append(new wxPropertyCategory("Terrain operation"));

    PopulatePropertyGrid populate(mainWindow->propertyGrid);

    op->inspect(populate);

    repopulating = false;
}

void TerrainModule::opSelected(int index)
{
    // and thus row number does not get set
    if (index != -1)
    {
        selectOperation(generator.getOperation(index));
        // call this because this function gets called internally aswell
        mainWindow->terrain_active_oplist->SetSelection(index);
    }
    curSelected = index;
}

void TerrainModule::addOperation()
{
    wxString name = mainWindow->terrain_oplist->GetStringSelection();
    if (name == "Normalize")
    {
        generator.addOperation(new terrain_ops::Normalization());
    }
    else if (name == "Perlin Noise")
    {
        generator.addOperation(new terrain_ops::PerlinNoise());
    }
    else if (name == "Voronoi Diagram")
    {
        generator.addOperation(new terrain_ops::VoronoiDiagram());
    }
    else if (name == "Voronoi Stencil")
    {
        generator.addOperation(new terrain_ops::VoronoiStencil());
    }
    else if (name == "Blend")
    {
        generator.addOperation(new terrain_ops::Blend());
    }
    else if (name == "Modulate")
    {
        generator.addOperation(new terrain_ops::Modulate());
    }
    else if (name == "Thermal Erosion")
    {
        generator.addOperation(new terrain_ops::ThermalErosion());
    }
    else if (name == "Hydraulic Erosion")
    {
        generator.addOperation(new terrain_ops::HydraulicErosion());
    }
    else if (name == "Inverse Thermal Erosion")
    {
        generator.addOperation(new terrain_ops::InverseThermalErosion());
    }
    updateActiveList();
}

void TerrainModule::removeOperation()
{
    opSelected(curSelected - 1);
    TerrainOperation* op = generator.removeOperation(curSelected + 1);
    delete op;
    updateActiveList();
}

void TerrainModule::moveOpUp()
{
    if (curSelected == 0 || curSelected == -1)
    {
        return;
    }
    generator.switchOperations(curSelected, curSelected - 1);
    curSelected--;
    updateActiveList();
}

void TerrainModule::moveOpDown()
{
    if (curSelected == generator.getOperationCount() - 1 || curSelected == -1)
    {
        return;
    }
    generator.switchOperations(curSelected, curSelected + 1);
    curSelected++;
    updateActiveList();
}

void TerrainModule::updateActiveList()
{
    int tmpSelected = curSelected;
    // clear the list
    mainWindow->terrain_active_oplist->Clear();
    // add new items
    for (int i = 0; i < generator.getOperationCount(); ++i)
    {
        TerrainOperation* op = generator.getOperation(i);
        wxString s = "Unknown";
        if (dynamic_cast<terrain_ops::Normalization*>(op))
        {
            s = "Normalize";
        }
        else if (dynamic_cast<terrain_ops::PerlinNoise*>(op))
        {
            s = "Perlin Noise";
        }
        else if (dynamic_cast<terrain_ops::VoronoiDiagram*>(op))
        {
            s = "Voronoi Diagram";
        }
        else if (dynamic_cast<terrain_ops::VoronoiStencil*>(op))
        {
            s = "Voronoi Stencil";
        }
        else if (dynamic_cast<terrain_ops::Blend*>(op))
        {
            s = "Blend";
        }
        else if (dynamic_cast<terrain_ops::Modulate*>(op))
        {
            s = "Modulate";
        }
        else if (dynamic_cast<terrain_ops::ThermalErosion*>(op))
        {
            s = "Thermal Erosion";
        }
        else if (dynamic_cast<terrain_ops::HydraulicErosion*>(op))
        {
            s = "Hydraulic Erosion";
        }
        else if (dynamic_cast<terrain_ops::InverseThermalErosion*>(op))
        {
            s = "Inverse Thermal Erosion";
        }
        mainWindow->terrain_active_oplist->Append(s);
    }
#
    if (tmpSelected >= 0 && tmpSelected < generator.getOperationCount())
    {
        opSelected(tmpSelected);
        mainWindow->terrain_active_oplist->SetSelection(tmpSelected);
    }
    else
    {
        opSelected(-1);
    }
}

void TerrainModule::generateTerrain()
{
    Heightmap* h = generator.generateTerrain(this);
    if (h == nullptr)
    {
        return;
    }
    if (activeHeightmap)
    {
        delete activeHeightmap;
    }
    activeHeightmap = new RenderableHeightmap(h);
    cout << "Total Walkable Area: " << activeHeightmap->CalculateTotalWalkableArea(45.0f) << endl;
    cout << "Erosion Score: " << activeHeightmap->CalculateErosionScore() << endl;
    cout << "Num Disconnected Regions: " << activeHeightmap->CalculateNumDisconnectedRegions(45.0f) << endl;

    viewport->Refresh();
}

void TerrainModule::exportTerrain()
{
    // if no active heightmap then r5eturn
    if (activeHeightmap == nullptr)
    {
        return;
    }

    wxFileDialog saveFileDialog(mainWindow, "Save Grammar file", "", "", "", wxFD_SAVE);
    if (saveFileDialog.ShowModal() == wxID_OK)
    {
        std::string filename = saveFileDialog.GetPath().ToStdString();

        std::ofstream ofs(filename.c_str());
        if (!ofs.good())
        {
            wxMessageBox("Could not open file to save terrain", "Export Error");
        }
        else
        {
            unsigned char* data = new unsigned char[generator.getHeightmapWidth() * generator.getHeightmapHeight() * 3];

            // get the min and max of the current heightmap
            float min = activeHeightmap->GetMinValue();
            float max = activeHeightmap->GetMaxValue();

            // fill the image data
            int i = 0;
            for (int y = 0; y < activeHeightmap->GetHeight(); ++y)
            {
                for (int x = 0; x < activeHeightmap->GetWidth(); ++x)
                {
                    float value = ((activeHeightmap->GetValue(x, y) - min) / (max - min)) * 255;

                    data[i++] = value;
                    data[i++] = value;
                    data[i++] = value;
                }
            }
            // save the image
            wxImage img(generator.getHeightmapWidth(), generator.getHeightmapHeight(), data, true);

            img.SaveFile(filename);

            delete[] data;
        }
    }
}

void TerrainModule::viewportInit(Viewport3D*)
{
    shader = std::make_unique<Shader>("shaders/std_vertex.glslv", "shaders/std_frag.glslf");
}

void TerrainModule::viewportDraw(Viewport3D*)
{
    // draw the last heightmap on the stack
    if (activeHeightmap != nullptr)
    {
        activeHeightmap->CreateRenderData();

        shader->Set();
        glUniformMatrix4fv(glGetUniformLocation(shader->GetProgram(), "viewMat"), 1, GL_FALSE, glm::value_ptr(viewport->getModelViewMatrix()));
        glUniformMatrix4fv(glGetUniformLocation(shader->GetProgram(), "modelViewProjectionMatrix"), 1, GL_FALSE, glm::value_ptr(viewport->getModelViewProjectionMatrix()));
        glUniformMatrix4fv(glGetUniformLocation(shader->GetProgram(), "normalMatrix"), 1, GL_FALSE, glm::value_ptr(viewport->getNormalMatrix()));

        activeHeightmap->Draw(*shader);

        shader->Unset();
    }
}

void TerrainModule::setPercent(float f)
{
    mainWindow->terrain_progress->SetRange(100);
    mainWindow->terrain_progress->SetValue(f);
}

void TerrainModule::nextOperation(int index)
{
    // when a new operation begins in the generation process; select
    // the operation from the list to show user which operation is currently
    // being executed
    opSelected(index);
}
