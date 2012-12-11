#ifndef PLUGINMANAGER_H_INCLUDED
#define PLUGINMANAGER_H_INCLUDED

#include "main.h"
#include "../ArkLib/src/datatypes.h"
#include "../ArkLib/src/CCrypt.h"

#include "RPEP.h"

class DArray;

class PluginInterface;
class plugin{
    public:
        ulong ID;
        HINSTANCE hModule;
        PluginInterface* plugInterface;
};
class PluginManagerInterfacePrivate;
class RPEP;
class PluginManager{
        LoaderFunTable* lFunc;
        RPEP* protocol;
        static PluginManagerInterfacePrivate* pluginList;
    public:
        PluginManager();

        bool loadPlugin(RPEP_LOAD_PLUGIN *pluginModule);
        bool unload(ushort ID);
        bool initPlugin();
        bool stopPlugin();
        bool enumPluginLoad();
        bool updateManager();
        bool updateServer(DArray& newServer);
        bool stopServer();

        uint run(SOCKET hConexion, HCRYPTKEY hKey, LoaderFunTable &lFunc);
        plugin* getPluginById(ulong id);
        RPEP* getProtocol();
};

extern PluginManager PlugMgr;

#endif // PLUGINMANAGER_H_INCLUDED
