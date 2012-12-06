#ifndef STEALTH_H
#define STEALTH_H

#include <QMainWindow>
#include <QStyleFactory>
#include <QMouseEvent>
#include <QGraphicsView>
#include <QTreeWidget>
#include <QFontDatabase>
#include <QTextCodec>
#include <QFile>
#include <QDir>

#include "server.h"
#include "connectionmanager.h"
#include "messagemanager.h"
#include "grouptreewidget.h"
#include "pluginmanager.h"
#include "pluginwindow.h"
#include "debugshell.h"

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
    ConnectionManager* connectionManager;
    MessageManager* messageManager;
    PluginManager* pluginManager;
    GroupTreeWidget* treewidget;

public slots:
    void addConnection(Connection* connection);
private slots:
    void closeCurrentExpanded(GroupTreeWidget*);
    void itemDoubleClicked(QTreeWidgetItem*,int);
    void on_btnDebug_clicked();
};

#endif // STEALTH_H
