#include "TerrainModule.h"
#include "ui_application.h"
#include <QFileDialog>
#include <QMessageBox>

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
    PopulatePropertyGrid(QStandardItemModel* model) : model(model), currentRow(0) {}

    template <class PropertyType>
    void AddProperty(const std::string& name, PropertyType& value)
    {
        QStandardItem* qname = new QStandardItem(name.c_str());
        qname->setFlags(qname->flags() & ~Qt::ItemIsEditable);
        QStandardItem* qvalue = new QStandardItem(QString::number(value));

        this->model->setItem(currentRow, 0, qname);
        this->model->setItem(currentRow, 1, qvalue);

        currentRow++;
    }

    void property(const std::string& name, int& value) override { AddProperty(name, value); }
    void property(const std::string& name, float& value) override { AddProperty(name, value); }

    QStandardItemModel* model;
    int currentRow;
};

// A class for updating the properties of a terrain operation object from the UI (i.e QStandardItemModel)
class PropertyUpdate : public IPropertyInspector
{
public:
    PropertyUpdate(QStandardItemModel* model)
        : model(model)
        , currentRow(0)
    {
    }

    void property(const std::string& name, int& value) override
    {
        value = model->item(currentRow, 1)->data(Qt::DisplayRole).toInt();
        currentRow++;
    }
    void property(const std::string& name, float& value) override
    {
        value = model->item(currentRow, 1)->data(Qt::DisplayRole).toFloat();
        currentRow++;
    }

    QStandardItemModel* model;
    int currentRow;
};



TerrainModule::TerrainModule(Ui::Application* ui, QObject *parent)
    : QObject(parent)
    , ui(ui)
    , curSelected(-1)
    , activeHeightmap(NULL)
    , generator(512, 512)
    , repopulating(false)
{
    // add operations to the ops list
    ui->terrain_oplist->addItem("Normalize");
    ui->terrain_oplist->addItem("Blend");
    ui->terrain_oplist->addItem("Modulate");
    ui->terrain_oplist->addItem("Perlin Noise");
    ui->terrain_oplist->addItem("Voronoi Diagram");
    ui->terrain_oplist->addItem("Voronoi Stencil");
    ui->terrain_oplist->addItem("Thermal Erosion");
    ui->terrain_oplist->addItem("Hydraulic Erosion");
    ui->terrain_oplist->addItem("Inverse Thermal Erosion");

    // create the gl widget
    viewport = new Viewport3D("skybox", 0.2f, 10.0f, 0.01);
    viewport->setRenderer(this);
    ui->terrain_viewport_layout->addWidget(viewport);


    connect(ui->terrain_active_oplist, SIGNAL(currentRowChanged(int)), this, SLOT(opSelected(int)));
    connect(ui->terrain_add_op, SIGNAL(clicked()), this, SLOT(addOperation()));
    connect(ui->terrain_remove_op, SIGNAL(clicked()), this, SLOT(removeOperation()));
    connect(ui->terrain_up, SIGNAL(clicked()), this, SLOT(moveOpUp()));
    connect(ui->terrain_down, SIGNAL(clicked()), this, SLOT(moveOpDown()));
    connect(ui->terrain_generate, SIGNAL(clicked()), this, SLOT(generateTerrain()));
    connect(ui->terrain_export, SIGNAL(clicked()), this, SLOT(exportTerrain()));


    // Initialise property inspector
    propertyModel = new QStandardItemModel(this);

    propertyModel->setHorizontalHeaderLabels(QStringList() << "Property" << "Value");

    ui->propertyInspector->setModel(propertyModel);

    ui->propertyInspector->setIndentation(0);
    ui->propertyInspector->header()->setStyleSheet("QHeaderView::section { font-weight: bold; }");

    connect(propertyModel, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)), this, SLOT(onPropertyChanged(const QModelIndex &, const QModelIndex &)));
}

TerrainModule::~TerrainModule()
{
    delete viewport;
}

void TerrainModule::onPropertyChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight)
{
    // Ignore any dataChanged events when repopulating the grid
    if (repopulating) return;

    TerrainOperation* terrainOp = generator.getOperation(curSelected);
    if (!terrainOp) return;

    PropertyUpdate update(propertyModel);

    terrainOp->inspect(update);
}

void TerrainModule::selectOperation(TerrainOperation* op)
{
    repopulating = true;

    // Populate the properties grid using the IPropertyInspector::inspect method of the 'op' TerrainOperation
    propertyModel->clear();
    propertyModel->setHorizontalHeaderLabels(QStringList() << "Property" << "Value");

    PopulatePropertyGrid populate(propertyModel);

    op->inspect(populate);

    repopulating = false;
}

void TerrainModule::deselectOperation(TerrainOperation* op) {
}

void TerrainModule::opSelected(int index) {
    // and thus row number does not get set
    if(curSelected != -1) {
        deselectOperation(generator.getOperation(curSelected));
    }
    if(index != -1) {
        selectOperation(generator.getOperation(index));
        // call this because this function gets called internally aswell
        ui->terrain_active_oplist->setCurrentRow(index);
    }
    curSelected = index;
}

