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

TerrainModule::TerrainModule(Ui::Application* ui, QObject *parent)
    : QObject(parent)
    , ui(ui)
    , curSelected(-1)
    , activeHeightmap(NULL)
    , generator(512, 512)
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
}

TerrainModule::~TerrainModule()
{
    delete viewport;
}

// mappings between the 'TerrainModule's slots and current operations functions (should maybe create class per-operation)
void TerrainModule::t_normSetMin(double d) { getCurOp<terrain_ops::Normalization>()->setMinRange(d); }
void TerrainModule::t_normSetMax(double d) { getCurOp<terrain_ops::Normalization>()->setMaxRange(d); }

void TerrainModule::t_setBlendFactor(int i) { getCurOp<terrain_ops::Blend>()->setBlendFactor((i/(float)1000.0f)); }

void TerrainModule::t_perlinSetEndOctave(int i) { getCurOp<terrain_ops::PerlinNoise>()->setEndOctave(i); }
void TerrainModule::t_perlinSetStartOctave(int i) { getCurOp<terrain_ops::PerlinNoise>()->setStartOctave(i); }
void TerrainModule::t_perlinSetPersistance(int i) { getCurOp<terrain_ops::PerlinNoise>()->setPersistance(i/(float)1000.0f); }
void TerrainModule::t_perlinSetSeed(int i) { getCurOp<terrain_ops::PerlinNoise>()->setSeed(i); }

void TerrainModule::t_voronoiSetNumPoints(int i) { getCurOp<terrain_ops::VoronoiDiagram>()->setNumFeaturePoints(i); }
void TerrainModule::t_voronoiSetBlendA(double d) { getCurOp<terrain_ops::VoronoiDiagram>()->setBlendWeightA(d); }
void TerrainModule::t_voronoiSetBlendB(double d) { getCurOp<terrain_ops::VoronoiDiagram>()->setBlendWeightB(d); }
void TerrainModule::t_voronoiSetSeed(int i) { getCurOp<terrain_ops::VoronoiDiagram>()->setSeed(i); }

void TerrainModule::t_vstencilSetSeed(int i) { getCurOp<terrain_ops::VoronoiStencil>()->setSeed(i); }
void TerrainModule::t_vstencilSetNumPoints(int i) { getCurOp<terrain_ops::VoronoiStencil>()->setNumFeaturePoints(i); }
void TerrainModule::t_vstencilSetBlackValue(double d) { getCurOp<terrain_ops::VoronoiStencil>()->setBlackValue(d); }
void TerrainModule::t_vstencilSetWhiteValue(double d) { getCurOp<terrain_ops::VoronoiStencil>()->setWhiteValue(d); }
void TerrainModule::t_vstencilSetProbability(int i) { getCurOp<terrain_ops::VoronoiStencil>()->setProbabilityFactor(i/(float)1000.0f); }

void TerrainModule::t_thermalSetNumIterations(int i) { getCurOp<terrain_ops::ThermalErosion>()->setNumIterations(i); }
void TerrainModule::t_thermalSetTalusAngle(double d) { getCurOp<terrain_ops::ThermalErosion>()->setTalusAngle(d); }

void TerrainModule::t_hydraulicSetWaterConst(double d) { getCurOp<terrain_ops::HydraulicErosion>()->setWaterConst(d); }
void TerrainModule::t_hydraulicSetSolubility(double d) { getCurOp<terrain_ops::HydraulicErosion>()->setSolubility(d); }
void TerrainModule::t_hydraulicSetEvaporationCoef(double d) { getCurOp<terrain_ops::HydraulicErosion>()->setEvaporationCoefficient(d); }
void TerrainModule::t_hydraulicSetSedimentCapacity(double d) { getCurOp<terrain_ops::HydraulicErosion>()->setSedimentCapacity(d); }
void TerrainModule::t_hydraulicSetNumIterations(int i) { getCurOp<terrain_ops::HydraulicErosion>()->setNumIterations(i); }

void TerrainModule::t_invthermalSetNumIterations(int i) { getCurOp<terrain_ops::InverseThermalErosion>()->setNumIterations(i); }
void TerrainModule::t_invthermalSetTalusAngle(double d) { getCurOp<terrain_ops::InverseThermalErosion>()->setTalusAngle(d); }

