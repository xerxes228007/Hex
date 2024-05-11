#include "mainwindow.h"
#include <QApplication>
#include "texture.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Texture::init("../assets");
    Client* client = new Client();
    MainWindow w(client);
    w.show();

    return a.exec();
}
