#ifndef __MAIN_H__
#define __MAIN_H__

#include <windows.h>

/*  To use this exported function of dll, include this header
 *  in your project.
 */

#ifdef BUILD_DLL
    #define DLL_EXPORT __declspec(dllexport)
#else
    #define DLL_EXPORT __declspec(dllimport)
#endif
typedef struct LoaderFunTable{
    //Puntero a las funciones para la carga y descarga de modulos desde memoria
    HINSTANCE (__stdcall *LoadLibraryFromMemoy)(void* AddrBase,const char* ModName);
    void* (__stdcall *FreeLibraryFromMemoy)(void* AddrBase);
    //Puntero a los punteros que usan las funiones anteriores para estas apis
    //Estan en una zona con permisos ReadExecute, pero se pueden cambiar
    unsigned long** LoadLibraryA;
    unsigned long** GetProcAddress;
}LoaderFunTable;
#ifdef __cplusplus
extern "C"
{
#endif

//void DLL_EXPORT SomeFunction(const LPCSTR sometext);
bool DLL_EXPORT WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved);
int DLL_EXPORT WINAPI InitPluginLoader(long hConexion,long hKey,LoaderFunTable& lFunc);

#ifdef __cplusplus
}
#endif

#endif // __MAIN_H__
