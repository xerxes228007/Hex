#ifndef USER_H
#define USER_H
#include <QTcpSocket>
#include <QColor>

class User
{
public:
    User(QTcpSocket*, QString, uint8_t);
    inline QTcpSocket* getSocket() const {return socket;};
    inline uint8_t getID() const {return id;};
    inline QString getName() const {return name;};
private:
    QTcpSocket* socket;
    QString name;
    uint8_t id;
};

#endif // USER_H
