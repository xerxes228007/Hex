#include "server.h"
#include <QVector2D>

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

        if(!nicknames.contains(userNickname)&&players.size()<4)
        {
            Data.clear();
            players.push_back(User(socket ,userNickname, players.size()));
            nicknames.push_back(userNickname);
            Data.push_back(Server::ServerCodes::CONNECTION_SUCCESS);
            Data.push_back(nicknames.size());
            foreach (auto player, players)
            {
                Data.push_back(player.getName().size());
                Data.push_back(player.getName().toUtf8());
                Data.push_back(player.getID());
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
            foreach (QVector2D str, game->getStructures()) {
                Data.push_back(str.x());
                Data.push_back(str.y());
            }
            SendSlot(Data);
        }
        break;

    }
    case Server::SenderCodes::REQUEST_WALK:
    {
        int id = Data.at(1);
        int i1 = Data.at(2);
        int j1 = Data.at(3);
        int i2 = Data.at(4);
        int j2 = Data.at(5);
        Data.clear();
        Data.push_back(Server::ServerCodes::WALK);
        Data.push_back(id);
        Data.push_back(i1);
        Data.push_back(j1);
        Data.push_back(i2);
        Data.push_back(j2);

        game->map[i2][j2].addUnit(game->map[i1][j1].getUnit().getType(), id);
        game->map[i2][j2].setDefaultColor(PLAYER_COLORS[id]);
        game->map[i2][j2].setUnitCount(game->map[i2][j2].getUnitCount()+game->map[i1][j1].getUnitCount());
        game->map[i1][j1].removeUnit();
        game->map[i1][j1].setUnitCount(0);

        SendSlot(Data);
        break;
    }
    case Server::SenderCodes::END_WALK:
    {
        uint8_t id = Data.at(1);
        id = (id == nicknames.size()-1) ? 0 : id+1;

        Data.clear();
        Data.push_back(Server::ServerCodes::NEXT_MOVE);
        Data.push_back(id);
        SendSlot(Data);
        break;
    }
    case Server::SenderCodes::PLACE_UNIT:
    {
        int i = Data.at(1);
        int j = Data.at(2);
        uint8_t type = Data.at(3);
        uint8_t id = Data.at(4);
        int count = Data.at(5);

        game->map[i][j].addUnit(Unit::Type(type), id);
        game->map[i][j].setUnitCount(count);

        Data[0] = Server::ServerCodes::PLACE_UNIT_ACCEPT;
        SendSlot(Data);
        break;
    }
    case Server::SenderCodes::LOSE:
    {
        uint8_t id = Data.at(1);
        for(int i = 0; i<players.size(); ++i)
        {
            if(players[i].getID() == id)
            {
                players.erase(players.begin() + i);
                break;
            }
        }
        if(players.size() == 1)
        {
            Data.clear();
            Data.push_back(Server::ServerCodes::WIN);
        }
        else
        {
            id = (id>=players.size()) ? 0 : id+1;
            Data.clear();
            Data.push_back(Server::ServerCodes::NEXT_MOVE);
            Data.push_back(id);
        }
        SendSlot(Data);
        break;
    }
    case Server::SenderCodes::BATTLE_REQUEST:
    {
        int i1 = Data.at(1);
        int j1 = Data.at(2);
        int i2 = Data.at(3);
        int j2 = Data.at(4);
        int c1 = Data.at(5);
        int c2 = Data.at(6);
        Data.clear();

        int AtackerPower  = game->map[i1][j1].getUnit().getDamage();
        int DefenderPower = game->map[i2][j2].getUnit().getProtection();

        int AtackerHealth = game->map[i1][j1].getUnit().getHealth()*c1;
        int DefenderHealth = game->map[i2][j2].getUnit().getHealth()*c2;

        int count;

        while(DefenderHealth>0)
        {
            AtackerHealth -= DefenderPower;
            if(AtackerHealth<=0) break;
            DefenderHealth -= AtackerPower;
        }

        if(AtackerHealth>0)
        {
            count = AtackerHealth/game->map[i1][j1].getUnit().getHealth() + ((AtackerHealth%game->map[i1][j1].getUnit().getHealth()==0) ? 0 : 1);
            Data.push_back(Server::ServerCodes::BATTLE_RESULT);
            Data.push_back(i1);
            Data.push_back(j1);
            Data.push_back(i2);
            Data.push_back(j2);
            Data.push_back(count);
            Data.push_back(false);
        }
        else
        {
            count = DefenderHealth/game->map[i2][j2].getUnit().getHealth() + ((DefenderHealth%game->map[i2][j2].getUnit().getHealth()==0) ? 0 : 1);
            Data.push_back(Server::ServerCodes::BATTLE_RESULT);
            Data.push_back(i1);
            Data.push_back(j1);
            Data.push_back(i2);
            Data.push_back(j2);
            Data.push_back(count);
            Data.push_back(true);
        }
        SendSlot(Data);
        break;
    }
    default:
        break;
    }

}

void Server::SendSlot(const QByteArray &arr)
{
    qDebug()<<"sending packages...";
    qDebug()<<Data;

    foreach (auto player, players) {
         player.getSocket()->write(arr);
    }
    qDebug()<<"packages sended";
}



