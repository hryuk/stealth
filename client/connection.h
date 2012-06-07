#ifndef CONNECTION_H
#define CONNECTION_H

#include <QTcpSocket>

class Connection : public QTcpSocket
{
    Q_OBJECT

public:
    enum State {JustConnected,Handshake,Ready,Sending,Reading};

    typedef struct _RPEP_HEADER
        {
            /* Codigo de operación */
            typedef enum _Operation
            {
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
                Reserved=0x3FFF
            } Operation;

            /* Tamaño de los datos  */
            struct _Size
            {
                /* Incida si el tamaño se transmite en bytes o en número de paquetes*/
                ulong bPaquets:1;
                union
                {
                    ulong Paquets:31;
                    ulong Bytes:31;
                };
            } Size;

            /* Tipo de operación */
            struct _OperationType
            {
                /* Operación interna / Operación plugin */
                unsigned short bOperation:1;
                union
                {
                    ushort Operation:15;
                    ushort PluginID:15;
                };
            } OperationType;

            /* Indica el numero de parte si hay mas de una; si se
               usa tamaño por paquetes, este campo es obligatorio */
            ulong PacketIndex;

            /* Datos */
            char Data[0];
        } RPEP_HEADER;

        /* Mensaje negociación cliente  */
        typedef struct _RPEP_CLIENT_HANDSHAKE
        {
            struct
            {
                char Low;
                char High;
            } Version;

            ushort MaxPaquetSize;
            ulong CompressionALGM;
            ulong PortCount;
            ushort Port[0];
        } RPEP_CLIENT_HANDSHAKE;

        /* Mensaje negociación servidor  */
        typedef struct _RPEP_SERVER_HANDSHAKE
        {
            struct
            {
                char Low;
                char High;
            } Version;

            ushort MaxPaquetSize;
            ulong SupportedCompressionAlgmCount;
            ulong SupportedCompressionAlgm[0];
        } RPEP_SERVER_HANDSHAKE;

        /* Mensaje de error*/
        typedef struct _RPEP_ERROR
        {
            ushort Level;
            ushort Code;
            ulong SourceID;
            ulong ExtendSize;
            char Extend[0];
        } RPEP_ERROR;

        /* Mensaje Cargar Plugin*/
        typedef struct _RPEP_LOAD_PLUGIN
        {
            ulong PluginID;
            bool ExternalDonwload;
            char PluginName[0];
            char PluginModule[0];
        } RPEP_LOAD_PLUGIN;

        /* Mensaje descargar plugin */
        typedef struct _RPEP_UNLOAD_PLUGIN
        {
            ulong PluginID;
            char PluginName[0];
        } RPEP_UNLOAD_PLUGIN;

        /*  Mensaje fijar tamaño paquetes */
        typedef struct _RPEP_SET_PACKAGE_SIZE
        {
            ulong Value;
        } RPEP_SET_PACKAGE_SIZE;

        /*  Mensaje fijar algoritmo de compresión */
        typedef struct _RPEP_SET_COMPRESSION_ALGORITHM
        {
            ulong Value;
        } RPEP_SET_COMPRESSION_ALGORITHM;

    Connection();
    State getState();
    void setState(State state);
    void setIV(QByteArray IV);
    QByteArray getIV();

private:
    State state;
    QByteArray IV;

};

#endif // CONNECTION_H
