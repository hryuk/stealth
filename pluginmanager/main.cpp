#pragma comment(linker,"/ENTRY:DllMain")

#include "main.h"
#include "PluginManager.h"

bool WINAPI DllMain(HINSTANCE,DWORD,LPVOID){
    return true;
}

int WINAPI InitPluginLoader(PSHELLCODE_CONTEXT ShellContext){
    uint status;
    PluginManager PlugMgr;

	DebufPrintf("[pm] InitPluginLoader");
    //MessageBox(0,"InitPluginLoader","",0);

    status = PlugMgr.run(ShellContext);

    return status;
}
