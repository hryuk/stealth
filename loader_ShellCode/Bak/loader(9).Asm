	global main

	%include "loader.inc"
	%include "kernel32.inc"

	section data USE32
	dllmain@ db "dllmain@"
	nulo db 0;
	
	
	section code USE32
%undef LoadLibraryA
extern LoadLibraryA
%undef GetProcAddress
extern GetProcAddress


	proc main,LoadLibrary_,GetProcAddress_,hConexion,hashUserPass
	localdd kernel32
	localdd msvcrt
	localdd VirtualProtect_
	localdd OldProtect
	endl
	
	xor eax,eax
	inc eax
	pushad
	
	
	call GetDeltaOffset
	mov esi,eax
	push dllmain@
	call lstrlenA
	
	push kernel32N
	call LoadLibraryA
	mov dword %$kernel32,eax
	
	push VirtualProtectN
	push dword %$kernel32
	call GetProcAddress
	mov %$VirtualProtect_,eax
	
	;//Cambio la proteccion
	lea ebx,%$OldProtect
	push ebx
	push PAGE_EXECUTE_READWRITE
	push kernel32N-main;Calculo el tamaño de mi code
	push main	
	call eax;//VirtualProtect
	
	mov ebx,VirtualProtect
	mov eax,%$VirtualProtect_
	mov [ebx+esi+1],eax
	
	
	
	push VirtualAllocN
	push dword %$kernel32
	call GetProcAddress
	
	push msvcrtN
	call LoadLibraryA
	mov dword %$msvcrt,eax
	
	push bad_typeidN
	push eax
	call GetProcAddress
	
	popad
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
	popad
	
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
	xor ecx,ecx;Pomgo a cero
	
	.nextChar:;Para convertir a unicode, ensancho los caracteres con un 0 de relleno para que quede asi: 0xXX 0x00 0xXX 0x00
	movzx ax,byte [esi+ecx]
	mov [edi+ecx*2],ax
	test eax,eax
	jnz .nextChar
	
	popad
endp

	proc LoadLibraryFromMemoy, AddrBase, ModName
	LOCALDD PE
	LOCALDD SectionsHead
	LOCALDD ldr_module
	LOCALDD NewBase
	LOCALDD OldProtect
	LOCALDB RelocFlag
	endl

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
	mov edi,%$NewBase
	;Creamos un LDR_MODULE para luego añadir el modulo a la lista
	push eax
	push sizeof(LDR_MODULE)
	ecall2 malloc
	add esp,4
	mov %$ldr_module,eax
	;Rellenamos los campos
	mov ecx,[ebx+IMAGE_NT_HEADERS.OptionalHeader+IMAGE_OPTIONAL_HEADER.SizeOfImage]
	mov edx,[ebx+IMAGE_NT_HEADERS.OptionalHeader+IMAGE_OPTIONAL_HEADER.AddressOfEntryPoint]
	add edx,edi
	mov dword [eax+LDR_MODULE.BaseAddress],edi
	mov dword [eax+LDR_MODULE.SizeOfImage],ecx
	mov dword [eax+LDR_MODULE.EntryPoint],edx
	mov word [eax+LDR_MODULE.LoadCount],1
	push eax

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
	push  dword %$ModName
	push ebx
	call AscToUni

	pop eax
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

	pop eax


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


	;ahora hay que llamar al dllmain si existe, sino se llama al punto de entrada

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


	endp

	proc FreeLibraryFromMemoy, AddrBase
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
	mov eax,%$PE_base
	movzx eax,word[eax+IMAGE_NT_HEADERS.FileHeader+IMAGE_FILE_HEADER.SizeOfOptionalHeader]
	add eax,%$PE_base
	add eax,IMAGE_FILE_HEADER_size+IMAGE_NT_HEADERS.FileHeader
	endp
	proc PEGetDirectoryEntry,AddrBase,PE_base, Table
	push ebx
	mov eax,%$PE_base
	mov ebx,%$Table
	shl ebx,3
	mov eax,[eax+IMAGE_NT_HEADERS.OptionalHeader+IMAGE_OPTIONAL_HEADER.DataDirectory+IMAGE_DATA_DIRECTORY.VirtualAddress+ebx]
	add eax,%$AddrBase
	pop ebx
	endp
	proc PEBaseReloc,AddrBase,PE_base
	push IMAGE_DIRECTORY_ENTRY_BASERELOC
	push dword %$PE_base
	push dword %$AddrBase
	call PEGetDirectoryEntry
	endp

	proc PELoadIAT,AddrBase,PE_base
	localdd IAT
	localdd OldProtect
	endl
	pushad
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

	push eax
	ecall LoadLibraryA
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
	lea ebx,[ebx+IMAGE_IMPORT_DESCRIPTOR_size]
	jmp .nextImportModule
.endLoad:
	popad
	endp

	proc PEEntry, base, LDR_Entry
	endl;Macro para las var locales
	pushad
	mov ebx,%$LDR_Entry
	push dllmain@
	push dword %$base
	ecall2 GetProcAddress
	test eax,eax
	jz .nodllmain
	push dword 0
	push dword 1
	push dword %$base
	call eax
	or dword [ebx+LDR_MODULE.Flags],LDR_PROCESS_ATTACHED
	jmp .end
.nodllmain:
	mov ebx,%$LDR_Entry
	mov eax,[ebx+LDR_MODULE.EntryPoint]
	push dword 0
	push dword 1
	push dword %$base
	call eax
.end:
	popad
	endp

	proc AddLdrModules,module;PLDR_MODULE
	localdd ldr
	endl
	pushad
	mov esi,%$module
	mov eax,[fs:0x30]
	mov eax,[eax+0x0c]
	mov %$ldr,eax
	lea eax,[eax+PEB_LDR_DATA.InLoadOrderModuleList]
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
	push nulo
	push dword [esi+LDR_MODULE.BaseAddress-LDR_MODULE.InMemoryOrderModuleList]
	ecall2 GetProcAddress


	popad
	endp
	proc DelLdrModules,module_node
	endp

VirtualAlloc:
	db 0x68;push 0xffffffff
	dd -1
	ret
malloc:
	db 0x68;push 0xffffffff
	dd -1
	ret
VirtualProtect:
	db 0x68;push 0xffffffff
	dd -1
	ret
;LoadLibraryA:
	db 0x68;push 0xffffffff
	dd -1
	ret
;GetProcAddress:
	db 0x68;push 0xffffffff
	dd -1
	ret
kernel32N		db "kernel32",0
VirtualAllocN	db "VirtualAlloc",0
VirtualProtectN	db "VirtualProtect",0

msvcrtN			db "msvcrt",0
mallocN			db "malloc",0
bad_typeidN		db "??_U@YAPAXIHPBDH@Z",0