#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include <QObject>
#include <QApplication>
#include <QPluginLoader>
#include <QDebug>
#include <QDir>
#include <QList>

#include "plugininterface.h"

class PluginManager : public QObject
{
    Q_OBJECT
public:
    explicit PluginManager(QObject *parent = 0);

    QList<PluginInterface*> plugins;

private:
    int pluginIndex;
    QList<int> runningPlugins;
    
signals:
    void sendData(int PluginID,QByteArray data);
    void sendPluginData(QByteArray data);
    
public slots:
    int getPluginID(PluginInterface* plugin);
    void on_plugin_sendData(QByteArray data);
    void on_plugin_recvData(int ID,QByteArray data);
    bool isPluginRunning(int ID);
    void setPluginRunning(int ID);
};

#endif // PLUGINMANAGER_H
