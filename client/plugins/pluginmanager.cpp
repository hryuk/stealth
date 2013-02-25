#include "pluginmanager.h"

#include <QMessageBox>

PluginManager::PluginManager(QObject *parent) :
    QObject(parent)
{
    /* Nos desplazamos al directorio de plugins */
    QDir pluginsDir=QDir(qApp->applicationDirPath());
    if(!pluginsDir.cd("plugins"))
    {
        qWarning()<<"No encontrado directorio /plugins";
        return;
    }

    pluginIndex=0;

    /* Recorremos todos los archivos */
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
                qDebug()<<"Cargado plugin #"+plugins.count()-1+stealthPlugin->getPluginName();
            }
            else
            {
                qWarning()<<"Error al cargar plugin inválido \""+fileName+"\"";
            }
        }
        else { qWarning()<<"Error al cargar plugin inválido \""+fileName+"\""; }
    }
}

int PluginManager::getPluginID(PluginInterface *plugin)
{
    return plugins.indexOf(plugin);
}

void PluginManager::on_plugin_sendData(QByteArray data)
{
    PluginInterface* plugin=qobject_cast<PluginInterface*>(sender());
    qDebug()<<"Plugin #"+QString::number(plugins.indexOf(plugin))+" envía un mensaje";
    emit sendData(plugins.indexOf(plugin),data);
}

void PluginManager::on_plugin_recvData(int ID, QByteArray data)
{
    qDebug()<<"Transfiriendo mensaje al plugin #"+QString::number(ID);
    plugins.at(ID)->recvData(data);
}