void TerrainModule::selectOperation(TerrainOperation* op) {
    // When an operation is selected a qt 'stacked widget' will appear showing the operations
    // specific parameters, each sub-widget needs to be connected to slots of this class - when
    // they are called they will retrieve the current operation and set it's parameters

    // check which type of operation has been selected
    if(dynamic_cast<terrain_ops::Normalization*>(op)) {
        // Normalization
        terrain_ops::Normalization* tn = (terrain_ops::Normalization*)op;
        ui->stackedWidget->setCurrentWidget(ui->terrain_normalization);
        ui->terrain_norm_min->setValue(tn->getMinRange());
        ui->terrain_norm_max->setValue(tn->getMaxRange());
        connect(ui->terrain_norm_max, SIGNAL(valueChanged(double)), this, SLOT(t_normSetMax(double)));
        connect(ui->terrain_norm_min, SIGNAL(valueChanged(double)), this, SLOT(t_normSetMin(double)));
    } else if(dynamic_cast<terrain_ops::Blend*>(op)) {
        // Blending
        terrain_ops::Blend* b = (terrain_ops::Blend*)op;
        ui->stackedWidget->setCurrentWidget(ui->terrain_blend);
        ui->terrain_blend_factor->setValue(b->getBlendFactor() * 1000);
        connect(ui->terrain_blend_factor, SIGNAL(valueChanged(int)), this, SLOT(t_setBlendFactor(int)));
    } else if(dynamic_cast<terrain_ops::PerlinNoise*>(op)) {
        // Perlin Noise
        terrain_ops::PerlinNoise* pn = (terrain_ops::PerlinNoise*)op;
        ui->stackedWidget->setCurrentWidget(ui->terrain_perlin_noise);
        ui->terrain_pl_start_octave->setValue(pn->getStartOctave());
        ui->terrain_pl_end_octave->setValue(pn->getEndOctave());
        ui->terrain_pl_persistance->setValue(pn->getPersistance()*1000);
        ui->terrain_pl_seed->setValue(pn->getSeed());
        connect(ui->terrain_pl_start_octave, SIGNAL(valueChanged(int)), this, SLOT(t_perlinSetStartOctave(int)));
        connect(ui->terrain_pl_end_octave, SIGNAL(valueChanged(int)), this, SLOT(t_perlinSetEndOctave(int)));
        connect(ui->terrain_pl_persistance, SIGNAL(valueChanged(int)), this, SLOT(t_perlinSetPersistance(int)));
        connect(ui->terrain_pl_seed, SIGNAL(valueChanged(int)), this, SLOT(t_perlinSetSeed(int)));
    } else if(dynamic_cast<terrain_ops::VoronoiDiagram*>(op)) {
        // Voronoi Diagram
        terrain_ops::VoronoiDiagram* vd = (terrain_ops::VoronoiDiagram*)op;
        ui->stackedWidget->setCurrentWidget(ui->terrain_voronoi_diagram);
        ui->terrain_vd_seed->setValue(vd->getSeed());
        ui->terrain_vd_num_points->setValue(vd->getNumFeaturePoints());
        ui->terrain_vd_weight_a->setValue(vd->getBlendWeightA());
        ui->terrain_vd_weight_b->setValue(vd->getBlendWeightB());
        connect(ui->terrain_vd_seed, SIGNAL(valueChanged(int)), this, SLOT(t_voronoiSetSeed(int)));
        connect(ui->terrain_vd_num_points, SIGNAL(valueChanged(int)), this, SLOT(t_voronoiSetNumPoints(int)));
        connect(ui->terrain_vd_weight_a, SIGNAL(valueChanged(double)), this, SLOT(t_voronoiSetBlendA(double)));
        connect(ui->terrain_vd_weight_b, SIGNAL(valueChanged(double)), this, SLOT(t_voronoiSetBlendB(double)));
    } else if(dynamic_cast<terrain_ops::VoronoiStencil*>(op)) {
        // Voronoi Stencil
        terrain_ops::VoronoiStencil* vs = (terrain_ops::VoronoiStencil*)op;
        ui->stackedWidget->setCurrentWidget(ui->terrain_voronoi_stencil);
        ui->terrain_vs_seed->setValue(vs->getSeed());
        ui->terrain_vs_num_feature_points->setValue(vs->getNumFeaturePoints());
        ui->terrain_vs_probability->setValue(vs->getProbabilityFactor()*1000);
        ui->terrain_vs_white_value->setValue(vs->getWhiteValue());
        ui->terrain_vs_black_value->setValue(vs->getBlackValue());
        connect(ui->terrain_vs_seed, SIGNAL(valueChanged(int)), this, SLOT(t_vstencilSetSeed(int)));
        connect(ui->terrain_vs_num_feature_points, SIGNAL(valueChanged(int)), this, SLOT(t_vstencilSetNumPoints(int)));
        connect(ui->terrain_vs_probability, SIGNAL(valueChanged(int)), this, SLOT(t_vstencilSetProbability(int)));
        connect(ui->terrain_vs_white_value, SIGNAL(valueChanged(double)), this, SLOT(t_vstencilSetWhiteValue(double)));
        connect(ui->terrain_vs_black_value, SIGNAL(valueChanged(double)), this, SLOT(t_vstencilSetBlackValue(double)));
    } else if(dynamic_cast<terrain_ops::ThermalErosion*>(op)) {
        terrain_ops::ThermalErosion* te = (terrain_ops::ThermalErosion*)op;
        ui->stackedWidget->setCurrentWidget(ui->terrain_thermal_erosion);
        ui->terrain_te_num_iterations->setValue(te->getNumIterations());
        ui->terrain_te_talus_ratio->setValue(te->getTalusAngle());
        connect(ui->terrain_te_num_iterations, SIGNAL(valueChanged(int)), this, SLOT(t_thermalSetNumIterations(int)));
        connect(ui->terrain_te_talus_ratio, SIGNAL(valueChanged(double)), this, SLOT(t_thermalSetTalusAngle(double)));
    } else if(dynamic_cast<terrain_ops::HydraulicErosion*>(op)) {
        terrain_ops::HydraulicErosion* he = (terrain_ops::HydraulicErosion*)op;
        ui->stackedWidget->setCurrentWidget(ui->terrain_hydraulic_erosion);
        ui->terrain_he_num_iterations->setValue(he->getNumIterations());
        ui->terrain_he_solubility->setValue(he->getSolubility());
        ui->terrain_he_evap_coef->setValue(he->getEvaporationCoefficient());
        ui->terrain_he_water_const->setValue(he->getWaterConst());
        ui->terrain_he_sed_cap->setValue(he->getSedimentCapacity());
        connect(ui->terrain_he_num_iterations, SIGNAL(valueChanged(int)), this, SLOT(t_hydraulicSetNumIterations(int)));
        connect(ui->terrain_he_solubility, SIGNAL(valueChanged(double)), this, SLOT(t_hydraulicSetSolubility(double)));
        connect(ui->terrain_he_evap_coef, SIGNAL(valueChanged(double)), this, SLOT(t_hydraulicSetEvaporationCoef(double)));
        connect(ui->terrain_he_water_const, SIGNAL(valueChanged(double)), this, SLOT(t_hydraulicSetWaterConst(double)));
        connect(ui->terrain_he_sed_cap, SIGNAL(valueChanged(double)), this, SLOT(t_hydraulicSetSedimentCapacity(double)));
    } else if(dynamic_cast<terrain_ops::InverseThermalErosion*>(op)) {
        ui->stackedWidget->setCurrentWidget(ui->terrain_inverse_thermal);
        terrain_ops::InverseThermalErosion* ite = (terrain_ops::InverseThermalErosion*)op;
        ui->terrain_ite_num_iterations->setValue(ite->getNumIterations());
        ui->terrain_ite_talus_angle->setValue(ite->getTalusAngle());
        connect(ui->terrain_ite_num_iterations, SIGNAL(valueChanged(int)), this, SLOT(t_invthermalSetNumIterations(int)));
        connect(ui->terrain_ite_talus_angle, SIGNAL(valueChanged(double)), this, SLOT(t_invthermalSetTalusAngle(double)));
    }
}

