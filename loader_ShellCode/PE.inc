;//Flag
;MAGE_SCN_TYPE_REG               EQU  00000000h ; Reserved
;MAGE_SCN_TYPE_REGULAR           EQU  00000000h ; Reserved
;MAGE_SCN_TYPE_DSECT             EQU  00000001h ; Reserved
;MAGE_SCN_TYPE_DUMMY             EQU  00000001h ; Reserved
;MAGE_SCN_TYPE_NOLOAD            EQU  00000002h ; Reserved
;MAGE_SCN_TYPE_NO_LOAD           EQU  00000002h ; Reserved
;MAGE_SCN_TYPE_GROUP             EQU  00000004h ; Reserved. Used for 16-bit offset code
;MAGE_SCN_TYPE_GROUPED           EQU  00000004h ; Reserved. Used for 16-bit offset code
IMAGE_SCN_TYPE_NO_PAD            EQU  00000008h ; Reserved
;MAGE_SCN_TYPE_COPY              EQU  00000010h ; Reserved
IMAGE_SCN_CNT_CODE               EQU  00000020h ; Section contains code.
IMAGE_SCN_CNT_INITIALIZED_DATA   EQU  00000040h ; Section contains initialized data.
IMAGE_SCN_CNT_UNINITIALIZED_DATA EQU  00000080h ; Section contains uninitialized data.
IMAGE_SCN_LNK_OTHER              EQU  00000100h ; Reserved.
IMAGE_SCN_LNK_INFO               EQU  00000200h ; Section contains comments or some other type of information.
;MAGE_SCN_TYPE_OVER              EQU  00000400h ; Reserved. Section contains an overlay.
;MAGE_SCN_LNK_OVERLAY            EQU  00000400h ; Reserved. Section contains an overlay.
IMAGE_SCN_LNK_REMOVE             EQU  00000800h ; Section contents will not become part of image.
IMAGE_SCN_LNK_COMDAT             EQU  00001000h ; Section contents comdat.
;                                EQU  00002000h ; Reserved.
;MAGE_SCN_MEM_PROTECTED          EQU  00004000h ; Obsolete.
IMAGE_SCN_MEM_FARDATA            EQU  00008000h
;MAGE_SCN_MEM_SYSHEAP            EQU  00010000h ; Obsolete.
IMAGE_SCN_MEM_PURGEABLE          EQU  00020000h
IMAGE_SCN_MEM_16BIT              EQU  00020000h
IMAGE_SCN_MEM_LOCKED             EQU  00040000h
IMAGE_SCN_MEM_PRELOAD            EQU  00080000h
IMAGE_SCN_ALIGN_1BYTES           EQU  00100000h
IMAGE_SCN_ALIGN_2BYTES           EQU  00200000h
IMAGE_SCN_ALIGN_4BYTES           EQU  00300000h
IMAGE_SCN_ALIGN_8BYTES           EQU  00400000h
IMAGE_SCN_ALIGN_16BYTES          EQU  00500000h ; Default alignment if no others are specified.
IMAGE_SCN_ALIGN_32BYTES          EQU  00600000h
IMAGE_SCN_ALIGN_64BYTES          EQU  00700000h
;                                EQU  00800000h ; Unused.
IMAGE_SCN_LNK_NRELOC_OVFL        EQU  01000000h ; Section contains extended relocations.
IMAGE_SCN_MEM_DISCARDABLE        EQU  02000000h ; Section can be discarded.
IMAGE_SCN_MEM_NOT_CACHED         EQU  04000000h ; Section is not cachable.
IMAGE_SCN_MEM_NOT_PAGED          EQU  08000000h ; Section is not pageable.
IMAGE_SCN_MEM_SHARED             EQU  10000000h ; Section is shareable.
IMAGE_SCN_MEM_EXECUTE            EQU  20000000h ; Section is executable.
IMAGE_SCN_MEM_READ               EQU  40000000h ; Section is readable.
IMAGE_SCN_MEM_WRITE              EQU  80000000h ; Section is writeable.
IMAGE_DLLCHARACTERISTICS_DYNAMIC_BASE EQU 0x0040;//Indica si puede ser cambiada de base

IMAGE_DIRECTORY_ENTRY_EXPORT		equ 0
IMAGE_DIRECTORY_ENTRY_IMPORT		equ 1
IMAGE_DIRECTORY_ENTRY_RESOURCE		equ 2
IMAGE_DIRECTORY_ENTRY_EXCEPTION		equ 3
IMAGE_DIRECTORY_ENTRY_SECURITY		equ 4
IMAGE_DIRECTORY_ENTRY_BASERELOC		equ 5
IMAGE_DIRECTORY_ENTRY_DEBUG			equ 6
IMAGE_DIRECTORY_ENTRY_COPYRIGHT		equ 7
IMAGE_DIRECTORY_ENTRY_GLOBALPTR		equ 8
IMAGE_DIRECTORY_ENTRY_TLS			equ 9
IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG	equ 10
IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT	equ 11
IMAGE_DIRECTORY_ENTRY_IAT			equ 12
IMAGE_NUMBEROF_DIRECTORY_ENTRIES	equ 16

STRUC IMAGE_DATA_DIRECTORY
	.VirtualAddress	RESD 1
	.size 			RESD 1
ends

