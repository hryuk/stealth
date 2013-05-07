#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QHostAddress>

#include "connection.h"

class Server : public QTcpServer
{
    Q_OBJECT

public:
    Server();

signals:
    void newConnection(Connection*);

protected:
    void incomingConnection(int socketDescriptor);

};

#endif // SERVER_H
