    LPCSTR              Strings;

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