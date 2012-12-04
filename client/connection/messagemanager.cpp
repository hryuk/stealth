#include "messagemanager.h"

MessageManager::MessageManager(QObject *parent) : QObject(parent)
{

}

void MessageManager::readMessage()
{
    QMessageBox::information(0,":P","CrashTest 1");

    Connection* connection=qobject_cast<Connection*>(sender());
    if(!connection) return;
    if(connection->getState()==Connection::JustConnected) return;

    if(connection->getState()==Connection::WaitingForLoader)
    {
        if(connection->bytesAvailable()<1) return; //WTF

        char ok;
        connection->read(&ok,1);

        if(ok==0x01)
        {
            emit receivedLoaderOk(connection);
        }
        else
        {
            connection->readAll();
        }
        return;
    }

    if(connection->getState()==Connection::WaitingForGreeting)
    {
        QDataStream in(connection);
        in.setVersion(QDataStream::Qt_4_8);



        if(connection->bytesAvailable()<(uint)sizeof(Connection::RPEP_HEADER)) return;

        if(in.readRawData((char*)&connection->NextBlockHeader,sizeof(Connection::RPEP_HEADER))!=sizeof(Connection::RPEP_HEADER)) return;

        QMessageBox::information(0,":P","Comprobacion #1");

        //if(!connection->NextBlockHeader.OperationType.bOperation) return;

        QMessageBox::information(0,":P","Comprobacion #2");

        if(connection->NextBlockHeader.OperationType.Operation!=Connection::RPEP_HEADER::ServerHandshake) return;


        QMessageBox::information(0,":P","Comprobacion #3");
        if(connection->NextBlockHeader.Size.bBlocks || connection->NextBlockHeader.Size.Bytes!=sizeof(Connection::RPEP_SERVER_HANDSHAKE)) return;

        QMessageBox::information(0,":P","Comprobacion #4");

        connection->setState(Connection::ReadingGreeting);
    }

    if(connection->getState()==Connection::ReadingGreeting)
    {
        QDataStream in(connection);
        in.setVersion(QDataStream::Qt_4_8);

        QMessageBox::information(0,":P","Recibiendo handshake");

        if(connection->bytesAvailable()<connection->NextBlockHeader.Size.Bytes)
        {
            QMessageBox::information(0,":P","Handshake Error 1");
            return;
        }

        connection->Data.resize(connection->NextBlockHeader.Size.Bytes);
        if(in.readRawData(connection->Data.data(),connection->NextBlockHeader.Size.Bytes)!=connection->NextBlockHeader.Size.Bytes)
        {
            QMessageBox::information(0,":P","Handshake Error 2");
            connection->setState(Connection::WaitingForGreeting);
            return;
        }

        QMessageBox::information(0,":P","Recibido handshake");

        /*TODO: esperar a que el servidor responda antes de añadir
                la conexión a la GUI */
        emit receivedHandshake(connection);
        return;
    }
}

void MessageManager::dispatchMessage()
{

}
