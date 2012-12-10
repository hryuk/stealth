#include "pluginmanager.h"

#include <QMessageBox>

PluginManager::PluginManager(QObject *parent) :
    QObject(parent)
{
    /* Cargamos todos los plugins */

    QDir pluginsDir=QDir(qApp->applicationDirPath());
    if(!pluginsDir.cd("plugins"))
    {
        qWarning()<<"No encontrado directorio /plugins";
        return;
    }

    pluginIndex=0;

    foreach(QString fileName,pluginsDir.entryList(QDir::Files))
    {
        QPluginLoader pluginLoader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin=pluginLoader.instance();
        if(plugin)
        {
            PluginInterface* stealthPlugin=qobject_cast<PluginInterface*>(plugin);
            if(stealthPlugin)
            {
                connect(plugin,SIGNAL(sendData(QByteArray)),this,SLOT(on_plugin_sendData(QByteArray)));
                plugins<<stealthPlugin;
                qDebug()<<"Cargado plugin "+stealthPlugin->getPluginName();
            }
            else
            {
                qDebug()<<"Error al cargar plugin inválido";
            }
        }
        else { qCritical("Error al cargar plugin inválido"); }
    }
}

int PluginManager::getPluginID(PluginInterface *plugin)
{
    return plugins.indexOf(plugin);
}

void PluginManager::on_plugin_sendData(QByteArray data)
{
    PluginInterface* plugin=qobject_cast<PluginInterface*>(sender());
    emit sendData(plugins.indexOf(plugin),data);
}

void PluginManager::on_plugin_recvData(int ID, QByteArray data)
{
    plugins.at(ID)->recvData(data);
}
