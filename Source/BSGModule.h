#ifndef BSGMODULE_H
#define BSGMODULE_H

#include "MainWindow.h"
#include "Structural/BasicShapeGrammar.h"
#include "Viewport3D.h"
#include <vector>
#include "Mesh.h"

class BSGModule : public Viewport3D::Renderer
{
    ShapeDatabase shapeDatabase;
    BasicShapeGrammar* activeGrammar;

    // the list of shapes the grammar is applied to
    //std::vector<BasicShapeGrammar::Shape> shapes;

    // number of iterations to be applied when user clicks the apply button
    int numIterations;

    Viewport3D* viewport;

    // the filename of the currently opened grammar file, empty if no file currently loaded
    std::string currentOpenFile;

    // application ui
    MainWindow* mainWindow;

    // used for rendering the floor grid
    //wxImage floorGridTex;

    // used for rendering the floor grid
    //GLuint floorGridTexture;

public:
    explicit BSGModule(MainWindow* mainWindow);
    ~BSGModule();

    Viewport3D* GetViewport() { return viewport; }

    void viewportInit(Viewport3D*);
    void viewportDraw(Viewport3D*);

    void load();
    void save();
    void clear();
    void apply();
    void setNumIterations(int);

};

#endif // BSGMODULE_H
