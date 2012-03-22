/*###############################################################################
** Macros:
**    En este fichero de encabezado estan definidos los macros preprocesador
**    encargados de:
**        *Crear e introducir hashes de las funciones en el fichero binario.
**        *Funciones para introducir BYTE/WORD/DWORD en el fichero binario.
**        *Función encargada de generar los identificadores que situan las
**            direcciones en el 'stack de direcciones'
**        *Compilación condicionada encargada de generar:
**            +Un shellcode libre de bytes nulos
**            +Direcciones relativas que permitan la reubicación del código
**            +Un gestor de errores
*###############################################################################*/

#include "preprocessor/seq/for_each_i.hpp"
#include "preprocessor/seq/fold_left.hpp"
#include "preprocessor/seq/size.hpp"
#include "preprocessor/cat.hpp"


/*###############################################################################
** Error_Check:
**    Si se define la compilación ERR_CHECK el código generado tendrá
**    comprobación de errores.
**    NOTA:{
**        Se añade código, como consecuencia aumenta el tamaño
**    }
*###############################################################################*/
#define ERR_CHECK
#undef ERR_CHECK

#ifdef ERR_CHECK
    /*###############################################################################
    ** Control de errores:
    **    Constantes utilizadas en el control de errores.
    **    Solamente las excepciones críticas serán controladas, y
    **    se devolverá la constante de error correspondiente como
    **    'Exit Code' en ExitProcess()@kernel32
    *###############################################################################*/
    #define ERR_NO  0x0     //No ha habido ningun error. El server ha finalizado correctamente.
    #define ERR_FNC 0x1     //Ha habido un error en la funcion LoadFunctions(). Probablemente alguna función no se ha encontrado.
    #define ERR_HST 0x2     //Ha habido un error al resolver el Hostname. Probablemente debido a un problema de conexión.
    #define ERR_MEM 0x3     //Ha habido un error al reservar memoria.
    #define ERR_SUM 0x4     //Ha habido un error en la suma de comprobación.
#endif

/*###############################################################################
** Shellcode:
**    Si se define la compilación en modo 'shellcode' el código generado no tendrá
**    bytes nulos ni direcciones estáticas.
**    Se diferencian dos modos de shellcode:
**      [+] Direcciones relativas (SC_DELTA)
**      [+] Bytes nulos (SC_NULL)
**    
**    NOTA:{
**        Se añade código, como consecuencia aumenta el tamaño
**    }
*###############################################################################*/
#define SC_DELTA
#define SC_NULL
#undef SC_NULL
#undef SC_DELTA

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
** EMIT_* :
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
#ifdef SC_DELTA
    #define DELTA __asm{add DWORD PTR[esp], edi}
    /*###############################################################################
    ** movr:
    **    Macro que hace un mov con una direccion estatica y la repara
    **    TODO: COMPROBAR QUE EDI SEA EL DELTA SIEMPRE!!!
    *###############################################################################*/
    #define movr(r, addr)\
        movc((r), (addr))\
        __asm{add (r), edi}
#else
    #define DELTA ;
    /*###############################################################################
    ** movr:
    **    Macro que hace un mov con una direccion estatica y la repara
    **    TODO: COMPROBAR QUE EDI SEA EL DELTA SIEMPRE!!!
    *###############################################################################*/
    #define movr(r, addr)\
        movc((r), (addr))
#endif

#ifdef SC_NULL
    #define K 0xFEEDCAFE
    /*###############################################################################
    ** pushc:
    **    Macro que pushea en el stack una constante y eliminando los bytes nulos
    **    NOTA{Se harcodean los opcodes para ahorrar el opcode de identificador de segmento}
    *###############################################################################*/
    #define pushc(addr)\
        __asm{push ((addr)-K)}\
        __asm{add DWORD PTR[esp], K}
    /*###############################################################################
    ** movc:
    **    Los movs de direcciones suelen tener bytes nulos, pera evitar esto hay que 
    **    modificar las direcciones restandoles una constante sin bytes nulos.
    **    NOTA{
    **        *La constante K puede necesitar ser cambiada para limpiar todos los
    **        bytes nulos de todas las direcciones relativas
    **    }
    *###############################################################################*/
    #define movc(r, addr)\
        __asm{mov (r), ((addr)-K)}\
        __asm{add (r), (K)}
#else
    #define pushc(addr)\
        __asm{push (addr)}
    #define movc(r, addr)\
        __asm{mov (r), (addr)}
#endif
/*###############################################################################
** pushr:
**    Macro que pushea en el stack una dirección estatica y la repara
**    TODO: COMPROBAR QUE EDI SEA EL DELTA SIEMPRE!!!
*###############################################################################*/
#define pushr(addr)\
    pushc((addr))\
    DELTA