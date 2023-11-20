#ifndef TERRAINMODULE_H
#define TERRAINMODULE_H

#include <vector>
#include "GL/glew.h"
#include "Viewport3D.h"
#include "Heightmap.h"
#include "RenderableHeightmap.h"
#include "Terrain/TerrainGenerator.h"
#include "MainWindow.h"
#include "Shader.h"
#include <memory>

class TerrainOperation;

// This class represents the main terrain application, it's main purpose it to
// create an interface between the user and the terrain generator class, i.e. it
// can be considered as the 'Controller' in the MVC design pattern
class TerrainModule : public Viewport3D::Renderer, public Progress
{
    // the widget used for rendering the GL scene
    Viewport3D* viewport;

    // terrain generator
    TerrainGenerator generator;

    // the applications ui object
    MainWindow* mainWindow;

    // currently selected terrain operation
    int curSelected;

    bool repopulating;

    // when a terrain is generated it is passed to RenderableHeightmap's
    // constructor so the user can see the results
    RenderableHeightmap* activeHeightmap;

    std::unique_ptr<Shader> shader;

    // selects an operation from the list (switches tab connects signals and slots)
    void selectOperation(TerrainOperation*);

    void updateActiveList();
    void addOperation();
    void removeOperation();
    void moveOpUp();
    void moveOpDown();
    void opSelected(int);
    void generateTerrain();
    void exportTerrain();

public:
    explicit TerrainModule(MainWindow* mainWindow);
    ~TerrainModule();

    // terrain generatot progress interface
    void nextOperation(int index);
    void setPercent(float);

    void viewportInit(Viewport3D*);
    void viewportDraw(Viewport3D*);
};

#endif // TERRAINMODULE_H
