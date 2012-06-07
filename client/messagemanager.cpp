#include "messagemanager.h"

MessageManager::MessageManager(QObject *parent) : QObject(parent)
{

}

void MessageManager::readMessage()
{
    Connection* connection=qobject_cast<Connection*>(sender());
    if(!connection) return;
    if(connection->getState()==Connection::JustConnected) return;

    if(connection->getState()==Connection::Handshake)
    {
        QDataStream in(connection);
        in.setVersion(QDataStream::Qt_4_7);

        if(connection->bytesAvailable()<sizeof(Connection::RPEP_HEADER)) return;

        Connection::RPEP_HEADER Header;
        if(in.readRawData((char*)&Header,sizeof(Connection::RPEP_HEADER))!=sizeof(Connection::RPEP_HEADER)) return;

        //TODO
    }

    connection->setState(Connection::Reading);

    //TODO

    connection->setState(Connection::Ready);
}

void MessageManager::dispatchMessage()
{

}
