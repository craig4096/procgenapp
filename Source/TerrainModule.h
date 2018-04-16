#ifndef TERRAINMODULE_H
#define TERRAINMODULE_H

#include <vector>
#include <QObject>
#include <QModelIndex>
#include "GL/glew.h"
#include "Viewport3D.h"
#include "Heightmap.h"
#include "RenderableHeightmap.h"
#include "Terrain/TerrainGenerator.h"

class TerrainOperation;
namespace Ui {
    class Application;
}

// This class represents the main terrain application, it's main purpose it to
// create an interface between the user and the terrain generator class, i.e. it
// can be considered as the 'Controller' in the MVC design pattern
class TerrainModule : public QObject, public Viewport3D::Renderer, public Progress {
    Q_OBJECT
    // the widget used for rendering the GL scene
    Viewport3D* viewport;

    // terrain generator
    TerrainGenerator generator;

    // the applications ui object
    Ui::Application* ui;

    // currently selected terrain operation
    int curSelected;

    // when a terrain is generated it is passed to RenderableHeightmap's
    // constructor so the user can see the results
    RenderableHeightmap* activeHeightmap;

    // deselects an operation from the list (switches tab and disconnects slots and signals)
    void deselectOperation(TerrainOperation*);
    // selects an operation from the list (switches tab connects signals and slots)
    void selectOperation(TerrainOperation*);

    void updateActiveList();

    template <class T>
    T* getCurOp() { return dynamic_cast<T*>(generator.getOperation(curSelected)); }

private slots:
    // functions for each individual terrain operation--
    // -------------------------------------------------
    // normalization
    void t_normSetMin(double);
    void t_normSetMax(double);

    // blending
    void t_setBlendFactor(int);

    // perlin noise
    void t_perlinSetSeed(int);
    void t_perlinSetStartOctave(int);
    void t_perlinSetEndOctave(int);
    void t_perlinSetPersistance(int);

    // voronoi diagram
    void t_voronoiSetSeed(int);
    void t_voronoiSetNumPoints(int);
    void t_voronoiSetBlendA(double);
    void t_voronoiSetBlendB(double);

    // voronoi stencil
    void t_vstencilSetSeed(int);
    void t_vstencilSetNumPoints(int);
    void t_vstencilSetProbability(int);
    void t_vstencilSetBlackValue(double);
    void t_vstencilSetWhiteValue(double);

    // thermal erosion
    void t_thermalSetNumIterations(int);
    void t_thermalSetTalusAngle(double);

    // hydraulic erosion
    void t_hydraulicSetWaterConst(double);
    void t_hydraulicSetSolubility(double);
    void t_hydraulicSetSedimentCapacity(double);
    void t_hydraulicSetEvaporationCoef(double);
    void t_hydraulicSetNumIterations(int);

    // inverse thermal erosion
    void t_invthermalSetNumIterations(int);
    void t_invthermalSetTalusAngle(double);
    // -------------------------------------------------


    void addOperation();
    void removeOperation();
    void moveOpUp();
    void moveOpDown();
    void opSelected(int);
    void generateTerrain();
    void exportTerrain();

public:
    explicit TerrainModule(Ui::Application* ui, QObject *parent = 0);
    ~TerrainModule();

    // terrain generatot progress interface
    void nextOperation(int index);
    void setPercent(float);

    void viewportInit(Viewport3D*){}
    void viewportDraw(Viewport3D*);
};

#endif // TERRAINMODULE_H
