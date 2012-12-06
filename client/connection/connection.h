#ifndef CONNECTION_H
#define CONNECTION_H

#include <QTcpSocket>
#include <QTimer>

#include "crypto.h"

class Connection : public QTcpSocket
{
    Q_OBJECT

public:
    enum State {JustConnected,WaitingForLoader,WaitingForGreeting,ReadingGreeting,WaitingGreetingOk,ReadingGreetingOk,Ready,Sending,Receiving};

    typedef struct _RPEP_HEADER
    {
        /* Codigo de operación */
        typedef enum _Operation
        {
            /*  Operaciones sobre la negociacion */
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

            /* Gestion del servidor  */
            UpdateLoader,
            UpdateServer,
            StopServer,

            /*  Reservado */
            Reserved=0x3FFF
        } Operation;

        /* Tipo de operación */
        struct _OperationType
        {
            /* Operación interna / Operación plugin */
            quint16 bOperation:1;
            union
            {
                quint16 Operation:15;
                quint16 PluginID:15;
            };
        } OperationType;


        /* Tamaño de los datos  */
        struct _Size
        {
            /* Incida si el tamaño se transmite en bytes o en número de bloques */
            quint32 bBlocks:1;
            union
            {
                quint32 Blocks:31;
                quint32 Bytes:31;
            };
        } Size;

        /* Indica el numero de parte si hay mas de una; si se
           usa tamaño por bloques, este campo es obligatorio */
        quint32 BlockIndex;

        /* Datos */
        char Data[];
    } RPEP_HEADER;

    /* Mensaje negociación cliente  */
    typedef struct _RPEP_CLIENT_HANDSHAKE
    {
        struct
        {
            char Low;
            char High;
        } Version;

        quint16 MaxBlockSize;
        quint32 CompressionALGM;
        quint32 PortCount;
        quint16 Port[];
    } RPEP_CLIENT_HANDSHAKE;

    /* Mensaje negociación servidor  */
    typedef struct _RPEP_SERVER_HANDSHAKE
    {
        struct
        {
            char Low;
            char High;
        } Version;

        quint16 MaxBlockSize;
        quint32 SupportedCompressionAlgmCount;
        quint32 SupportedCompressionAlgm[];
    } RPEP_SERVER_HANDSHAKE;

    /* Mensaje de error*/
    typedef struct _RPEP_ERROR
    {
        quint16 Level;
        quint16 Code;
        quint32 SourceID;
        quint32 ExtendSize;
        char Extend[];
    } RPEP_ERROR;

    /* Mensaje Cargar Plugin*/
    typedef struct _RPEP_LOAD_PLUGIN
    {
        quint32 PluginID;
        bool ExternalDonwload;
        char PluginName[];
        char PluginModule[];
    } RPEP_LOAD_PLUGIN;

    /* Mensaje descargar plugin */
    typedef struct _RPEP_UNLOAD_PLUGIN
    {
        quint32 PluginID;
        char PluginName[];
    } RPEP_UNLOAD_PLUGIN;

    /*  Mensaje fijar tamaño bloques */
    typedef struct _RPEP_SET_BLOCK_SIZE
    {
        quint32 Value;
    } RPEP_SET_BLOCK_SIZE;

    /*  Mensaje fijar algoritmo de compresión */
    typedef struct _RPEP_SET_COMPRESSION_ALGORITHM
    {
        quint32 Value;
    } RPEP_SET_COMPRESSION_ALGORITHM;

    Connection();
    ~Connection();
    void setState(State state);
    void setKey(QString strKey);
    void setBlockSize(ulong BlockSize);
    State getState();
    QString getKey();
    QByteArray getIV();
    ulong getBlockSize();

    Connection::RPEP_HEADER NextBlockHeader;
    Connection::RPEP_SERVER_HANDSHAKE HandShake;
    QByteArray Data;

private:
    /* Estado previo usado para el timeout */
    State previousState;
    State state;
    QCA::SymmetricKey key;
    QString strKey;
    ulong BlockSize;
    QCA::Cipher* cipher;
    QCA::Initializer init;
    QCA::InitializationVector* iv;
    QTimer timer;

private slots:
    QByteArray addPadding(QByteArray data);
    void checkTimeout();

public slots:
    int send(RPEP_HEADER::_OperationType* operation,char* data,int size);
    QByteArray crypt(QByteArray data,bool padding=true);
    QByteArray decrypt(QByteArray data);

signals:
    void timeout();
};

#endif // CONNECTION_H
