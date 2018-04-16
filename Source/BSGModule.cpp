#include "BSGModule.h"
#include "ui_application.h"
#include <stdexcept>
#include <QTextStream>
#include <QFileDialog>
#include <QMessageBox>
#include "ImageManager.h"
#include <iostream>
using namespace std;

BSGModule::BSGModule(Ui::Application* ui, QObject *parent)
    : QObject(parent)
    , activeGrammar(NULL)
    , ui(ui)
    , numIterations(1)
{
    // create the gl widget
    viewport = new Viewport3D("skybox", 0.01f, 10.0f, 0.01);
    viewport->setRenderer(this);
    ui->bsg_viewport->addWidget(viewport);

    connect(ui->bsg_load, SIGNAL(clicked()), this, SLOT(load()));
    connect(ui->bsg_save, SIGNAL(clicked()), this, SLOT(save()));
    connect(ui->bsg_clear, SIGNAL(clicked()), this, SLOT(clear()));
    connect(ui->bsg_apply, SIGNAL(clicked()), this, SLOT(apply()));
    connect(ui->bsg_num_iterations, SIGNAL(valueChanged(int)), this, SLOT(setNumIterations(int)));
}

BSGModule::~BSGModule() {
}

void BSGModule::viewportInit(Viewport3D*) {
    // load a std global symbol map
    cout << "LOADING SYMBOL MESH MAP" << endl;
    shapeDatabase.loadSymbolMeshMap("grammars/castle/mapping.txt");
    // axiom shape FIXME?
    Shape shape;
    shape.position.y = 1.0f;
    shape.yRotation = 0.0f;
    shape.symbol = "castle_entrance";
    shape.terminal = false;
    shape.calculateBoundingBox(shapeDatabase.getSymbolMeshMap());
    shapeDatabase.insert(shapeDatabase.begin(), shape);
    cout << "LOADED SYMBOL MESH MAP"<<endl;
    floorGridTex.load("floor_grid.png");
}

void BSGModule::viewportDraw(Viewport3D*) {

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

    vec3 dir(0.5,0.5,1);
    dir.normalize();

    GLfloat pos[4] = { dir.x,dir.y,dir.z,0 };
    glLightfv(GL_LIGHT0, GL_POSITION, pos);

    // draw the model
    for(ShapeDatabase::iterator i = shapeDatabase.begin(); i != shapeDatabase.end(); ++i) {
        const Shape& shape = (*i);
        glPushMatrix();
        glTranslatef(shape.position.x, shape.position.y, shape.position.z);
        glRotatef(shape.yRotation, 0, 1, 0);
        shapeDatabase.getSymbolMeshMap().find(shape.symbol)->second->Draw();
        glPopMatrix();
    }
/*
    // draw the floor grid
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   // glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);
    //glBindTexture(GL_TEXTURE_2D, floorGridTexture);
    viewport->bindTexture(floorGridTex);
    glBegin(GL_QUADS);
        static const float scale = 1000.0f;
        glTexCoord2f(-scale, scale);
        glVertex3f(-scale, 0, scale);

        glTexCoord2f(scale, scale);
        glVertex3f(scale, 0, scale);

        glTexCoord2f(scale, -scale);
        glVertex3f(scale, 0, -scale);

        glTexCoord2f(-scale, -scale);
        glVertex3f(-scale, 0, -scale);

    glEnd();
    glDisable(GL_TEXTURE_2D);
   // glDisable(GL_BLEND);
*/
    glColor3f(1,1,1);
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);

    glColor3f(0,0,0);
    glLineWidth(2.0);
    glPolygonMode(GL_FRONT, GL_LINE);
        // draw the model
        //glDepthMask(GL_FALSE);

    for(ShapeDatabase::iterator i = shapeDatabase.begin(); i != shapeDatabase.end(); ++i) {
        const Shape& shape = (*i);
        glPushMatrix();
        glTranslatef(shape.position.x, shape.position.y, shape.position.z);
        glRotatef(shape.yRotation + 180, 0, 1, 0);
        shapeDatabase.getSymbolMeshMap().find(shape.symbol)->second->Draw();
        glPopMatrix();
    }

    //glDepthMask(GL_TRUE);
    glPolygonMode(GL_FRONT, GL_FILL);
    glColor3f(1,1,1);
    glLineWidth(1.0);
}

void BSGModule::load() {
    // load grammar into memory
    QString filename = QFileDialog::getOpenFileName(0, "Open Grammar File");
    if(filename != "") {
        if(activeGrammar != NULL) {
            delete activeGrammar;
        }
        // load text from file into editor
        QFile file(filename);
        if(!file.open(QIODevice::ReadOnly)) {
            QMessageBox::critical(0, "Error", "Could not open file");
        }
        QTextStream in(&file);
        ui->bsg_text->setText(in.readAll());
        file.close();

        // now the file text has been set, parse the actual file
        activeGrammar = new BasicShapeGrammar(filename.toStdString());
        currentOpenFile = filename.toStdString();
    }
}

void BSGModule::save() {
    // if there is no active save file then prompt the user for one
    QString filename("");
    if(currentOpenFile == "") {
        filename = QFileDialog::getSaveFileName(0, "Save Grammar File");
        currentOpenFile = filename.toStdString();
    } else {
        filename = QString(currentOpenFile.c_str());
    }

    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly)) {
        // error msg
    } else {
        QTextStream out(&file);
        out << ui->bsg_text->toPlainText();
        out.flush();
        file.close();
    }
    // now the file has been saved we need to update the active grammar
    // delete old one
    delete activeGrammar;
    activeGrammar = new BasicShapeGrammar(currentOpenFile);
}

void BSGModule::clear() {
    shapeDatabase.clear();

    // axiom shape FIXME
    Shape shape;
    shape.position.y = 1.0f;
    shape.yRotation = 0.0f;
    shape.symbol = "castle_entrance";
    shape.terminal = false;
    shape.calculateBoundingBox(shapeDatabase.getSymbolMeshMap());
    shapeDatabase.insert(shapeDatabase.begin(), shape);

    viewport->updateGL();
}

void BSGModule::apply() {
    if(activeGrammar != NULL) {
        activeGrammar->generate(shapeDatabase, numIterations);
        viewport->updateGL();
    }
}

void BSGModule::setNumIterations(int numIterations) {
    this->numIterations = numIterations;
}
