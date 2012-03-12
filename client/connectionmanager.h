#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H



#include "connection.h"
#include "messagemanager.h"
#include "crypto.h"


#include <QFile>

class ConnectionManager : public QObject
{
    Q_OBJECT
public:
    explicit ConnectionManager(MessageManager*,QObject* parent=0);

private:
    QList<Connection*> Connections;
    MessageManager* mngMessage;

public slots:
    void SetupConnection(Connection*);
    void addConnection(Connection*);

signals:
    void ConnectionEstablished(Connection*);

};

#endif // CONNECTIONMANAGER_H
