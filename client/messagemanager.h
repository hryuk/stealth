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

public slots:
    void readMessage();
    void dispatchMessage();
};

#endif // MESSAGEMANAGER_H
