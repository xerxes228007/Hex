#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "client.h"
#include <QPushButton>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(Client* Client, QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void ReadSlot();
    void StartGameReq();

private:
    QByteArray Data;
    QTcpSocket *socket;
    Client* client;
    uint8_t gameId;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
