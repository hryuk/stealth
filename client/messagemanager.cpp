#include "messagemanager.h"

MessageManager::MessageManager(QObject *parent) : QObject(parent)
{

}

void MessageManager::readMessage()
{
    Connection* connection=qobject_cast<Connection*>(sender());
    if(!connection) return;
    if(connection->getState()==Connection::JustConnected) return;

    if(connection->getState()==Connection::WaitingForGreeting)
    {
        QDataStream in(connection);
        in.setVersion(QDataStream::Qt_4_8);

        if(connection->bytesAvailable()<(uint)sizeof(Connection::RPEP_HEADER)) return;

        if(in.readRawData((char*)&connection->NextBlockHeader,sizeof(Connection::RPEP_HEADER))!=sizeof(Connection::RPEP_HEADER)) return;

        if(!connection->NextBlockHeader.OperationType.bOperation || connection->NextBlockHeader.OperationType.Operation!=Connection::RPEP_HEADER::ServerHandshake) return;
        if(connection->NextBlockHeader.Size.bBlocks || connection->NextBlockHeader.Size.Bytes!=sizeof(Connection::RPEP_SERVER_HANDSHAKE)) return;

        connection->setState(Connection::ReadingGreeting);
    }

    if(connection->getState()==Connection::ReadingGreeting)
    {
        QDataStream in(connection);
        in.setVersion(QDataStream::Qt_4_8);

        if(connection->bytesAvailable()<connection->NextBlockHeader.Size.Bytes) return;

        connection->Data.resize(connection->NextBlockHeader.Size.Bytes);
        if(in.readRawData(connection->Data.data(),connection->NextBlockHeader.Size.Bytes)!=connection->NextBlockHeader.Size.Bytes)
        {
            connection->setState(Connection::WaitingForGreeting);
            return;
        }

        emit receivedHandshake(connection);
        return;
    }
}

void MessageManager::dispatchMessage()
{

}
