#include "ProcTexturingModule.h"
#include "ui_application.h"
#include <cstdio>
#include <stdexcept>
#include <QFileDialog>
#include <QTextStream>
#include <iostream>
#include <QMessageBox>
using namespace std;

ProcTexturingModule::ProcTexturingModule(Ui::Application* ui, QObject *parent)
  : QObject(parent)
  , ui(ui)
  , shaderInvalid(true)
  , shader(NULL)
  , time(0.0f)
{
    viewport = new Viewport3D("skybox", 0.01f, 5, 0.005f);

    viewport->setRenderer(this);
    ui->proctex_viewport->addWidget(viewport);

    connect(ui->proctex_sac, SIGNAL(clicked()), this, SLOT(saveAndCompile()));
    connect(ui->proctex_new, SIGNAL(clicked()), this, SLOT(newShader()));
    connect(ui->proctex_load, SIGNAL(clicked()), this, SLOT(loadShader()));
}

ProcTexturingModule::~ProcTexturingModule() {
    delete shader;
    delete mesh;
}

void ProcTexturingModule::viewportInit(Viewport3D*) {
    mesh = new Mesh("meshes/proctex_mesh.txt");


    glEnable(GL_BLEND);
    //glBlendFunc(GL_ONE, GL_ONE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ProcTexturingModule::viewportDraw(Viewport3D*) {
    if(shaderInvalid && activeFragShaderFilename != "") {
        // try to load new shader
        try {
            Shader* tmp = new Shader("shaders/std_vertex.glslv", activeFragShaderFilename); // FIXME: if exception is thrown does this get deallocated?
            shaderInvalid = false;
            // delete previous shader and assign new
            delete shader;
            shader = tmp;
            ui->proctext_errors->setText("");
        } catch(std::exception& e) {
            ui->proctext_errors->setText(QString::fromStdString(e.what()));
        }
    }
    if(shader != NULL) {
        time += 0.033f;
        shader->Set();
        glUniform1f(glGetUniformLocation(shader->GetProgram(), "time"), time);
        glUniformMatrix4fv(glGetUniformLocation(shader->GetProgram(), "viewMat"), 1, GL_FALSE, (float*)&viewport->getViewMatrix());
    }
    mesh->DrawNoTextures();
    Shader::Unset();
}

void ProcTexturingModule::saveAndCompile() {
    // save the edited fragment shader
    if(activeFragShaderFilename.empty()) {
        // choose file location
        activeFragShaderFilename = QFileDialog::getSaveFileName(0, "Choose fragment shader file location").toStdString();
    }

    if(activeFragShaderFilename == "") {
        return;
    }

    // save file
    QFile file(QString::fromStdString(activeFragShaderFilename));
    if(!file.open(QIODevice::WriteOnly)) {
        // error msg
    } else {
        QTextStream out(&file);
        out << ui->proctex_fragedit->toPlainText();
        out.flush();
        file.close();
    }
    // invalidate the shader
    shaderInvalid = true;
    viewport->updateGL();
}

void ProcTexturingModule::loadShader() {
    QString filename = QFileDialog::getOpenFileName(0, "Load Shader");
    if(filename == "") {
        return;
    }
    // load text from file into editor
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(0, "Error", "Could not open file");
        return;
    }
    QTextStream in(&file);
    ui->proctex_fragedit->setText(in.readAll());
    file.close();

    // load the new shader
    shaderInvalid = true;
    activeFragShaderFilename = filename.toStdString();
}

void ProcTexturingModule::newShader() {
    //ui->proctex_fragedit->setText("\nvoid main() {\n\tgl_FragColor = vec4(1,1,1,1);\n}");
    ui->proctex_fragedit->setText("");
    activeFragShaderFilename = "";
}
