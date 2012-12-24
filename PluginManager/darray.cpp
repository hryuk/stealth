#include "darray.h"
#include "stdlib.h"

DArray::DArray(){
    printf("DArray ctor\n");
    size = MaxSize = 0;
    data = 0;
}
DArray::DArray(void* Data,ulong size){
    this->size = MaxSize = 0;
    data = 0;
    addData(Data,size);
}
DArray::~DArray(){
    if(data){
        free(data);
    }
}
DArray& DArray::operator=(DArray& ArrayCp){
    Resize(ArrayCp.MaxSize);
    addData(ArrayCp.data,ArrayCp.size);
    return *this;
}
DArray::DArray(DArray& ArrayCp){
    size = MaxSize = 0;
    data = 0;
    *this=ArrayCp;
}
DArray* DArray::DelData(ulong offset,ulong Size){
    if((Size+offset)>this->size)
        return 0;
    else if((Size+offset)!=this->size)
        memcpy(data+offset,data+(Size+offset),this->size-(Size+offset));

    this->size-=Size;

    return this;
};
DArray* DArray::Insert(ulong offset,char* ToInsert,ulong size){
    char* TMP;
    if(MaxSize<(this->size+size)){
        Expand((this->size+size));
    }
    TMP = (char*)malloc(this->size-offset);
    memcpy(TMP,cadena+offset,this->size-offset);

    memcpy(cadena+offset,ToInsert,size);
    memcpy(cadena+offset+size,TMP,this->size-offset);
    this->size+=size;
    free(TMP);

    return this;
}
/*
ulong DArray::operator[](ulong index){
    if(index>=Size)return -1;
    return ((ulong*)Data)[index];
};*/
uchar* DArray::Resize(ulong NewSize){
    if(NewSize!=MaxSize){
    if(NewSize<size)size = NewSize;
    if(MaxSize){
        data =  (uchar*)realloc(this->data,NewSize);
    }else{
        data = (uchar*)malloc(NewSize);
    }

    MaxSize = NewSize;

    }
    return data;
}
DArray* DArray::addData(DArray& ToAdd){
    return addData(ToAdd.cadena,ToAdd.size);
}
DArray* DArray::addData(const void* data,ulong Size){
    DArray* NewArray = this;
    if((Size+size)>MaxSize){
        NewArray->data = Resize(Size+size);
    };
    memcpy(NewArray->data+NewArray->size,data,Size);
    NewArray->size += Size;
    return NewArray;
}
uchar* DArray::Expand(ulong NewSize){
    NewSize++;
    if(MaxSize<NewSize){
        Resize(NewSize);
        cadena[NewSize-1]= 0;
    }
    return data;
}
