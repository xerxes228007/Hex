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
    ui->pushButton_2->setVisible(0);
    socket = client->getSocket();
    connect(ui->pushButton, &QPushButton::clicked, this, [this](){
        Data.clear();
        Data.push_back(Client::SenderCodes::GAME_CONNECTION_CODE);
        QString str = ui->lineEdit->text();
        client->setNickname(str);
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
        ui->pushButton_2->setVisible(1);
        ui->label_6->setText("Ожидание хоста...");
        ui->label_6->setStyleSheet(QString("font-size: %1px").arg(20));

        QVector<QString> nicknames;
        int offset = 2;
        for(auto i = 0; i < Data.at(1); ++i)
        {
            int length = Data.at(offset);
            QString nick = Data.mid(offset+1, length);
            offset+=length+2;
            if(nick == client->getNickname()) client->setID(Data.at(offset-1));
            nicknames.push_back(nick);
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

        QVector<QVector2D> coordinates;

        for(int i = offset; i<Data.size(); i+=2)
        {
            coordinates.push_back(QVector2D(Data.at(i), Data.at(i+1)));
        }

        game = new GameWindow(client, coordinates, map, this);
        game->showFullScreen();
        this->hide();
    }
    default:
        game->setData(Data);
        break;
    }
}



MainWindow::~MainWindow()
{
    delete ui;
}
