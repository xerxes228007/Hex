#include "client.h"
#include <QHostInfo>

Client::Client() {
    socket = new QTcpSocket(this);
    QString localhostname = QHostInfo::localHostName();
    QString localhostIP;
    QList<QHostAddress> hostList = QHostInfo::fromName(localhostname).addresses();
    foreach (const QHostAddress& address, hostList) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address.isLoopback() == false) {
            localhostIP = address.toString();
        }
    }
    socket->connectToHost(localhostIP, 7777);
    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
}

void Client::ReadSlot()
{
    socket = (QTcpSocket*)sender();
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_6_6);
    if(in.status()==QDataStream::Ok)
    {
        qDebug()<<"read...";
    }
}

void Client::SendSlot()
{
    Data.clear();
    socket->write(Data);
}