STRUC IMAGE_OPTIONAL_HEADER
	.Magic							RESW 1
	.MajorLinkerVersion				RESB 1
	.MinorLinkerVersion				RESB 1
	.SizeOfCode						RESD 1
	.SizeOfInitializedData			RESD 1
	.SizeOfUninitializedData		RESD 1
	.AddressOfEntryPoint			RESD 1
	.BaseOfCode						RESD 1
	.BaseOfData						RESD 1
	.ImageBase						RESD 1
	.SectionAlignment				RESD 1
	.FileAlignment					RESD 1
	.MajorOperatingSystemVersion	RESW 1
	.MinorOperatingSystemVersion	RESW 1
	.MajorImageVersion				RESW 1
	.MinorImageVersion				RESW 1
	.MajorSubsystemVersion			RESW 1
	.MinorSubsystemVersion			RESW 1
	.Reserved1						RESD 1
	.SizeOfImage					RESD 1
	.SizeOfHeaders					RESD 1
	.CheckSum						RESD 1
	.Subsystem						RESW 1
	.DllCharacteristics				RESW 1
	.SizeOfStackReserve				RESD 1
	.SizeOfStackCommit				RESD 1
	.SizeOfHeapReserve				RESD 1
	.SizeOfHeapCommit				RESD 1
	.LoaderFlags					RESD 1
	.NumberOfRvaAndSizes			RESD 1
	.DataDirectory					RESQ 1
ends

STRUC IMAGE_FILE_HEADER
	.Machine				RESW 1
	.NumberOfSections		RESW 1
	.TimeDateStamp			RESD 1
	.PointerToSymbolTable	RESD 1
	.NumberOfSymbols		RESD 1
	.SizeOfOptionalHeader	RESW 1
	.Characteristics		RESW 1
ends

STRUC IMAGE_NT_HEADERS
	.Signature		RESD 1
	.FileHeader		RESB sizeof(IMAGE_FILE_HEADER);
	.OptionalHeader	RESD 1
ends


STRUC IMAGE_EXPORT_DIRECTORY
	.Characteristics		RESD 1
	.TimeDateStamp			RESD 1
	.MajorVersion			RESW 1
	.MinorVersion			RESW 1
	.nName					RESD 1
	.nBase					RESD 1
	.NumberOfFunctions		RESD 1
	.NumberOfNames			RESD 1
	.AddressOfFunctions		RESD 1
	.AddressOfNames			RESD 1
	.AddressOfNameOrdinals	RESW 1
ends

STRUC IMAGE_DOS_HEADER
	.e_magic	RESW 1
	.e_cblp		RESW 1
	.e_cp		RESW 1
	.e_crlc		RESW 1
	.e_cparhdr	RESW 1
	.e_minalloc RESW 1
	.e_maxalloc RESW 1
	.e_ss		RESW 1
	.e_sp		RESW 1
	.e_csum		RESW 1
	.e_ip		RESW 1
	.e_cs		RESW 1
	.e_lfarlc	RESW 1
	.e_ovno		RESW 1
	.e_res4		RESW 1
	.e_oemid	RESW 1
	.e_oeminfo	RESW 1
	.e_res2		RESW 1
	.e_lfanew	RESD 1
ends

struc IMAGE_SECTION_HEADER
	.Name 					resb 8
    .Misc 					resd 1
    .PhysicalAddress:
    .VirtualSize:
    .VirtualAddress			resd 1
    .SizeOfRawData			resd 1
    .PointerToRawData		resd 1
    .PointerToRelocations	resd 1
    .PointerToLinenumbers	resd 1
    .NumberOfRelocations	resw 1
    .NumberOfLinenumbers	resw 1
    .Characteristics		resd 1
ends

struc IMAGE_IMPORT_DESCRIPTOR
	.Characteristics:
	.OriginalFirstThunk	resd 1;
	.TimeDateStamp		resd 1
	.ForwarderChain		resd 1
	.Name1				resd 1
	.FirstThunk			resd 1
ends
struc IMAGE_THUNK_DATA
	;Esto es una union
	.AddressOfData:		;resd 1
	.OrdinalNameFlag	resw 1
	.OrdinalNumber		resw 1
ends

struc IMAGE_IMPORT_BY_NAME
	.Hint resw 1
	.Name1:
ends
	
IMAGE_REL_BASED_ABSOLUTE      EQU   0
IMAGE_REL_BASED_HIGH          EQU   1
IMAGE_REL_BASED_LOW           EQU   2
IMAGE_REL_BASED_HIGHLOW       EQU   3

IMAGE_REL_BASED_HIGHADJ       EQU   4
IMAGE_REL_BASED_MIPS_JMPADDR  EQU   5

;typedef struct
;{
;  WORD RelocOfs : 12;
;  WORD RelocType:  4;
;} IMAGE_RELOCATION_DATA;

struc IMAGE_BASE_RELOCATION
	.BR_VirtualAddress	resd 1
	.BR_SizeOfBlock		resd 1
;   BR_TypeOffset       IMAGE_RELOCATION_DATA 1 DUP (?)   ; Array of zero or more relocations (type + RVAs)
ends

IMAGE_SIZEOF_BASE_RELOCATION  EQU   sizeof(IMAGE_BASE_RELOCATION)   

struc TLS_DIRECTORY
	.RawDataStartVA		resd 1
	.RawDataEndVA		resd 1
	.AddressOfIndex		resd 1
	.AddressOfCallbacks	resd 1;Array de punteros a las funciones Callback terminado en nulo
	.SizeOfZeroFill		resd 1
	.Characteristics	resd 1
ends
;//Proto de TLS_Callback
;//void NTAPI tls_callback(PVOID hinstance, DWORD dwReason, PVOID reservado)
