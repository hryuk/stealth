#include "messagemanager.h"

MessageManager::MessageManager(QObject *parent) : QObject(parent)
{

}

void MessageManager::readMessage()
{
    Connection* connection=qobject_cast<Connection*>(sender());
    if(!connection) return;

    connection->setState(Connection::Reading);

    //TODO

    connection->setState(Connection::Ready);
}

