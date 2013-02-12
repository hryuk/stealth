#ifndef REMOTESHELL_H
#define REMOTESHELL_H

#include <windows.h>
#include "main.h"
#include "../PluginManager/plugininterface.h"


#ifdef __cplusplus
extern "C"
{
#endif

DLL_EXPORT PluginInterface* getInterface();

#ifdef __cplusplus
}
#endif
class RemoteShell:public PluginInterface{
        STARTUPINFO StartupInfo;
        PROCESS_INFORMATION ProcessInformation;
        HANDLE hReadPipe,hWritePipe;
        pluginManagerInterface* mgr;
        HANDLE hThreadReader;
        bool threadRuning;
        static int threadReader(RemoteShell *_this);
    public:
        RemoteShell();
        virtual ~RemoteShell();
        bool open();
        bool close();
        bool read(char **buffer, ulong* dataSize);
        bool write(char* buffer,ulong size);

        virtual const char* getPluginName();
        virtual int onReciveData(char* data,uint size);
        virtual void setPluginManager(pluginManagerInterface* mgr);
};

#endif // REMOTESHELL_H
