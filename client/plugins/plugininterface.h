#ifndef PLUGININTERFACE_H
#define PLUGININTERFACE_H

#include <QtPlugin>
#include <QIcon>
#include <QWidget>
#include <QByteArray>

class PluginInterface
{
public:
    virtual QWidget* getGUI()=0;
    virtual QString getPluginName()=0;
    virtual QIcon getIcon()=0;
    virtual void recvData(QByteArray)=0;
    virtual QByteArray serverPlugin()=0;
};

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(PluginInterface,"Stealth.Plugins/1.0")
QT_END_NAMESPACE

#endif // PLUGININTERFACE_H
