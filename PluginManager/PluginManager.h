#ifndef PLUGINMANAGER_H_INCLUDED
#define PLUGINMANAGER_H_INCLUDED

#include "main.h"
#include "../ArkLib/src/datatypes.h"
#include "../ArkLib/src/CCrypt.h"

#include "RPEP.h"

class DArray;


class PluginManager{
    public:
        PluginManager();

        bool initPlugin();
        bool stopPlugin();
        bool enumPluginLoad();
        bool updateManager();
        bool updateServer(DArray& newServer);
        bool stopServer();

        uint run(SOCKET hConexion,HCRYPTKEY hKey,LoaderFunTable& lFunc);
};

extern PluginManager PlugMgr;

#endif // PLUGINMANAGER_H_INCLUDED
