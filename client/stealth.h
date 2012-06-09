#ifndef STEALTH_H
#define STEALTH_H

#include <QMainWindow>
#include <QStyleFactory>
#include <QMouseEvent>
#include <QGraphicsView>
#include <QTreeWidget>
#include <QFile>
#include <QDir>

#include "server.h"
#include "connectionmanager.h"
#include "messagemanager.h"
#include "grouptreewidget.h"

class ConnectionManager;
class MessageManager;

namespace Ui {
    class Stealth;
    class ItemInfo;
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
    QList<GroupTreeWidget*> lstGroupWidgets;
    GroupTreeWidget* expandedGroup;

public slots:
    void addConnection(Connection* connection);
private slots:
    void closeCurrentExpanded(GroupTreeWidget*);
};

#endif // STEALTH_H
