#include "connection.h"

Connection::Connection()
{
    this->state=JustConnected;
    this->IV=QByteArray(16,0);
}

Connection::State Connection::getState()
{
    return this->state;
}

void Connection::setState(State state)
{
    this->state=state;
}

void Connection::setIV(QByteArray IV)
{
    this->IV.clear();
    this->IV.append(IV);
}

QByteArray Connection::getIV()
{
    return this->IV;
}
