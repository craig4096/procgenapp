#include <QApplication>
#include "Application.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Application b;
    b.show();
    
    return a.exec();
}
