#include <windows.h>

#include "main.h"
#include "PluginManager.h"

using namespace std;
extern "C" bool DLL_EXPORT WINAPI _DllMainCRTStartup(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved){
    return true;
}
int main(){
    return 0;
}
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

int WINAPI InitPluginLoader(PSHELLCODE_CONTEXT ShellContext){
    uint status;
    PluginManager PlugMgr;
    //MessageBox(0,"InitPluginLoader","",0);

    status = PlugMgr.run(ShellContext);

    return status;
}
