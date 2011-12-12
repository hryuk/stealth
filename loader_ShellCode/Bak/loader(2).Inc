
%include "MACROS.inc"
%include "PEB.inc"
%include "PE.inc"
%include "WIN32N.INC"

struc LoaderParam

ends
struc LIST_ENTRY
	.Flink	resd 1
	.Blink	resd 1
ends

struc UNICODE_STRING
     .Length		resw 1;
     .MaximumLength	resw 1;
     .Buffer		resd 1;
ends
;//Valores conbinables de LDR_MODULE.Flags
%define LDR_IMAGE_IS_DLL                0x00000004
%define LDR_LOAD_IN_PROGRESS            0x00001000
%define LDR_UNLOAD_IN_PROGRESS          0x00002000
%define LDR_NO_DLL_CALLS                0x00040000
%define LDR_PROCESS_ATTACHED            0x00080000
%define LDR_MODULE_REBASED              0x00200000

%define LDR_STATIC_LINK					0x0000002
%define LDR_IMAGE_DLL					0x0000004
%define LDR_LOAD_IN_PROGRESS			0x0001000
%define LDR_UNLOAD_IN_PROGRESS			0x0002000
%define LDR_ENTRY_PROCESSED				0x0004000
%define LDR_ENTRY_INSERTED				0x0008000
%define LDR_CURRENT_LOAD				0x0010000
%define LDR_FAILED_BUILTIN_LOAD			0x0020000
%define LDR_DONT_CALL_FOR_THREADS		0x0040000
%define LDR_PROCESS_ATTACH_CALLED		0x0080000
%define LDR_DEBUG_SYMBOLS_LOADED		0x0100000
%define LDR_IMAGE_NOT_AT_BASE			0x0200000
%define LDR_WX86_IGNORE_MACHINETYPE		0x0400000

;/* these ones is Wine specific */
%define LDR_DONT_RESOLVE_REFS           0x40000000
%define LDR_WINE_INTERNAL               0x80000000