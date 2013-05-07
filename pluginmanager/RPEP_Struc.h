#ifndef RPEP_STRUC_H_INCLUDED
#define RPEP_STRUC_H_INCLUDED

//#pragma warning(push)
#pragma warning(disable: 4200)
#pragma warning(disable: 4482)

typedef struct version{
    char Low;
    char High;
    version(char Low,char High){
        this->Low = Low;
        this->High = High;
    }
} version;

typedef struct _RPEP_HEADER
    {
        /* Codigo de operación */
        typedef enum _Operation
        {
            /* Operaciones sobre la negociacion */
            ClientHandshake = 1,
            ServerHandshake,
            SetBlockSize,
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

            /* Gestion del servidor */
            UpdateLoader,
            UpdateServer,
            StopServer,

            /* Reservado */
            Reserved=0x3FFF
        } Operation;

        /* Tipo de operación */
        typedef struct OperationType
        {
            /* Operación interna / Operación plugin */
            unsigned short bOperation:1;
            union
            {
                ushort Operation:15;
                ushort PluginID:15;
            };
        } OperationType;
        OperationType opType;

        /* Tamaño de los datos */
        struct Size
        {
            /* Incida si el tamaño se transmite en bytes o en número de bloques */
            ulong bBlocks:1;
            union
            {
                ulong Blocks:31;
                ulong Bytes:31;
            };
        } Size;

        /* Indica el numero de parte si hay mas de una; si se
usa tamaño por bloques, este campo es obligatorio */
        ulong BlockIndex;

        /* Datos */
        char Data[];
    } RPEP_HEADER;

    /* Mensaje negociación cliente */
    typedef struct _RPEP_CLIENT_HANDSHAKE
    {
        version Version;

        ushort MaxBlockSize;
        ulong CompressionALGM;
        ulong PortCount;
        ushort Port[];
    } RPEP_CLIENT_HANDSHAKE;

    /* Mensaje negociación servidor */
    typedef struct _RPEP_SERVER_HANDSHAKE
    {
        version Version;

        ushort MaxBlockSize;
        ulong SupportedCompressionAlgmCount;
        ulong SupportedCompressionAlgm[];
    } RPEP_SERVER_HANDSHAKE;

    /* Mensaje de error*/
    typedef struct _RPEP_ERROR
    {
        ushort Level;
        ushort Code;
        ulong SourceID;
        ulong ExtendSize;
        char Extend[];
    } RPEP_ERROR;

    /* Mensaje Cargar Plugin*/
    typedef struct _RPEP_LOAD_PLUGIN
    {
        ulong PluginID;
        char PluginModule[];
    } RPEP_LOAD_PLUGIN;

    /* Mensaje descargar plugin */
    typedef struct _RPEP_UNLOAD_PLUGIN
    {
        ulong PluginID;
        char PluginName[];
    } RPEP_UNLOAD_PLUGIN;

    /* Mensaje fijar tamaño bloques */
    typedef struct _RPEP_SET_BLOCK_SIZE
    {
        ulong Value;
    } RPEP_SET_BLOCK_SIZE;

    /* Mensaje fijar algoritmo de compresión */
    typedef struct _RPEP_SET_COMPRESSION_ALGORITHM
    {
        ulong Value;
    } RPEP_SET_COMPRESSION_ALGORITHM;

//#pragma warning(pop)
#endif // RPEP_STRUC_H_INCLUDED
