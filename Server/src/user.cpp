#include "user.h"

User::User(QTcpSocket* s, QString n, uint8_t i)
    :socket(s)
    ,name(n)
    ,id(i)
{}
