#include "messagemanager.h"

MessageManager::MessageManager(QObject *parent) : QObject(parent)
{

}

void MessageManager::readMessage()
{
    Connection* connection=qobject_cast<Connection*>(sender());
    if(!connection) return;

    //Leer mensaje
    //Identificar mensaje
    //Procesar mensaje

    //emit newInternalMessage(QByteArray*)
    //emit newPluginMessage(QByteArray*)
}

