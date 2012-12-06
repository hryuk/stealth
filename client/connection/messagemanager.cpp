#include "messagemanager.h"

MessageManager::MessageManager(QObject *parent) : QObject(parent)
{

}

void MessageManager::readMessage()
{
    qDebug("Recibido mensaje");
    Connection* connection=qobject_cast<Connection*>(sender());
    if(!connection) return;
    if(connection->getState()==Connection::JustConnected) return;

    if(connection->getState()==Connection::WaitingForLoader)
    {
        qDebug("Leyendo OK");
        if(connection->bytesAvailable()<16) return;

        /*
        char ok;
        connection->read(&ok,1);

        qDebug("OK leido, comprobando OK");
        if(ok==0x01)
        {
            qDebug("OK correcto");
            emit receivedLoaderOk(connection);
        }
        else
        {
            qDebug("OK incorrecto");
            connection->readAll();
        }
        return;

        */

        QByteArray cmsgOk=connection->read(16);

        QByteArray msgOk=connection->decrypt(cmsgOk);
        foreach(char c,msgOk)
        {
            if(c!=0x01)
            {
                char err=0;
                connection->write(&err,1);
                connection->readAll();
                return;
            }
        }

        emit receivedLoaderOk(connection);
    }

    return;

    if(connection->getState()==Connection::WaitingForGreeting)
    {
        QString msg;
        msg+="+Leyendo cabecera del hanshake";
        msg+=QString::number((uint)sizeof(Connection::RPEP_HEADER));
        qDebug(msg.toAscii().data());
        QDataStream in(connection);
        in.setVersion(QDataStream::Qt_4_8);

        if(connection->bytesAvailable()<(uint)sizeof(Connection::RPEP_HEADER)) return;

        qDebug("     -Leyendo cabecera del hanshake");
        if(in.readRawData((char*)&connection->NextBlockHeader,sizeof(Connection::RPEP_HEADER))!=sizeof(Connection::RPEP_HEADER)) return;

        qDebug("     -Comprobando bOperation");
        if(!connection->NextBlockHeader.OperationType.bOperation) return;

        qDebug("     -Comprobando Operation");
        if(connection->NextBlockHeader.OperationType.Operation!=Connection::RPEP_HEADER::ServerHandshake) return;

        qDebug("     -Comprobando Tamaño");
        if(connection->NextBlockHeader.Size.bBlocks || connection->NextBlockHeader.Size.Bytes!=sizeof(Connection::RPEP_SERVER_HANDSHAKE)) return;

        qDebug("     -Cabecera correcta");
        connection->setState(Connection::ReadingGreeting);
    }

    if(connection->getState()==Connection::ReadingGreeting)
    {
        qDebug("Leyendo Hanshake");
        QDataStream in(connection);
        in.setVersion(QDataStream::Qt_4_8);

        if(connection->bytesAvailable()<connection->NextBlockHeader.Size.Bytes)
        {
            return;
        }

        connection->Data.resize(connection->NextBlockHeader.Size.Bytes);
        if(in.readRawData(connection->Data.data(),connection->NextBlockHeader.Size.Bytes)!=connection->NextBlockHeader.Size.Bytes)
        {
            connection->setState(Connection::WaitingForGreeting);
            return;
        }

        /*TODO: esperar a que el servidor responda antes de añadir
                la conexión a la GUI */
        emit receivedHandshake(connection);
        return;
    }
}

void MessageManager::dispatchMessage()
{

}
