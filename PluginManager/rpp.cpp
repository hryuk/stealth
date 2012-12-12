#include "rpp.h"
//#include "../ArkLib/src/Network/TCP.h"
/**
*
*	Algoripmos de compresion fastlz
*	http://code.google.com/p/fastlz/source/checkout
*
**/

compresAlg compressAlgSupport[] = {compresAlg::null};

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
};/*
ulong rpp::ProcessCMD(PKG_DATA* pkg,DArray& Response){
	if(pkg->ID_Plugin.IsCommand){
		Response.Vaciar();
		//Proceso el comando
		//Estos son los del protocolo
		if(pkg->ID_Plugin.CommandClient<PKG_DATA::ClientOpCode::reserved){
			switch(pkg->ID_Plugin.CommandClient){
				case PKG_DATA::ClientOpCode::clientHello:
					ProcessClientHello((rpp_ClientHello*)pkg->data);
					Response.addData(errorBuff);
					break;
				case PKG_DATA::ClientOpCode::setSizePKG:
					break;
				case PKG_DATA::ClientOpCode::setCompressAlg:
					break;
				case PKG_DATA::ClientOpCode::setPorts:
					break;
				case PKG_DATA::ClientOpCode::loadPlugin:
					break;
				case PKG_DATA::ClientOpCode::cancelLoadPlugin:
					break;
				case PKG_DATA::ClientOpCode::unloadPlugin:
					break;
				case PKG_DATA::ClientOpCode::pingRecuent:
					break;
				case PKG_DATA::ClientOpCode::pingResponse:
					break;
				case PKG_DATA::ClientOpCode::Error:
					break;

				default:
					break;
			}
		}else{
			//Estos son los comandos del PluginLoader
			switch(pkg->ID_Plugin.CommandClient){
				case PKG_DATA::ClientOpCode::initPlugin:
					break;
				case PKG_DATA::ClientOpCode::stopPlugin:
					break;
				case PKG_DATA::ClientOpCode::enumPluginLoad:
					break;
				case PKG_DATA::ClientOpCode::updateLoader:
					break;
				case PKG_DATA::ClientOpCode::updateServer:
					break;
				case PKG_DATA::ClientOpCode::stopServer:
					break;
				default:
					break;
			}
		}
	}else{
		//Se lo paso a los plugin
	}

}
rpp::rpp(SOCKET hConexion,HCRYPTKEY hKey){
	maxSizePKG = -1;
	compressAlg = 0;
	initState = 0;
	protocolo = new TCP(this);
	((TCP*)protocolo)->setSocket(hConexion);
}
rpp::~rpp(){
}
bool rpp::ProcessClientHello(rpp_ClientHello* Hello){
	if(!setSizePKG(Hello->maxSizePKG))return false;
	if(!setCompressAlg(Hello->compressAlg))return false;
	//Seteo los puertos adicionales a los que conectara
	for(int i = 1;i<Hello->NPortUsed;i++){
		if(!setPorts(i,Hello->PortNumber[i]))return false;
	}

	return true;
}
bool rpp::MKServerHello(DArray& buff){
	buff.Expand(sizeof(rpp_ServerHello)+sizeof(compressAlgSupport));
	rpp_ServerHello& Hello = *((rpp_ServerHello*)buff.puntero);

	Hello.maxSizePKG = -1;
	Hello.version.low = 1;
	Hello.version.high = 0;
	Hello.NSupportCompressAlg = sizeof(compressAlgSupport)/sizeof(compresAlg);
	memcpy(Hello.supportCompressAlg,compressAlgSupport,sizeof(compressAlgSupport));

	return true;
}
ulong rpp::InicialiceProto(DArray& OutBuff){

}
ulong rpp::ProcessPKG(DArray& InBuff,DArray& OutBuff){
	ulong PKGSize = 0;

	PKG_DATA* pkg = (PKG_DATA*)InBuff.data;

	if(pkg->Size.Blokes){
		if(pkg->Size.NBlokes==1){
			ProcessCMD(pkg,OutBuff);
		}else{
			if(ReAsembleBuff.size){
				ReAsembleBuff.addData(pkg,sizeof(PKG_DATA));
			}
			ReAsembleBuff.addData(pkg->data,maxSizePKG);
			if(pkg->Size.NBlokes==(pkg->NParte+1)){
				pkg = (PKG_DATA*)ReAsembleBuff.data;
				ProcessCMD(pkg,OutBuff);
			}
		}
	}else{
		ProcessCMD(pkg,OutBuff);
	}
}
bool rpp::setSizePKG(ulong size){
	this->maxSizePKG = size;
	return true;
};
ulong rpp::getSizePKG(){
	return maxSizePKG;
}
bool rpp::setCompressAlg(ulong compressID){
}
bool rpp::setPorts(ulong PortIndex,ushort PortNumber){
}

void rpp::OnRecive(NetData* data){
	DArray	OutBuff;
	DArray&	InBuff = *(data->Array);
	ProcessPKG(InBuff,OutBuff);
}
void rpp::OnDisconected(){
}
void rpp::OnConected(){
}
*/
