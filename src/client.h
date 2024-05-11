#ifndef CLIENT_H
#define CLIENT_H

#include <QTcpSocket>
#include <QVector>

class Client: QTcpSocket
{
public:
    inline QTcpSocket* getSocket() const {return socket;};
    Client();
    enum SenderCodes : int
    {
        START_GAME_REQUEST   = 0x00,
        REGISTRATION_CODE    = 0x01,
        LOGIN_CODE           = 0x02,
        GAME_CREATION_CODE   = 0x03,
        GAME_CONNECTION_CODE = 0x04,
        VERIFICATION_CODE    = 0x05,
        REQUEST_CONNECTION   = 0x06,
        REQUEST_WALK         = 0x07,
        CHEAK_MOVE           = 0x08,
        SEND_BOARD           = 0x09,
        REQUEST_STATS        = 0x0A,
        REQUEST_REVENGE      = 0x0B,

    };

    enum ServerCodes : int
    {
        ERROR               = 0x00,
        CONNECTION_SUCCESS  = 0x01,
        OK                  = 0x02,
        NO_USER             = 0x03,
        NOT_REGISTRATE      = 0x04,
        GAME_NOT_AVAILIABLE = 0x05,
        START_GAME          = 0x06,
        CREATED_SUCCESS     = 0x07,
        WALK                = 0x08,
        MOVE_ANS            = 0x09,
        WIN                 = 0x0A,
        SWAP_SIDES          = 0x0B,
        SET_CELL            = 0x0C,
        SET_NAMES           = 0x0D,
        DRAW                = 0x0E,
        STATS_ANS           = 0x0F,
        NOT_READY           = 0x10,
        REJECT              = 0x11,
        ACCEPT              = 0x12,
    };
private:
    QTcpSocket *socket;
    QByteArray Data;


public slots:
    void ReadSlot();
    void SendSlot();
};

#endif // CLIENT_H
