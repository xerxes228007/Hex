#ifndef SERVER_H
#define SERVER_H
#include <QTcpServer>
#include <QTcpSocket>
#include <QVector>
#include "game.h"

class Server: public QTcpServer
{
    Q_OBJECT
public:
    Server();
    QTcpSocket *socket;

    enum SenderCodes : int
    {
        START_GAME_REQUEST   = 0x00,
        GAME_CONNECTION_CODE = 0x04,
        REQUEST_WALK         = 0x07,
        SEND_BOARD           = 0x09,

    };

    enum ServerCodes : int
    {
        ERROR               = 0x00,
        CONNECTION_SUCCESS  = 0x01,
        OK                  = 0x02,
        PLAYER_LIMIT_ERROR  = 0x03,
        NICKNAME_ERROR      = 0x04,
        START_GAME          = 0x06,
        WALK                = 0x08,
        MOVE_ANS            = 0x09,
        WIN                 = 0x0A,
        ACCEPT              = 0x12,
    };
private:
    QVector<QTcpSocket*> Sockets;
    QByteArray Data;
    QVector<QString> nicknames;
    Game* game;
public slots:
    void incomingConnection(qintptr socketDiscriptor);
    void SendSlot(const QByteArray &arr);
    void ReadSlot();
signals:
    void sendSlotSignal(const QByteArray &arr);
};

#endif // SERVER_H
