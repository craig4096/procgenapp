#ifndef BSGMODULE_H
#define BSGMODULE_H

#include <QObject>
#include "Structural/BasicShapeGrammar.h"
#include "Viewport3D.h"
#include <vector>
#include "Mesh.h"

namespace Ui { class Application; }

class BSGModule : public QObject, public Viewport3D::Renderer {
    Q_OBJECT

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
    Ui::Application* ui;

    // used for rendering the floor grid
    QImage floorGridTex;

    // used for rendering the floor grid
    GLuint floorGridTexture;

public:
    explicit BSGModule(Ui::Application*, QObject *parent = 0);
    ~BSGModule();

    void viewportInit(Viewport3D*);
    void viewportDraw(Viewport3D*);

public slots:

    void load();
    void save();
    void clear();
    void apply();
    void setNumIterations(int);
    
};

#endif // BSGMODULE_H
