//#include <iostream>
//#include <stdio.h>
#include <stdarg.h>
#include <cstdlib>
#include <float.h>
#include <windows.h>

//#include <unwind.h>

#define __UNKNOWN_APP    0
#define __CONSOLE_APP    1
#define __GUI_APP        2
void __set_app_type(int);
typedef int (*_controlfp)(unsigned a, unsigned b);

typedef struct
{
    int newmode;
} _startupinfo;

void getmainargs(int *pargc, char ***pargv, char ***penv, int globb, _startupinfo*);

int main(int argc, char **argv, char **env);
extern "C" bool WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved);

typedef int (__cdecl *GETMAINARGS)(int*, char***, char***, int, _startupinfo*);


int STDCALL _start(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved){
    int argc = 0; char **argv = 0; char **env = 0;
    _startupinfo start_info = {0};
    HINSTANCE  hMsvcrt = LoadLibrary("msvcrt");
    GETMAINARGS getmainargs;
    _controlfp controlfp;

    getmainargs = (GETMAINARGS) GetProcAddress(hMsvcrt,"__getmainargs");
    controlfp = (_controlfp) GetProcAddress(hMsvcrt,"_controlfp");

    controlfp(0x10000, 0x30000);
    //__set_app_type(__CONSOLE_APP);
    getmainargs(&argc, &argv, &env, 0, &start_info);

    #ifdef BUILD_DLL
    return DllMain(hinstDLL,fdwReason,lpvReserved);
    #else
    int ret;
    ret = main(argc, argv, env);
    exit(ret);
    #endif
}
extern "C" int __main(){
    //return main(argc, argv, env);
    return 0;
}

/* Level 1: Base ABI  */

/* @@@ The IA-64 ABI uses uint64 throughout.  Most places this is
   inefficient for 32-bit and smaller machines.  */
typedef unsigned _Unwind_Word __attribute__((__mode__(__unwind_word__)));
typedef signed _Unwind_Sword __attribute__((__mode__(__unwind_word__)));
#if defined(__ia64__) && defined(__hpux__)
typedef unsigned _Unwind_Ptr __attribute__((__mode__(__word__)));
#else
typedef unsigned _Unwind_Ptr __attribute__((__mode__(__pointer__)));
#endif
typedef unsigned _Unwind_Internal_Ptr __attribute__((__mode__(__pointer__)));

/* @@@ The IA-64 ABI uses a 64-bit word to identify the producer and
   consumer of an exception.  We'll go along with this for now even on
   32-bit machines.  We'll need to provide some other option for
   16-bit machines and for machines with > 8 bits per byte.  */
typedef unsigned _Unwind_Exception_Class __attribute__((__mode__(__DI__)));

/* The unwind interface uses reason codes in several contexts to
   identify the reasons for failures or other actions.  */
typedef enum _Unwind_Reason_Code{
  _URC_NO_REASON = 0,
  _URC_FOREIGN_EXCEPTION_CAUGHT = 1,
  _URC_FATAL_PHASE2_ERROR = 2,
  _URC_FATAL_PHASE1_ERROR = 3,
  _URC_NORMAL_STOP = 4,
  _URC_END_OF_STACK = 5,
  _URC_HANDLER_FOUND = 6,
  _URC_INSTALL_CONTEXT = 7,
  _URC_CONTINUE_UNWIND = 8
} _Unwind_Reason_Code;


/* The unwind interface uses a pointer to an exception header object
   as its representation of an exception being thrown. In general, the
   full representation of an exception object is language- and
   implementation-specific, but it will be prefixed by a header
   understood by the unwind interface.  */

struct _Unwind_Exception;

typedef void (*_Unwind_Exception_Cleanup_Fn) (_Unwind_Reason_Code,
                          struct _Unwind_Exception *);

struct _Unwind_Exception
{
  _Unwind_Exception_Class exception_class;
  _Unwind_Exception_Cleanup_Fn exception_cleanup;
  _Unwind_Word private_1;
  _Unwind_Word private_2;

  /* @@@ The IA-64 ABI says that this structure must be double-word aligned.
     Taking that literally does not make much sense generically.  Instead we
     provide the maximum alignment required by any type for the machine.  */
} __attribute__((__aligned__));

typedef int _Unwind_Action;

#define _UA_SEARCH_PHASE	1
#define _UA_CLEANUP_PHASE	2
#define _UA_HANDLER_FRAME	4
#define _UA_FORCE_UNWIND	8
#define _UA_END_OF_STACK	16



extern "C" void __cxa_pure_virtual(){
    // Do nothing or print an error message.
}

void *operator new(size_t size){
    return malloc(size);
}

void *operator new[](size_t size){
    return malloc(size);
}

void operator delete(void *p){
    free(p);
}
void operator delete[](void *p){
    free(p);
}
extern "C" void _Unwind_Resume (struct _Unwind_Exception *){
}

extern "C" void* __cxa_allocate_exception(size_t thrown_size) throw(){
    return malloc(thrown_size);
}
extern "C" void __cxa_free_exception(void * thrown_exception) throw(){
    free(thrown_exception);
}
extern "C" void* __cxa_allocate_dependent_exception() throw(){
    return 0;
}
extern "C" void __cxa_free_dependent_exception (void* dependent_exception) throw(){
}

extern "C" void __cxa_throw(void* thrown_exception, struct std::type_info * tinfo, void (*dest)(void*)){
}

extern "C" void* __cxa_get_exception_ptr(void* exceptionObject) throw(){
    exceptionObject = exceptionObject;
    return 0;
}

extern "C" void* __cxa_begin_catch(void* exceptionObject) throw(){
    exceptionObject = exceptionObject;
    return 0;
}

extern "C" void __cxa_end_catch(){
}

extern "C" std::type_info* __cxa_current_exception_type(){
    return 0;
}

extern "C" void __cxa_rethrow(){
}

extern "C" void* __cxa_current_primary_exception() throw(){
    return 0;
}

extern "C" void __cxa_decrement_exception_refcount(void* primary_exception) throw(){
}

//extern "C" __cxa_eh_globals* __cxa_get_globals() throw();

extern "C" _Unwind_Reason_Code __gxx_personality_v0 (int, _Unwind_Action, _Unwind_Exception_Class, struct _Unwind_Exception *, struct _Unwind_Context *){
    return _URC_NO_REASON;
}
extern "C" void _pei386_runtime_relocator (void){
}
extern "C" void __chkstk_ms(void){
}//*
int vfprintf (char * s, const char * format, __gnuc_va_list arg );
extern "C" int __mingw_vprintf (char * s, const char * format, __gnuc_va_list arg ){
    return 0/*vfprintf(s,format,arg)*/;
}
//*/

