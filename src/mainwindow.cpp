#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "gamewindow.h"
#include <QHostInfo>

MainWindow::MainWindow(Client* Client, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    ,client(Client)

{
    ui->setupUi(this);
    socket = client->getSocket();
    connect(ui->pushButton, &QPushButton::clicked, this, [this](){
        Data.clear();
        Data.push_back(Client::SenderCodes::GAME_CONNECTION_CODE);
        QString str = ui->lineEdit->text();
        Data.push_back(str.toUtf8());
        socket->write(Data);
    });
    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::ReadSlot);
    connect(ui->pushButton_2, &QPushButton::clicked, this, &MainWindow::StartGameReq);

}

void MainWindow::StartGameReq()
{
    Data.clear();
    Data.push_back(Client::SenderCodes::START_GAME_REQUEST);
    socket->write(Data);
}

void MainWindow::ReadSlot()
{
    socket = (QTcpSocket*)sender();
    Data = socket->readAll();
    switch (Data.at(0)) {
    case Client::ServerCodes::CONNECTION_SUCCESS:{
        ui->label->setText("sucsessfull connection");
        ui->label_6->setText("Ожидание хоста...");
        ui->label_6->setStyleSheet(QString("font-size: %1px").arg(20));

        QVector<QString> nicknames;
        int offset = 2;
        for(auto i = 0; i < Data.at(1); ++i)
        {
            int length = Data.at(offset);
            nicknames.push_back(Data.mid(offset+1, length));
            offset+=length+1;
        }
        QVector<QLabel*> players = {ui->player1_label, ui->player2_label, ui->player3_label, ui->player4_label};
        for (int i = 0; i<nicknames.size(); ++i)
        {
            players[i]->setText(nicknames[i]);
            players[i]->setStyleSheet(QString("font-size: %1px").arg(25));
        }
        ui->label->hide();
        ui->pushButton->hide();
        ui->lineEdit->hide();
        break;
    }
    case Client::ServerCodes::START_GAME:
    {
        int height = Data.at(1);
        int width = Data.at(2);
        int offset = 3;
        QVector<QVector<Biome::BiomeName>> map;
        qDebug()<<Data.size();

        for(int i = 0; i<height; ++i)
        {
            QVector<Biome::BiomeName> row;
            for(int j = 0; j<width; ++j)
            {
                int biome = Data.at(offset);
                row.push_back(Biome::BiomeName(biome));
                offset++;
            }
            map.push_back(row);
        }

        GameWindow* game = new GameWindow(client, map, this);
        game->show();
        this->hide();
    }
    default:
        break;
    }
}



MainWindow::~MainWindow()
{
    delete ui;
}
