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
    void receivedHandshake(Connection*);

};

#endif // MESSAGEMANAGER_H
