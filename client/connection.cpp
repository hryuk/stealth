#include "connection.h"

Connection::Connection()
{
    this->state=Connecting;
    this->IV=QByteArray(16,0);
}

Connection::SetupState Connection::getSetupState()
{
    return this->setupState;
}

Connection::State Connection::getState()
{
    return this->state;
}

void Connection::setState(State state)
{
    this->state=state;
}

void Connection::setSetupState(SetupState setupState)
{
    this->setupState=setupState;
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
