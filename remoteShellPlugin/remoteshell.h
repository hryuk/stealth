#ifndef REMOTESHELL_H
#define REMOTESHELL_H

#include <windows.h>
#include "../PluginManager/plugininterface.h"

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
        ~RemoteShell();
        bool open();
        bool close();
        bool read(char** buffer,ulong* dataSize);
        bool write(char* buffer,ulong size);

        virtual const char* getPluginName();
        virtual int onReciveData(char* data,uint size);
        virtual void setPluginManager(pluginManagerInterface* mgr);
};

#endif // REMOTESHELL_H
