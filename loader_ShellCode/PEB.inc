struc PEB_LDR_DATA
    .Length								resd    1;ULONG
    .Initialized						resd    1;BOOLEAN
    .SsHandle							resd    1;PVOID
    .InLoadOrderModuleList				resd    2;LIST_ENTRY
    .InMemoryOrderModuleList			resd    2;LIST_ENTRY
    .InInitializationOrderModuleList	resd    2;LIST_ENTRY
ends
 
 
struc LDR_MODULE
    .InLoadOrderModuleList				resd 2;;LIST_ENTRY         
    .InMemoryOrderModuleList			resd 2;;LIST_ENTRY         
    .InInitializationOrderModuleList	resd 2;;LIST_ENTRY         
    .BaseAddress						resd 1;;void*               
    .EntryPoint							resd 1;;void*               
    .SizeOfImage						resd 1;;ULONG               
    .FullDllName						resd 2;;UNICODE_STRING     
    .BaseDllName						resd 2;;UNICODE_STRING     
    .Flags								resd 1;;ULONG               
    .LoadCount							resw 1;;SHORT               
    .TlsIndex							resw 1;;SHORT               
    .SectionHandle						resd 1;;HANDLE             
    .CheckSum							resd 1;ULONG               
    .TimeDateStamp						resd 1;ULONG
ends