#ifndef STEALTH_H
#define STEALTH_H

#include <QMainWindow>

#include "server.h"
#include "connectionmanager.h"
#include "messagemanager.h"

class ConnectionManager;

namespace Ui {
    class Stealth;
}

class Stealth : public QMainWindow
{
    Q_OBJECT

public:
    explicit Stealth(QWidget *parent = 0);
    ~Stealth();

private:
    Ui::Stealth *ui;
    Server* server;
    ConnectionManager* mngConnection;
    MessageManager* mngMessage;

public slots:
    void addConnection(Connection* connection);
};

#endif // STEALTH_H
