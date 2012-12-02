#include "messagemanager.h"

MessageManager::MessageManager(QObject *parent) : QObject(parent)
{

}

void MessageManager::readMessage()
{
    Connection* connection=qobject_cast<Connection*>(sender());
    if(!connection) return;
    if(connection->getState()==Connection::JustConnected) return;

    //FIXME: Si se recibe otra cosa distinta de 0x01, el cliente se queda
    // esperando infinitamente, hay que limpiar el buffer de entrada del socket
    if(connection->getState()==Connection::WaitingForLoader)
    {
        if(connection->bytesAvailable()<1) return; //WTF

        char ok;
        connection->read(&ok,1);

        if(ok==0x01)
        {
            emit receivedLoaderOk(connection);
        }
        return;
    }

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

        QMessageBox::information(0,":P","Recibido handshake");

        emit receivedHandshake(connection);
        return;
    }
}

void MessageManager::dispatchMessage()
{

}
