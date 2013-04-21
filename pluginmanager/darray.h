#ifndef DARRAY_H
#define DARRAY_H

#undef __DECLSPEC_SUPPORTED
#include "basicTypes.h"
#include "rtl.h"

class  DArray{
    protected:
    ulong MaxSize;
    public:

    DArray();
    DArray(void* Data,ulong size);
    ~DArray();
    DArray(DArray& ArrayCp);
    virtual DArray& operator=(DArray& ArrayCp);

    ulong size;
    union{
        uchar* data;
        char* cadena;
        long* numeros;
        void* puntero;
    };
    virtual uchar* Expand(ulong NewSize);
    virtual uchar* Resize(ulong NewSize);
    ulong GetMaxSize(){
        return MaxSize;
    };
    void Vaciar(){
        size = 0;
        if(data)*data = 0;
    };
    void FillZero(){
        memset(data,0,MaxSize-size);
    }
    virtual DArray* addData(DArray& ToAdd);
    virtual DArray* addData(const void* data,ulong size);
    virtual DArray* addData(const char data){
        return addData(&data,sizeof(data));
    }
    virtual DArray* addData(const ushort data){
        return addData(&data,sizeof(data));
    }
    virtual DArray* addData(const int data){
        return addData(&data,sizeof(data));
    }
    virtual DArray* DelData(ulong offset,ulong size);
    virtual DArray* Insert(ulong offset,char* ToInsert,ulong size);
    virtual DArray* Insert(ulong offset,DArray& ToInsert){
        return Insert(offset,ToInsert.cadena,ToInsert.size);
    }
};
#endif // DARRAY_H
