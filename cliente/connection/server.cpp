#include "server.h"

Server::Server()
{
    this->listen(QHostAddress::Any,11155);

    qDebug()<<"Escuchando por el puerto "+QString::number(11155)+"...";
}

/*Si hay una conexión prendiente, creamos un nuevo socket
 *y decimos que la conexión está lista para inicializar */
void Server::incomingConnection(int socketDescriptor)
{
    Connection* connection=new Connection();
    connection->setState(Connection::JustConnected);
    connection->setSocketDescriptor(socketDescriptor);

    qDebug()<<"Conexión recibida";

    emit newConnection(connection);
}
