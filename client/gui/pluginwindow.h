#ifndef PLUGINWINDOW_H
#define PLUGINWINDOW_H

#include <QMainWindow>
#include "connection.h"
#include "pluginmanager.h"
#include "QHostAddress"

namespace Ui {
class PluginWindow;
}

class PluginWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit PluginWindow(Connection* connection,QWidget *parent = 0);
    ~PluginWindow();
    void showEvent(QShowEvent*);
    
private:
    Ui::PluginWindow *ui;
    int ID;
    Connection* connection;
    PluginManager* pluginManager;

public slots:
    int getID();
};

#endif // PLUGINWINDOW_H
