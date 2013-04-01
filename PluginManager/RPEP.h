#ifndef RPEP_H
#define RPEP_H

#include <windows.h>
#include <wincrypt.h>

#include "basicTypes.h"
#include "RPEP_Struc.h"

typedef UINT_PTR        SOCKET;

class DArray;
class PluginManager;
class RPEP{
        friend class PluginManagerInterfacePrivate;
        typedef enum stateType{
            noConfigured,
            ready
        }stateType;
        //Indica el estado del protocolo
        stateType state;
        //Socket de la conexion
        SOCKET hConexion;
        //Clave de cifrado
        HCRYPTKEY hKey;
        //Parametros configurables del protocolo
        ushort MaxPaquetSize;
        version ver;
        ulong CompresAlg;
        ulong PortCount;
        ushort* Port;

        bool runing;

        PluginManager* PlugMgr;

        //set and get
        void setMaxPaquetSize(ulong s);
        void setCompresAlg(ulong CompresAlg);
        void setPort(ushort* Port,ulong count);

        bool encript(DArray &data);
        uint decript(byte *data, ulong size);


        //Generan un paquete
        uint MakePacket(DArray& outBuff,RPEP_HEADER::Operation op,const void* data,ulong size);
        uint MakePacket(DArray& outBuff,ushort pluginID,const void* data,ulong size);
        uint MakePacket(DArray& outBuff,bool IsOperation,ushort opOrIDCode,const void* data,ulong size);

        //Generan paquetes determinados
        uint MakeServerHello(DArray& outBuff);
        uint MakeClienHello(DArray& outBuff,ushort ver,ulong MaxPaquetSize,ulong CompresAlg,ulong PortCount = 1, ushort* Port = 0);
        uint MakeError(DArray &outBuff, uint code);

        bool procesPkg(DArray& in, DArray& out, DArray& workBuff);
        bool procesCMD(RPEP_HEADER::OperationType opType, char* data, uint size, DArray &response);

        bool processClientHello(RPEP_CLIENT_HANDSHAKE* clientHello, DArray &response);

    public:
        /** Default constructor */
        RPEP(SOCKET hConexion,HCRYPTKEY hKey,PluginManager* PlugMgr);
        /** Default destructor */
        virtual ~RPEP();
        ulong clientLoop();
        ulong serverLoop();

        //Manda los datos al otro extremo
        int send(const void *data, uint size);
        int recv(char *data, uint size);
    protected:
    private:
};

#endif // RPEP_H
