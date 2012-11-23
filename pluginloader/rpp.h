#ifndef RPP_H_INCLUDED
#define RPP_H_INCLUDED

#include "../ArkLib/src/datatypes.h"
//#include "../ArkLib/src/Network/NetClient.h"
#include "../ArkLib/src/CCrypt.h"

struct PKG_DATA;
typedef enum:ulong{
	null
}compresAlg;
typedef struct rpp_ClientHello{
	struct{
		char low;
		char high;
	}version;
	ushort maxSizePKG;
	ulong compressAlg;
	ulong NPortUsed;
	ushort PortNumber[0];
}rpp_ClientHello;
typedef struct rpp_ServerHello{
	struct{
		char low;
		char high;
	}version;
	ushort maxSizePKG;
	ulong NSupportCompressAlg;
	ulong supportCompressAlg[0];
}rpp_ServerHello;
typedef struct rpp_error{
	ushort level;
	ushort code;
	ulong sourceID;
	ulong ExtendSize;
	char Extend[0];
}rpp_error;
typedef struct rpp_loadPlugin{
	ulong pluginID;
	bool externalDonwload;
	char pluginName[0];
	char pluginModule[0];
}rpp_loadPlugin;
typedef struct rpp_unloadPlugin{
	ulong pluginID;
	char pluginName[0];
}rpp_unloadPlugin;
typedef struct rpp_sizePKG{
	ulong value;
}rpp_sizePKG;
typedef struct rpp_CompressAlgID{
	ulong value;
}rpp_CompressAlgID;
/*
class rpp:public NetClient{
	private:
		ulong initState;
		ulong compressAlg;
		ushort maxSizePKG;
		DArray ReAsembleBuff;
		DArray errorBuff;

		bool MKError(ushort level,ushort code,ulong source,DArray* Extra = 0);
		bool MKServerHello(DArray& buff);
		bool ProcessClientHello(rpp_ClientHello* Hello);
		bool setSizePKG(ulong size);
		ulong getSizePKG();
		bool setCompressAlg(ulong compressID);
		bool setPorts(ulong PortIndex,ushort PortNumber);
		bool loadPlugin(rpp_loadPlugin* toLoad,ulong size);
		bool unloadPlugin(rpp_unloadPlugin* toUnload,ulong size);
		bool cancelLoadPlugin();
		bool pingRecuent();
		bool pingResponse();

		ulong ProcessCMD(PKG_DATA* pkg,DArray& response);
		ulong ProcessPKG(DArray& InBuff,DArray& OutBuff);
		ulong InicialiceProto(DArray& OutBuff);
	public:
		rpp(SOCKET hConexion,HCRYPTKEY hKey);
		virtual ~rpp();

		virtual void OnRecive(NetData* data);
		virtual void OnDisconected();
		virtual void OnConected();
};*/

#endif // RPP_H_INCLUDED
