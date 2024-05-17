#ifndef SERVER_H
#define SERVER_H
#include <QTcpServer>
#include <QTcpSocket>
#include <QVector>
#include "game.h"
#include "user.h"

class Server: public QTcpServer
{
    Q_OBJECT
public:
    Server();
    QTcpSocket *socket;

    enum SenderCodes : int
    {
        START_GAME_REQUEST   = 0x00,
        BATTLE_REQUEST       = 0x01,
        BUY_REQUEST          = 0x02,
        STRUCTURES_REQUEST   = 0x03,
        GAME_CONNECTION_CODE = 0x04,
        END_WALK             = 0x05,
        PLACE_UNIT           = 0x06,
        LOSE                 = 0x07,
        REQUEST_WALK         = 0x08,
    };

    enum ServerCodes : int
    {
        ERROR               = 0x00,
        CONNECTION_SUCCESS  = 0x01,
        OK                  = 0x02,
        PLAYER_LIMIT_ERROR  = 0x03,
        NICKNAME_ERROR      = 0x04,
        START_GAME          = 0x06,
        SEND_STRUCTURES     = 0x07,
        WALK                = 0x08,
        NEXT_MOVE           = 0x09,
        PLACE_UNIT_ACCEPT   = 0x0A,
        WIN                 = 0x0B,
        BATTLE_RESULT       = 0x0C,
    };
private:
    const QVector<QColor> PLAYER_COLORS {Qt::magenta, Qt::yellow, Qt::cyan, Qt::darkGreen};
    QVector<User> players;
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
