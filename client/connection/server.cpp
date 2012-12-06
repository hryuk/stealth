#include "server.h"

Server::Server()
{
    this->listen(QHostAddress::Any,2000);
}

//Si hay una conexión prendiente, creamos un nuevo socket
//y decimos que la conexión está lista para inicializar
void Server::incomingConnection(int socketDescriptor)
{
    Connection* connection=new Connection();
    connection->setState(Connection::JustConnected);
    connection->setSocketDescriptor(socketDescriptor);

    qDebug()<<tr("Recibida conexion");

    emit newConnection(connection);
}
