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
#include <QList>
#include <QShowEvent>

#include "server.h"
#include "connectionmanager.h"
#include "messagemanager.h"
#include "grouptreewidget.h"
#include "pluginmanager.h"
#include "pluginwindow.h"
#include "debugshell.h"
#include "stealthtab1.h"
#include "stealthtab2.h"
#include "stealthtab3.h"
#include "stealthtab4.h"
#include "slidingstackedwidget.h"

class MessageManager;
class ConnectionManager;

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
    void showEvent(QShowEvent*);

private:
    Server* server;
    ConnectionManager* connectionManager;
    MessageManager* messageManager;
    PluginManager* pluginManager;
    Ui::Stealth *ui;
    StealthTab1* stab1;
    StealthTab2* stab2;
    StealthTab3* stab3;
    StealthTab4* stab4;
    SlidingStackedWidget* slidingStackedWidget;
    QList<PluginWindow*> pluginWindows;
    bool bdebugShell;

public slots:
    void addConnection(Connection* connection);

private slots:
    void itemDoubleClicked(QTreeWidgetItem*,int);
    void on_btnDebug_clicked();
    void processPluginMessage(Connection*,int PluginID,QByteArray data);
    void debugSheell_destroyed();
    void deleteConnection(int ID);
    void on_btnConnections_toggled(bool checked);
    void on_btnConfiguration_toggled(bool checked);
    void on_btnPlugins_toggled(bool checked);
    void on_btnServer_toggled(bool checked);
};

#endif // STEALTH_H
