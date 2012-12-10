

#undef UNICODE
#include "testplugin.h"
#include <windows.h>


TestPlugin::TestPlugin(){
    pluginMgr = 0;
}

const char *TestPlugin::getPluginName(){
    return "TestPlugin";
}

int TestPlugin::onReciveData(char *data, uint size){
    bool result = false;
    if(pluginMgr){
        int MsgResult;

        MsgResult = MessageBox(0,"TestPlugin",data,MB_YESNO);

        pluginMgr->sendData((char*)&MsgResult,sizeof(MsgResult));
        result = true;
    }
    return result;
}
void TestPlugin::setPluginManager(pluginManagerInterface *pluginMgr){
    this->pluginMgr = pluginMgr;
}
PluginInterface* getInterface(){
    return new TestPlugin();
}
