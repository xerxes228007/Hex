#include "server.h"

Server::Server()
{
    if(this->listen(QHostAddress::Any, 7777))
    {
        qDebug()<<"start";
    }
    else
    {
        qDebug()<<"error";
    }
}

void Server::incomingConnection(qintptr socketDiscriptor)
{
    socket = new QTcpSocket;
    socket->setSocketDescriptor(socketDiscriptor);
    connect(socket, &QTcpSocket::readyRead, this, &Server::ReadSlot);
    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
    connect(this, &Server::sendSlotSignal, this, &Server::SendSlot);
    Sockets.push_back(socket);
    qDebug()<<"client connected" <<socketDiscriptor;
}

void Server::ReadSlot()
{
    socket = (QTcpSocket*)sender();
    Data = socket->readAll();
    qDebug()<<Data;

    switch (Data.at(0))
    {
    case Server::SenderCodes::GAME_CONNECTION_CODE:
    {
        QString userNickname = Data.mid(1, Data.size());
        qDebug()<<userNickname<< " tried to connect the game";


        if(!nicknames.contains(userNickname)&&nicknames.size()<4)
        {
            Data.clear();
            nicknames.push_back(userNickname);
            Data.push_back(Server::ServerCodes::CONNECTION_SUCCESS);
            Data.push_back(nicknames.size());
            foreach (auto nick, nicknames)
            {
                Data.push_back(nick.size());
                Data.push_back(nick.toUtf8());
            }
            qDebug() <<"Sucsessfull conection";
            SendSlot(Data);

        }
        else if(nicknames.size()>=4)
        {
            qDebug() <<"conection declined";
            Data.clear();
            Data.push_back(Server::ServerCodes::PLAYER_LIMIT_ERROR);
            SendSlot(Data);
        }
        else
        {
            qDebug() <<"conection declined";
            Data.clear();
            Data.push_back(Server::ServerCodes::NICKNAME_ERROR);
            SendSlot(Data);
        }
        break;
    }
    case Server::SenderCodes::START_GAME_REQUEST:
    {
        qDebug()<<nicknames.size();
        if(nicknames.size()>1)
        {
            game = new Game(nicknames.size(), 60, 30);

            Data.clear();
            Data.push_back(Server::ServerCodes::START_GAME);
            Data.push_back(game->getMap().size());
            Data.push_back(game->getMap()[0].size());
            for(int i = 0; i<Data.at(1); ++i)
            {
                for (int j = 0; j < Data.at(2); ++j) {
                    Data.push_back(game->getMap()[i][j].getBiome());
                }
            }

            SendSlot(Data);

        }
    }
    default:
        break;
    }

}

void Server::SendSlot(const QByteArray &arr)
{
    qDebug()<<"sending packages...";
    Data = arr;
    qDebug()<<Data;
    foreach (auto soc, Sockets) {
        soc->write(Data);
    }
}



