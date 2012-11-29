#include "RPEP.h"
#include "PluginManager.h"
#include "plugininterface.h"

RPEP::RPEP(SOCKET hConexion,HCRYPTKEY hKey){
    Port = NULL;
    CompresAlg = NULL;
    PortCount = CompresAlgCount = ver = MaxPaquetSize = 0;
    runing = true;
    this->ver = 0;
    this->hKey = hKey;
    conexion.setSocketDescriptor(hConexion,AbtractSocket::SocketState::ConnectedState);

}
RPEP::~RPEP(){}

ulong RPEP::clientLoop(){
    return 0;
};
ulong RPEP::serverLoop(){
    DArray buff;

    MakeServerHello(buff);
    conexion.write(buff);
    do{
        buff.Vaciar();
        conexion.read(buff);
    }while(runing);

    return 0;
}

uint RPEP::MakePacket(DArray &outBuff, RPEP_HEADER::Operation op, const void *data, ulong size){
    return MakePacket(outBuff,true,op,data,size);
}
uint RPEP::MakePacket(DArray &outBuff, ushort pluginID, const void *data, ulong size){
    return MakePacket(outBuff,false,pluginID,data,size);
}
uint RPEP::MakePacket(DArray &outBuff, bool IsOperation, ushort opOrIDCode, const void *data, ulong size){
    RPEP_HEADER header;

    //Aqui aparece el tipo de codificacion usada para el tamaño
    header.Size.bPaquets = size>MaxPaquetSize;
    //Indico el tipo de operacion
    header.opType.bOperation = IsOperation;
    header.opType.op = opOrIDCode;
    header.PacketIndex = 0;

    //Segun el tipo de tamaño usado se agrega de una u otra manera
    if(header.Size.bPaquets){
        header.Size.Paquets = size/MaxPaquetSize;
        //En caso de no caver en un solo paquete, lo voy dividiendo en trozos el contenido
        for(;header.PacketIndex<header.Size.Paquets;header.PacketIndex++){
            //Agrego la cabecera
            outBuff.addData(&header,sizeof(header));
            //Agrego los datos frgmentados
            outBuff.addData(((char*)data)+header.PacketIndex*MaxPaquetSize,
                            ((((header.PacketIndex+1)*MaxPaquetSize)<=size)?MaxPaquetSize:size%MaxPaquetSize));
        }

    }else{
        header.Size.Bytes = size;
        outBuff.addData(&header,sizeof(header));
        outBuff.addData(data,size);
    }
    return 0;
}


uint RPEP::MakeServerHello(DArray& outBuff){
    //Buffert en pila para el hello
    uint buffSize = sizeof(RPEP_SERVER_HANDSHAKE)+CompresAlgCount*sizeof(RPEP_SERVER_HANDSHAKE::SupportedCompressionAlgm);
    char buff[buffSize];
    register RPEP_SERVER_HANDSHAKE* sHello = (RPEP_SERVER_HANDSHAKE*)buff;

    *((ushort*)&sHello->Version) = ver;
    sHello->MaxPaquetSize = MaxPaquetSize;
    sHello->SupportedCompressionAlgmCount = CompresAlgCount;
    if(CompresAlgCount)memcpy(sHello->SupportedCompressionAlgm,CompresAlg,CompresAlgCount*sizeof(RPEP_SERVER_HANDSHAKE::SupportedCompressionAlgm));

    MakePacket(outBuff,RPEP_HEADER::Operation::ServerHandshake,buff,buffSize);

    return 0;
}
uint RPEP::MakeError(DArray& outBuff,uint code){
    RPEP_ERROR error;

    error.Code = code;
    return MakePacket(outBuff,RPEP_HEADER::Operation::Error,&error,sizeof(error));
}

bool RPEP::procesPkg(DArray& in,DArray& out){

}
bool RPEP::procesCMD(RPEP_HEADER::OperationType opType, char* data,uint size){
    bool result = false;

    if(opType.bOperation){
        switch(opType.op){
            case RPEP_HEADER::Operation::ClientHandshake:
                processClientHello((RPEP_CLIENT_HANDSHAKE*)data);
                break;
            case RPEP_HEADER::Operation::ServerHandshake:
                break;
            case RPEP_HEADER::Operation::SetPacketSize:
                break;
            case RPEP_HEADER::Operation::SetCompressionAlgm:
                break;
            case RPEP_HEADER::Operation::PingRequest:
                break;
            case RPEP_HEADER::Operation::PingResponse:
                break;
            case RPEP_HEADER::Operation::Error:
                break;
            //
            case RPEP_HEADER::Operation::LoadPlugin:
               result = PlugMgr.loadtPlugin(data);
               break;
            case RPEP_HEADER::Operation::CancelPluginLoad:
               break;
            case RPEP_HEADER::Operation::InitPlugin:
               break;
            case RPEP_HEADER::Operation::StopPlugin:
               break;
            case RPEP_HEADER::Operation::UnloadPlugin:
               break;
            case RPEP_HEADER::Operation::EnumLoadedPlugins:
               break;
            //
            case RPEP_HEADER::Operation::UpdateLoader:
               break;
            case RPEP_HEADER::Operation::UpdateServer:
               break;
            case RPEP_HEADER::Operation::StopServer:
               break;
            default:
               //Desconocido
               ;
        }
    }else{
        plugin* currentPlugin;
        if((currentPlugin = PlugMgr.getPluginById(opType.PluginID))){
            currentPlugin->interdace->onReciveData(data,size);
        }
    }
    return result;
}

bool RPEP::processClientHello(RPEP_CLIENT_HANDSHAKE *clientHello){
}
