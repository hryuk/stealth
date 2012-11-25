#include "RPEP.h"
struct PKG_DATA{
    typedef enum ClientOpCode:unsigned short{
        //Comandos del protocolo
        clientHello = 1,
        serverHello,
        setSizePKG,
        setCompressAlg,
        setPorts,//Cambia el numero de puerto de una conexion
        loadPlugin,
        cancelLoadPlugin,
        unloadPlugin,
        pingRecuent,
        pingResponse,
        Error,
        reserved = 0x3FFF,
        //Comandos del pluginLoader
        initPlugin,
        stopPlugin,
        enumPluginLoad,
        updateLoader,
        updateServer,
        stopServer
    }ClientOpCode;
    struct Size{
        union{
            ulong NBlokes:31;
            ulong NBytes:31;
        };
        ulong Blokes:1;//Indica si el tamaño esta en blokes o en bytes
    }Size;
    //Identificador de plugin u orden del cliente
    struct ID_Plugin{
        union{
            ushort ID:15;
            ushort CommandClient:15;
        };
        unsigned short IsCommand:1;
    }ID_Plugin;
    //Indica el numero de parte si hay mas de una.
    //Si se usa Size por bloques, este campo tiene que estar
    ulong NParte;
    char data[0];
};
typedef enum:ulong{
    null
}compresAlg;
typedef struct RPEP_ClientHello{
    struct{
        char low;
        char high;
    }version;
    ushort maxSizePKG;
    ulong compressAlg;
    ulong NPortUsed;
    ushort PortNumber[0];
}RPEP_ClientHello;
typedef struct RPEP_ServerHello{
    struct{
        char low;
        char high;
    }version;
    ushort maxSizePKG;
    ulong NSupportCompressAlg;
    ulong supportCompressAlg[0];
}RPEP_ServerHello;
typedef struct RPEP_error{
    ushort level;
    ushort code;
    ulong sourceID;
    ulong ExtendSize;
    char Extend[0];
}RPEP_error;
typedef struct RPEP_loadPlugin{
    ulong pluginID;
    bool externalDonwload;
    char pluginName[0];
    char pluginModule[0];
}RPEP_loadPlugin;
typedef struct RPEP_unloadPlugin{
    ulong pluginID;
    char pluginName[0];
}RPEP_unloadPlugin;
typedef struct RPEP_sizePKG{
    ulong value;
}RPEP_sizePKG;
typedef struct RPEP_CompressAlgID{
    ulong value;
}RPEP_CompressAlgID;


RPEP::RPEP(SOCKET hConexion,HCRYPTKEY hKey){
    MaxPaquetSize = 0;
    conexion.setSocketDescriptor(hConexion,AbtractSocket::SocketState::ConnectedState);
    //ctor
}

RPEP::~RPEP(){
    //dtor
}
ulong RPEP::clientLoop(){
    return 0;
};
uint RPEP::MakePacket(DArray& outBuff,RPEP_HEADER::Operation op,RPEP_HEADER::OperationType opType,DArray* inBuff){
    return MakePacket(outBuff,op,opType,(char*)(inBuff?inBuff->data:0),inBuff?inBuff->size:0);
}
uint RPEP::MakePacket(DArray& outBuff,RPEP_HEADER::Operation op,RPEP_HEADER::OperationType opType,char* inBuff,ulong inSize){
    ulong  result = 0;
    RPEP_HEADER head;
    head.opCode = op;
    head.opType = opType;

    if(this->MaxPaquetSize && inSize>this->MaxPaquetSize){
        //Se manda por partes al superar el maximo de cada paquete
        head.Size.bPaquets = true;
    }else{
        //Si no hay definido tamaño maximo o es menor que el Maximo se manda de una
        head.Size.bPaquets = false;
        head.Size.Bytes = inSize;

        //Añado el header
        outBuff.addData(&head,sizeof(head));
        //Añado los datos
        outBuff.addData(inBuff,inSize);
        result++;
    }

    return result;
}

uint RPEP::MakeServerHello(DArray& outBuff,ushort ver,ulong MaxPaquetSize,ulong CompresAlgCount,ulong* CompresAlg){
    //Buffert en pila para el hello
    char buff[sizeof(RPEP_SERVER_HANDSHAKE)+CompresAlgCount*sizeof(RPEP_SERVER_HANDSHAKE::SupportedCompressionAlgm)];
    register RPEP_SERVER_HANDSHAKE* sHello = (RPEP_SERVER_HANDSHAKE*)buff;

    *((ushort*)&sHello->Version) = ver;
    sHello->MaxPaquetSize = MaxPaquetSize;
    sHello->SupportedCompressionAlgmCount = CompresAlgCount;
    if(CompresAlgCount)memcpy(sHello->SupportedCompressionAlgm,CompresAlg,CompresAlgCount*sizeof(RPEP_SERVER_HANDSHAKE::SupportedCompressionAlgm));

    //MakePacket(outBuff,RPEP_HEADER::Operation::ServerHandshake,0,(char*)buff,sizeof(RPEP_SERVER_HANDSHAKE)+CompresAlgCount*sizeof(RPEP_SERVER_HANDSHAKE::SupportedCompressionAlgm));


}
