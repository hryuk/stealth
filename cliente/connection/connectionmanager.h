#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include <QFile>
#include <QtCrypto>

#include "connection.h"
#include "stealth.h"
#include "messagemanager.h"
#include "crypto.h"

class Stealth;
class MessageManager;

class ConnectionManager : public QObject
{
    Q_OBJECT
public:
    explicit ConnectionManager(Stealth*,MessageManager*,QObject* parent=0);

private:
    QList<Connection*> Connections;
    MessageManager* mngMessage;
    Stealth* stealth;
    int connectionIndex;

public slots:
    void sendLoader(Connection*);
    void sendPluginManager(Connection*);
    void addConnection(Connection*);
    void processHandshake(Connection*);
    void checkHandshakeOk(Connection*);
    Connection* connection(int ID);

private slots:
    void connection_timeout();
    void connection_disconnected();
    void connectionError(QAbstractSocket::SocketError);

signals:
    void connectionReady(Connection*);
    void connectionDeleted(int ID);
};

#endif // CONNECTIONMANAGER_H
