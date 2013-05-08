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

    /*FIXME: Los plugins se deberían cargar sólo una vez al principio */
    /* Recorremos todos los archivos */
    foreach(QString fileName,pluginsDir.entryList(QDir::Files))
    {
        QPluginLoader pluginLoader(pluginsDir.absoluteFilePath(fileName));

        if(!qobject_cast<PluginInterface*>(pluginLoader.instance()))
        {
            qWarning()<<"Error al cargar plugin inválido \""+fileName+"\"";
        }
        else
        {
            PluginInterface* stealthPlugin=qobject_cast<PluginInterface*>(pluginLoader.instance());
            QObject* instance=stealthPlugin->createInstance();
            connect(instance,SIGNAL(sendData(QByteArray)),this,SLOT(on_plugin_sendData(QByteArray)));
            plugins<<qobject_cast<PluginInterface*>(instance);
            qDebug()<<"Cargado plugin #"+QString::number(plugins.count()-1)+" "+plugins.last()->getPluginName();
        }
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

bool PluginManager::isPluginRunning(int ID)
{
    if(runningPlugins.contains(ID)) return true;
    return false;
}
void PluginManager::setPluginRunning(int ID)
{
    runningPlugins.append(ID);
}
