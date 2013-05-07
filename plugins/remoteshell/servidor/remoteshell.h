#ifndef REMOTESHELL_H
#define REMOTESHELL_H

#include <windows.h>

#include "main.h"
#include "plugininterface.h"


#ifdef __cplusplus
extern "C"
{
#endif

DLL_EXPORT PluginInterface* getInterface();

#ifdef __cplusplus
}
#endif

typedef struct Shell{
        STARTUPINFO startInfo;
        PROCESS_INFORMATION ProcessInformation;
        HANDLE hRead;
        HANDLE hWrite;
}Shell;

Shell* createShell();
bool deleteShell(Shell* shell);

int writeShell(Shell* shell, char* str, uint size);
int readShell(Shell* shell, char *buff, ulong size);

class RemoteShell:public PluginInterface{
        Shell* shell;
        pluginManagerInterface* mgr;
        HANDLE hThreadReader;
        bool threadRuning;
        char* recivedData;
        ulong sizeRecived;
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
