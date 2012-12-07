#ifndef TESTPLUGIN_H
#define TESTPLUGIN_H
#include "../PluginManager/plugininterface.h"

#ifdef BUILD_DLL
    #define DLL_EXPORT __declspec(dllexport)
#else
    #define DLL_EXPORT __declspec(dllimport)
#endif

class TestPlugin : public PluginInterface{
        pluginManagerInterface* pluginMgr;
    public:
        TestPlugin();
        //PluginInterface
        virtual const char *getPluginName();
        virtual int onReciveData(char* data,uint size);
        virtual void setPluginManager(pluginManagerInterface* pluginMgr);
        //TestPlugin
};
extern "C"{
    PluginInterface* DLL_EXPORT getInterface();
}

#endif // TESTPLUGIN_H
