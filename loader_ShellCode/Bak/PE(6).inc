%define LDR_WINE_INTERNAL 0x80000000
%define LDR_IMAGE_IS_DLL                0x00000004
%define LDR_LOAD_IN_PROGRESS            0x00001000
%define LDR_UNLOAD_IN_PROGRESS          0x00002000
%define LDR_NO_DLL_CALLS                0x00040000
%define LDR_PROCESS_ATTACHED            0x00080000
%define LDR_MODULE_REBASED              0x00200000

;/* these ones is Wine specific */
%define LDR_DONT_RESOLVE_REFS           0x40000000
%define LDR_WINE_INTERNAL               0x80000000


STRUC IMAGE_DATA_DIRECTORY
	.VirtualAddress	RESD 1
	.isize 			RESD 1
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
    .PhysicalAddress
    .VirtualSize
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
	.Characteristics	
	.OriginalFirstThunk	resd 1;
	.TimeDateStamp		resd 1
	.ForwarderChain		resd 1
	.Name1				resd 1
	.FirstThunk			resd 1
ends
struc IMAGE_THUNK_DATA
	;Esto es una union
	.AddressOfData		;resd 1
	.OrdinalNameFlag	resw 1
	.OrdinalNumber		resw 1
ends

struc IMAGE_IMPORT_BY_NAME
	.Hint resw 1
	.Name1
ends
	
IMAGE_REL_BASED_ABSOLUTE      EQU   0
IMAGE_REL_BASED_HIGH          EQU   1
IMAGE_REL_BASED_LOW           EQU   2
IMAGE_REL_BASED_HIGHLOW       EQU   3
IMAGE_REL_BASED_HIGHADJ       EQU   4
IMAGE_REL_BASED_MIPS_JMPADDR  EQU   5

;IMAGE_RELOCATION_DATA   RECORD      {
;    RD_RelocType        :4
;    RD_RelocOffset      :12
;}

struc IMAGE_BASE_RELOCATION
	.BR_VirtualAddress	resd 1
	.BR_SizeOfBlock		resd 1
;   BR_TypeOffset       IMAGE_RELOCATION_DATA 1 DUP (?)   ; Array of zero or more relocations (type + RVAs)
ends

IMAGE_SIZEOF_BASE_RELOCATION  EQU   sizeof(IMAGE_BASE_RELOCATION)   ; 8