#ifndef MESSAGEMANAGER_H
#define MESSAGEMANAGER_H

#include "connection.h"

#include <QObject>
#include <QDataStream>

class MessageManager : public QObject
{
    Q_OBJECT
public:
    explicit MessageManager(QObject* parent=0);

private:

public slots:
    void readMessage();
    void dispatchMessage();

signals:
    void receivedHandshakeOk(Connection*);
    void receivedHandshake(Connection*);
    void receivedLoaderOk(Connection*);
    void receivedPluginMessage(Connection*,int PluginID,QByteArray data);
};

#endif // MESSAGEMANAGER_H