void TerrainModule::addOperation() {
    QString name = ui->terrain_oplist->item(ui->terrain_oplist->currentRow())->text();
    if(name == "Normalize") {
        generator.addOperation(new terrain_ops::Normalization());
    } else if(name == "Perlin Noise") {
        generator.addOperation(new terrain_ops::PerlinNoise());
    } else if(name == "Voronoi Diagram") {
        generator.addOperation(new terrain_ops::VoronoiDiagram());
    } else if(name == "Voronoi Stencil") {
        generator.addOperation(new terrain_ops::VoronoiStencil());
    } else if(name == "Blend") {
        generator.addOperation(new terrain_ops::Blend());
    } else if(name == "Modulate") {
        generator.addOperation(new terrain_ops::Modulate());
    } else if(name == "Thermal Erosion") {
        generator.addOperation(new terrain_ops::ThermalErosion());
    } else if(name == "Hydraulic Erosion") {
        generator.addOperation(new terrain_ops::HydraulicErosion());
    } else if(name == "Inverse Thermal Erosion") {
        generator.addOperation(new terrain_ops::InverseThermalErosion());
    }
    updateActiveList();
}

void TerrainModule::removeOperation() {
    opSelected(curSelected-1);
    TerrainOperation* op = generator.removeOperation(curSelected+1);
    delete op;
    updateActiveList();
}

void TerrainModule::moveOpUp() {
    if(curSelected == 0 || curSelected == -1) {
        return;
    }
    generator.switchOperations(curSelected, curSelected-1);
    curSelected--;
    updateActiveList();
}

void TerrainModule::moveOpDown() {
    if(curSelected == generator.getOperationCount()-1 || curSelected == -1) {
        return;
    }
    generator.switchOperations(curSelected, curSelected+1);
    curSelected++;
    updateActiveList();
}

void TerrainModule::updateActiveList() {
    int tmpSelected = curSelected;
    // clear the list
    ui->terrain_active_oplist->clear();
    // add new items
    for(int i = 0; i < generator.getOperationCount(); ++i) {
        TerrainOperation* op = generator.getOperation(i);
        QString s = "Unknown";
        if(dynamic_cast<terrain_ops::Normalization*>(op)) {
            s = "Normalize";
        } else if(dynamic_cast<terrain_ops::PerlinNoise*>(op)) {
            s = "Perlin Noise";
        } else if(dynamic_cast<terrain_ops::VoronoiDiagram*>(op)) {
            s = "Voronoi Diagram";
        } else if(dynamic_cast<terrain_ops::VoronoiStencil*>(op)) {
            s = "Voronoi Stencil";
        } else if(dynamic_cast<terrain_ops::Blend*>(op)) {
            s = "Blend";
        } else if(dynamic_cast<terrain_ops::Modulate*>(op)) {
            s = "Modulate";
        } else if(dynamic_cast<terrain_ops::ThermalErosion*>(op)) {
            s = "Thermal Erosion";
        } else if(dynamic_cast<terrain_ops::HydraulicErosion*>(op)) {
            s = "Hydraulic Erosion";
        } else if(dynamic_cast<terrain_ops::InverseThermalErosion*>(op)) {
            s = "Inverse Thermal Erosion";
        }
        ui->terrain_active_oplist->addItem(s);
    }
    //if(prevSelected >= 0) {
    if(tmpSelected >= 0 && tmpSelected < generator.getOperationCount()) {
        opSelected(tmpSelected);
        ui->terrain_active_oplist->item(tmpSelected)->setSelected(true);
    } else {
        opSelected(-1);
    }
    //}
}

void TerrainModule::generateTerrain() {

    Heightmap* h = generator.generateTerrain(this);
    if(h == NULL) {
        return;
    }
    if(activeHeightmap) {
        delete activeHeightmap;
    }
    activeHeightmap = new RenderableHeightmap(h);
    cout << "Total Walkable Area: " << activeHeightmap->CalculateTotalWalkableArea(45.0f) << endl;
    cout << "Erosion Score: " << activeHeightmap->CalculateErosionScore() << endl;
    cout << "Num Disconnected Regions: " << activeHeightmap->CalculateNumDisconnectedRegions(45.0f) << endl;

    viewport->update();
}

void TerrainModule::exportTerrain() {
    // if no active heightmap then r5eturn
    if(activeHeightmap == NULL) {
        return;
    }

    QString filename = QFileDialog::getSaveFileName(0, "Save Terrain");
    if(filename != "") {
        QFile file(filename);
        if(!file.open(QIODevice::WriteOnly)) {
            QMessageBox::critical(0, "Export Error", "Could not open file to save terrain");
        } else {
            unsigned char* data = new unsigned char[generator.getHeightmapWidth() * generator.getHeightmapHeight() * 4];

            // get the min and max of the current heightmap
            float min = activeHeightmap->GetMinValue();
            float max = activeHeightmap->GetMaxValue();

            // fill the image data
            for(int i = 0; i < activeHeightmap->GetSize(); ++i) {
                float value = ((activeHeightmap->GetValue(i) - min) / (max - min)) * 255;
                for(int j = 0; j < 4; ++j) {
                    data[i*4+j] = value;
                }
            }
            // save the image
            QImage(data, generator.getHeightmapWidth(), generator.getHeightmapHeight(), QImage::Format_RGB32).save(filename);

            delete[] data;
        }
    }
}


void TerrainModule::viewportDraw(Viewport3D*) {
    // draw the last heightmap on the stack
    if(activeHeightmap != NULL) {
        activeHeightmap->CreateRenderData();
        activeHeightmap->Draw();
    }
}


void TerrainModule::setPercent(float f) {
    ui->terrain_progress->setMaximum(100);
    ui->terrain_progress->setValue(f);
}

void TerrainModule::nextOperation(int index) {
    // when a new operation begins in the generation process; select
    // the operation from the list to show user which operation is currently
    // being executed
    opSelected(index);
}