void TerrainModule::deselectOperation(TerrainOperation* op) {
    ui->stackedWidget->setCurrentWidget(ui->terrain_default_param);
    // check which type of operation has been selected
    if(dynamic_cast<terrain_ops::Normalization*>(op)) {
        disconnect(ui->terrain_norm_max, SIGNAL(valueChanged(double)), this, SLOT(t_normSetMax(double)));
        disconnect(ui->terrain_norm_min, SIGNAL(valueChanged(double)), this, SLOT(t_normSetMin(double)));
    } else if(dynamic_cast<terrain_ops::Blend*>(op)) {
        disconnect(ui->terrain_blend_factor, SIGNAL(valueChanged(int)), this, SLOT(t_setBlendFactor(int)));
    } else if(dynamic_cast<terrain_ops::PerlinNoise*>(op)) {
        disconnect(ui->terrain_pl_start_octave, SIGNAL(valueChanged(int)), this, SLOT(t_perlinSetStartOctave(int)));
        disconnect(ui->terrain_pl_end_octave, SIGNAL(valueChanged(int)), this, SLOT(t_perlinSetEndOctave(int)));
        disconnect(ui->terrain_pl_persistance, SIGNAL(valueChanged(int)), this, SLOT(t_perlinSetPersistance(int)));
        disconnect(ui->terrain_pl_seed, SIGNAL(valueChanged(int)), this, SLOT(t_perlinSetSeed(int)));
    } else if(dynamic_cast<terrain_ops::VoronoiDiagram*>(op)) {
        disconnect(ui->terrain_vd_seed, SIGNAL(valueChanged(int)), this, SLOT(t_voronoiSetSeed(int)));
        disconnect(ui->terrain_vd_num_points, SIGNAL(valueChanged(int)), this, SLOT(t_voronoiSetNumPoints(int)));
        disconnect(ui->terrain_vd_weight_a, SIGNAL(valueChanged(double)), this, SLOT(t_voronoiSetBlendA(double)));
        disconnect(ui->terrain_vd_weight_b, SIGNAL(valueChanged(double)), this, SLOT(t_voronoiSetBlendB(double)));
    } else if(dynamic_cast<terrain_ops::VoronoiStencil*>(op)) {
        disconnect(ui->terrain_vs_seed, SIGNAL(valueChanged(int)), this, SLOT(t_vstencilSetSeed(int)));
        disconnect(ui->terrain_vs_num_feature_points, SIGNAL(valueChanged(int)), this, SLOT(t_vstencilSetNumPoints(int)));
        disconnect(ui->terrain_vs_probability, SIGNAL(valueChanged(int)), this, SLOT(t_vstencilSetProbability(int)));
        disconnect(ui->terrain_vs_white_value, SIGNAL(valueChanged(double)), this, SLOT(t_vstencilSetWhiteValue(double)));
        disconnect(ui->terrain_vs_black_value, SIGNAL(valueChanged(double)), this, SLOT(t_vstencilSetBlackValue(double)));
    } else if(dynamic_cast<terrain_ops::ThermalErosion*>(op)) {
        disconnect(ui->terrain_te_num_iterations, SIGNAL(valueChanged(int)), this, SLOT(t_thermalSetNumIterations(int)));
        disconnect(ui->terrain_te_talus_ratio, SIGNAL(valueChanged(double)), this, SLOT(t_thermalSetTalusAngle(double)));
    } else if(dynamic_cast<terrain_ops::HydraulicErosion*>(op)) {
        disconnect(ui->terrain_he_num_iterations, SIGNAL(valueChanged(int)), this, SLOT(t_hydraulicSetNumIterations(int)));
        disconnect(ui->terrain_he_solubility, SIGNAL(valueChanged(double)), this, SLOT(t_hydraulicSetSolubility(double)));
        disconnect(ui->terrain_he_evap_coef, SIGNAL(valueChanged(double)), this, SLOT(t_hydraulicSetEvaporationCoef(double)));
        disconnect(ui->terrain_he_water_const, SIGNAL(valueChanged(double)), this, SLOT(t_hydraulicSetWaterConst(double)));
        disconnect(ui->terrain_he_sed_cap, SIGNAL(valueChanged(double)), this, SLOT(t_hydraulicSetSedimentCapacity(double)));
    } else if(dynamic_cast<terrain_ops::InverseThermalErosion*>(op)) {
        disconnect(ui->terrain_ite_num_iterations, SIGNAL(valueChanged(int)), this, SLOT(t_invthermalSetNumIterations(int)));
        disconnect(ui->terrain_ite_talus_angle, SIGNAL(valueChanged(double)), this, SLOT(t_invthermalSetTalusAngle(double)));
    }
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
