#ifndef RPEP_H
#define RPEP_H
//#include "../ArkLib/src/Network/AbtractSocket.h"
#include "../ArkLib/src/Network/TCPSocket.h"
#include "../ArkLib/src/CCrypt.h"
#include "RPEP_Struc.h"


class RPEP{
        TCPSocket conexion;
        ushort MaxPaquetSize;

        uint MakePacket(DArray& outBuff,RPEP_HEADER::Operation op,DArray* inBuff = 0);
        uint MakePacket(DArray& outBuff,RPEP_HEADER::Operation op,char* inBuff = 0,ulong inSize = 0);
        uint MakePacket(DArray& outBuff,RPEP_HEADER::Operation op,RPEP_HEADER::OperationType opType,DArray* inBuff);
        uint MakePacket(DArray& outBuff,ushort op,DArray* inBuff = 0);
        uint MakePacket(DArray& outBuff,RPEP_HEADER::Operation op,RPEP_HEADER::OperationType opType,char* inBuff,ulong inSize);
        uint MakePacket(DArray& outBuff,ushort op,char* inBuff = 0,ulong inSize = 0);
        uint MakeServerHello(DArray& outBuff,ushort ver,ulong MaxPaquetSize,ulong CompresAlgCount = 0,ulong* CompresAlg = 0);
        uint MakeClienHello(DArray& outBuff,ushort ver,ulong MaxPaquetSize,ulong CompresAlg,ulong PortCount = 1, ushort* Port = 0);
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
