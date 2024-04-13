#include "mainwindow.h"

#include <QApplication>

#include "texture.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Texture::init("../assets");

    MainWindow w;


    w.show();
    return a.exec();
}
