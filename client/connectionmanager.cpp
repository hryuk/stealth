#include "connectionmanager.h"

ConnectionManager::ConnectionManager(MessageManager* mngMessage,QObject* parent) : QObject(parent)
{
    this->mngMessage=mngMessage;

    connect(this,SIGNAL(ConnectionEstablished(Connection*)),this,SLOT(addConnection(Connection*)));
    //connect ConnectionEstablished -> GUI
}

void ConnectionManager::SetupConnection(Connection *connection)
{
    switch(connection->getSetupState()){

        case Connection::JustConnected:
            connect(connection,SIGNAL(readyRead()),mngMessage,SLOT(readMessage()));
            //---Comprobar contraseña--- (Implica cambios)
            //EnviarSizeOfImage
        break;

        case Connection::RecivedRelocationDir:
            //Rehubicar con RelocationDir
            //Enviar Loader IAT
            //Enviar Cargador

            connection->setState(Connection::Ready);
            //emit ConnectionEstablished(connection);
        break;
    }
}

void ConnectionManager::addConnection(Connection * connection)
{
    this->Connections.append(connection);
}
