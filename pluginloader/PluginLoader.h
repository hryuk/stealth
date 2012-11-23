#ifndef PLUGINLOADER_H_INCLUDED
#define PLUGINLOADER_H_INCLUDED

#include "main.h"
#include "../ArkLib/src/datatypes.h"
#include "../ArkLib/src/CCrypt.h"

#include "RPEP.h"

class DArray;


class PluginLoader{
	public:
		PluginLoader();

		bool initPlugin();
		bool stopPlugin();
		bool enumPluginLoad();
		bool updateLoader();
		bool updateServer(DArray& newServer);
		bool stopServer();

		uint run(SOCKET hConexion,HCRYPTKEY hKey,LoaderFunTable& lFunc);
};

extern PluginLoader Loader;

#endif // PLUGINLOADER_H_INCLUDED
