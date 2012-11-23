#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include <QObject>
#include <QApplication>
#include <QPluginLoader>
#include <QDir>
#include <QList>

#include "plugininterface.h"

class pluginmanager : public QObject
{
    Q_OBJECT
public:
    explicit pluginmanager(QObject *parent = 0);

    QList<PluginInterface> plugins;
    
signals:
    
public slots:
    
};

#endif // PLUGINMANAGER_H
