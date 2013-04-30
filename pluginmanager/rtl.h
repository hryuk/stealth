#include <Windows.h>

int __cdecl _purecall(void);
void * __cdecl operator new(unsigned int s);
void __cdecl operator delete(void* p);
void * __cdecl malloc(size_t size);
void * __cdecl memcpy(void *dst,const void *src,size_t size);
#pragma intrinsic(memcpy)
void * __cdecl memset(void *dst,int c,size_t count);
#pragma intrinsic(memset)
void __cdecl free(void *ptr);
void * __cdecl realloc(void * p, size_t size);

int __cdecl _printf(const char * format, ...);
