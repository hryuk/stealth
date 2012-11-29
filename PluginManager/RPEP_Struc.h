#ifndef RPEP_STRUC_H_INCLUDED
#define RPEP_STRUC_H_INCLUDED

typedef struct RPEP_HEADER{
    /* Codigo de operacion */
    typedef enum Operation:unsigned short{
        /*  Operaciones sobre la negociacion */
        ClientHandshake = 1,
        ServerHandshake,
        SetPacketSize,
        SetCompressionAlgm,
        SetSecondaryPort,
        PingRequest,
        PingResponse,
        Error,

        /* Operaciones gestion de plugins */
        LoadPlugin,
        CancelPluginLoad,
        InitPlugin,
        StopPlugin,
        UnloadPlugin,
        EnumLoadedPlugins,

        /* Gestion del servidor  */
        UpdateLoader,
        UpdateServer,
        StopServer,
        /*  Reservado */
        Reserved=0x3FFF,
    } Operation;

    /* Tamaño de los datos  */
    struct Size{
        /* Incida si el tamaño se transmite en bytes o en nemero de paquetes*/
        ulong bPaquets:1;
        union{
            ulong Paquets:31;
            ulong Bytes:31;
        };
    } Size;

    /* Tipo de operacion */
    typedef struct OperationType{
        /* Operacion interna / Operacion plugin */
        unsigned short bOperation:1;
        union{
            ushort op:15;//RPEP_HEADER::Operation op
            ushort PluginID:15;
        };
    } OperationType;
    OperationType opType;
    /* Indica el numero de parte si hay mas de una; si se
        usa tamaoo por paquetes, este campo es obligatorio */
    ulong PacketIndex;

    /* Datos */
    char Data[0];
}RPEP_HEADER;

/* Mensaje negociacion cliente  */
typedef struct RPEP_CLIENT_HANDSHAKE{
    struct{
        char Low;
        char High;
    } Version;

    ushort MaxPaquetSize;
    ulong CompressionALGM;
    ulong PortCount;
    ushort Port[0];
} RPEP_CLIENT_HANDSHAKE;

/* Mensaje negociacion servidor  */
typedef struct RPEP_SERVER_HANDSHAKE{
    struct{
        char Low;
        char High;
    } Version;
    ushort MaxPaquetSize;
    ulong SupportedCompressionAlgmCount;
    ulong SupportedCompressionAlgm[0];
}RPEP_SERVER_HANDSHAKE;

/* Mensaje de error*/
typedef struct RPEP_ERROR{
    ushort Level;
    ushort Code;
    ulong SourceID;
    ulong ExtendSize;
    char Extend[0];
}RPEP_ERROR;

/* Mensaje Cargar Plugin*/
typedef struct RPEP_LOAD_PLUGIN{
    ulong PluginID;
    bool ExternalDonwload;
    char PluginName[0];
    char PluginModule[0];
}RPEP_LOAD_PLUGIN;

/* Mensaje descargar plugin */
typedef struct RPEP_UNLOAD_PLUGIN{
    ulong PluginID;
    char PluginName[0];
}RPEP_UNLOAD_PLUGIN;

/*  Mensaje fijar tamaño paquetes */
typedef struct RPEP_SET_PACKAGE_SIZE{
    ulong Value;
}RPEP_SET_PACKAGE_SIZE;

/*  Mensaje fijar algoritmo de compresiñn */
typedef struct RPEP_SET_COMPRESSION_ALGORITHM{
    ulong Value;
}RPEP_SET_COMPRESSION_ALGORITHM;

#endif // RPEP_STRUC_H_INCLUDED
