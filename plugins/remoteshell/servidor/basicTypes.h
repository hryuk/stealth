#ifndef BASICTYPES_H
#define BASICTYPES_H

#define null NULL
//#define DEBUG

#ifdef DEBUG
#define DebufPrintf(...) printf(__VA_ARGS__)
#else
#define DebufPrintf(...)
#endif

//Tipos unsigned
typedef unsigned char uchar;
typedef unsigned char byte;
typedef unsigned short ushort;
typedef unsigned short word;
typedef unsigned long ulong;
typedef unsigned long dword;
typedef unsigned int uint;
typedef long long llong;
typedef unsigned long long ullong;
typedef unsigned long long qword;
typedef long double ___int128;
//typedef unsigned long double dqword;
//Tipos constantes
typedef const uchar cuchar;
typedef const uchar cbyte;
typedef const ushort cushort;
typedef const ushort cword;
typedef const ulong culong;
typedef const ulong cdword;
typedef const uint cuint;
typedef const qword cqword;

typedef const char cchar;
typedef const short cshort;
typedef const long clong;
typedef const int cint;
typedef const llong cllong;

#endif // BASICTYPES_H
