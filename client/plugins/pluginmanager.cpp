#include "pluginmanager.h"

pluginmanager::pluginmanager(QObject *parent) :
    QObject(parent)
{
    /* Cargamos todos los plugins */


    QDir pluginsDir=QDir(qApp->applicationDirPath());
    pluginsDir.cd("plugins");

    foreach(QString fileName,pluginsDir.entryList(QDir::Files))
    {
        QPluginLoader pluginLoader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin=pluginLoader.instance();
        if(plugin)
        {
            PluginInterface* stealthPlugin=qobject_cast<PluginInterface*>(plugin);
            if(stealthPlugin)
            {
               // plugins<<stealthPlugin;
            }
            else
            {
                /** ERROR **/
            }
        }
        else /** ERROR **/ ;
    }
}
