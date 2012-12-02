#undef UNICODE
#include <windows.h>
#include <iostream>

#include "main.h"
#include "PluginManager.h"

using namespace std;

bool WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved){
    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
            // attach to process
            // return FALSE to fail DLL load
            //SomeFunction("OnAttach");
            break;

        case DLL_PROCESS_DETACH:
            //SomeFunction("OnDetach");
            // detach from process
            break;

        case DLL_THREAD_ATTACH:
            //SomeFunction("OnNewThread");
            // attach to thread
            break;

        case DLL_THREAD_DETACH:
            //SomeFunction("OnExitThread");
            // detach from thread
            break;
    }
    return true; // succesful
}

int WINAPI InitPluginLoader(SOCKET hConexion,HCRYPTKEY hKey,LoaderFunTable& lFunc){
    //SomeFunction("InitPluginLoader");
    MessageBox(0,"","InitPluginLoader",0);

    PlugMgr.run(hConexion,hKey,lFunc);

    return 0;
}
