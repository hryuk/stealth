#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include <QFile>
#include <QtCrypto>

#include "connection.h"
#include "stealth.h"
#include "messagemanager.h"
#include "crypto.h"

class Stealth;

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
    void SetupConnection(Connection*);
    void addConnection(Connection*);

signals:
    void ConnectionEstablished(Connection*);

};

#endif // CONNECTIONMANAGER_H
