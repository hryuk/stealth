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

public slots:
    void sendLoader(Connection*);
    void sendPluginManager(Connection*);
    void addConnection(Connection*);
    void processHandshake(Connection*);

signals:
    void connectionEstablished(Connection*);

};

#endif // CONNECTIONMANAGER_H
