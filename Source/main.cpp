#include <QtGui/QApplication>
#include "Application.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QGLFormat glf = QGLFormat::defaultFormat();
    glf.setSampleBuffers(true);
    glf.setSamples(4);
    QGLFormat::setDefaultFormat(glf);
    Application b;
    b.show();
    
    return a.exec();
}
