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

    QDataStream in(connection);
    in.setVersion(QDataStream::Qt_4_8);

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
        return;
    }

    if(connection->getState()==Connection::WaitingForGreeting)
    {
        qDebug("+Leyendo cabecera del handshake");

        if(connection->bytesAvailable()<(uint)sizeof(Connection::RPEP_HEADER)) return;

        qDebug("     -Leyendo cabecera del handshake");
        if(in.readRawData((char*)&connection->NextBlockHeader,sizeof(Connection::RPEP_HEADER))!=sizeof(Connection::RPEP_HEADER)) return;

        qDebug("     -Comprobando bOperation");
        if(!connection->NextBlockHeader.OperationType.bOperation) return;

        qDebug("     -Comprobando Operation");
        if(connection->NextBlockHeader.OperationType.Operation!=Connection::RPEP_HEADER::ServerHandshake) return;

        qDebug("     -Comprobando Tamaño");
        qDebug()<<"    -Tamaño recibido: 0x"+QString::number(connection->NextBlockHeader.Size.Bytes);
        qDebug()<<"    -Tamaño esperado: 0x"+QString::number(sizeof(Connection::RPEP_SERVER_HANDSHAKE));
        qDebug()<<"    -Tamaño redondeado: 0x"+QString::number(connection->round16(sizeof(Connection::RPEP_SERVER_HANDSHAKE)));
        if(connection->NextBlockHeader.Size.bBlocks || connection->NextBlockHeader.Size.Bytes!=connection->round16(sizeof(Connection::RPEP_SERVER_HANDSHAKE)))
        {
            connection->readAll();
            return;
        }

        qDebug("     -Cabecera correcta");
        connection->setState(Connection::ReadingGreeting);

        qDebug("Leyendo Handshake");

        if(connection->bytesAvailable()<connection->NextBlockHeader.Size.Bytes) return;

        connection->Data.resize(connection->NextBlockHeader.Size.Bytes);
        if(in.readRawData(connection->Data.data(),connection->NextBlockHeader.Size.Bytes)!=connection->NextBlockHeader.Size.Bytes)
        {
            connection->setState(Connection::WaitingForGreeting);
            return;
        }

        connection->Data=connection->decrypt(connection->Data);
        connection->NextBlockHeader.Size.Bytes=connection->Data.size();

        emit receivedHandshake(connection);
        return;
    }

    if(connection->getState()==Connection::WaitingGreetingOk)
    {
        qDebug("+Leyendo cabecera Handshake OK");

        if(connection->bytesAvailable()<(uint)sizeof(Connection::RPEP_HEADER)) return;

        qDebug("    -Leyendo cabecera");

        if(in.readRawData((char*)&connection->NextBlockHeader,sizeof(Connection::RPEP_HEADER))!=sizeof(Connection::RPEP_HEADER)) return;

        qDebug("    -Comprobando bOperation");
        if(!connection->NextBlockHeader.OperationType.bOperation) return;

        qDebug("    -Comprobando Operation");
        if(connection->NextBlockHeader.OperationType.Operation!=Connection::RPEP_HEADER::Error) return;

        qDebug("    -Comprobando Tamaño");
        if(connection->NextBlockHeader.Size.bBlocks || connection->NextBlockHeader.Size.Bytes!=connection->round16(sizeof(Connection::RPEP_ERROR))) return;

        qDebug("    -Cabecera correcta");
        qDebug("Leyendo confirmación de handshake");

        if(connection->bytesAvailable()<connection->NextBlockHeader.Size.Bytes) return;

        connection->Data.resize(connection->NextBlockHeader.Size.Bytes);
        if(in.readRawData(connection->Data.data(),connection->NextBlockHeader.Size.Bytes)!=connection->NextBlockHeader.Size.Bytes)
        {
            connection->readAll();
            connection->setState(Connection::WaitingForGreeting);
            return;
        }

        connection->Data=connection->decrypt(connection->Data);
        connection->NextBlockHeader.Size.Bytes=connection->Data.size();

        qDebug("Confirmación de handshake leída");

        connection->setState(Connection::ReadingGreetingOk);

        emit receivedHandshakeOk(connection);
        return;
    }

    if(connection->getState()==Connection::Ready)
    {
        qDebug("Leyendo cabecera mensaje");

        if(connection->bytesAvailable()<(uint)sizeof(Connection::RPEP_HEADER)) return;
        if(in.readRawData((char*)&connection->NextBlockHeader,sizeof(Connection::RPEP_HEADER))!=connection->round16(sizeof(Connection::RPEP_HEADER))) return;

        if(connection->NextBlockHeader.Size.bBlocks)
        {
            qWarning()<<"Mensaje por bloques no implementado aún, descartado";
            connection->readAll();
            return;
        }

        qDebug("Cabecera leída, identificando mensaje");

        if(!connection->NextBlockHeader.OperationType.bOperation)
        {
            qDebug()<<"Mensaje para el plugin #"+QString::number(connection->NextBlockHeader.OperationType.PluginID);

            QByteArray msg=connection->read(connection->NextBlockHeader.Size.Bytes);

            msg=connection->decrypt(msg);
            connection->NextBlockHeader.Size.Bytes=connection->Data.size();

            qDebug()<<"Mensaje leído, enviando mensaje al plugin #"+QString::number(connection->NextBlockHeader.OperationType.PluginID);
            emit receivedPluginMessage(connection,connection->NextBlockHeader.OperationType.PluginID,msg);
        }
    }
}

void MessageManager::dispatchMessage()
{

}
