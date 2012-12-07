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

    foreach(QString fileName,pluginsDir.entryList(QDir::Files))
    {
        QPluginLoader pluginLoader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin=pluginLoader.instance();
        if(plugin)
        {
            PluginInterface* stealthPlugin=qobject_cast<PluginInterface*>(plugin);
            if(stealthPlugin)
            {
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
