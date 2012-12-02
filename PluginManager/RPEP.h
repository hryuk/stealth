#ifndef RPEP_H
#define RPEP_H
//#include "../ArkLib/src/Network/AbtractSocket.h"
#include "../ArkLib/src/Network/TCPSocket.h"
#include "../ArkLib/src/CCrypt.h"
#include "RPEP_Struc.h"


class RPEP{
        typedef enum stateType{
            noConfigured,
            ready
        }stateType;
        //Indica el estado del protocolo
        stateType state;
        //Socket de la conexion
        TCPSocket conexion;
        SOCKET hConexion;
        //Clave de cifrado
        HCRYPTKEY hKey;
        //Parametros configurables del protocolo
        ushort MaxPaquetSize;
        ushort ver;
        ulong CompresAlg;
        ulong PortCount;
        ushort* Port;

        bool runing;

        //set and get
        void setMaxPaquetSize(ulong s);
        void setCompresAlg(ulong CompresAlg);
        void setPort(ushort* Port,ulong count);

        bool encript(DArray &data);
        bool decript(DArray &data);

        //Generan un paquete
        uint MakePacket(DArray& outBuff,RPEP_HEADER::Operation op,const void* data,ulong size);
        uint MakePacket(DArray& outBuff,ushort pluginID,const void* data,ulong size);
        uint MakePacket(DArray& outBuff,bool IsOperation,ushort opOrIDCode,const void* data,ulong size);

        //Generan paquetes determinados
        uint MakeServerHello(DArray& outBuff);
        uint MakeClienHello(DArray& outBuff,ushort ver,ulong MaxPaquetSize,ulong CompresAlg,ulong PortCount = 1, ushort* Port = 0);
        uint MakeError(DArray &outBuff, uint code);

        bool procesPkg(DArray& in,DArray& out);
        bool procesCMD(RPEP_HEADER::OperationType opType, char* data, uint size, DArray &response);

        bool processClientHello(RPEP_CLIENT_HANDSHAKE* clientHello, DArray &response);

    public:
        /** Default constructor */
        RPEP(SOCKET hConexion,HCRYPTKEY hKey);
        /** Default destructor */
        virtual ~RPEP();
        ulong clientLoop();
        ulong serverLoop();
    protected:
    private:
};

#endif // RPEP_H
