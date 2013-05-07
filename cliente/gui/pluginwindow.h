#ifndef PLUGINWINDOW_H
#define PLUGINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QDesktopWidget>

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
    PluginManager* pluginManager;
    
private:
    Ui::PluginWindow* ui;
    int ID;
    Connection* connection;
    QVBoxLayout* leftBoxLayout;

private slots:
    void leftBoxButton_clicked(bool);

public slots:
    int getID();
};

#endif // PLUGINWINDOW_H
