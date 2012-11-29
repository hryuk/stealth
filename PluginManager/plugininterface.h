#ifndef PLUGININTERFACE_H
#define PLUGININTERFACE_H

#include "../ArkLib/src/datatypes.h"

class pluginManagerInterface{
        pluginManagerInterface();
    public:
        virtual int sendData(char* data,uint size) = 0;
};
class PluginInterface{
        PluginInterface();
    public:
        virtual char* getPluginName() = 0;
        virtual int onReciveData(char* data,uint size) = 0;
        virtual void setPluginManager(pluginManagerInterface* mgr) = 0;
};

typedef PluginInterface* (*pgetInterface)();

#endif // PLUGININTERFACE_H
