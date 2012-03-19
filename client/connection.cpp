#include "connection.h"

Connection::Connection()
{
    this->state=Connecting;
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
