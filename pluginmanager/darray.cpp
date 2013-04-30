#include "darray.h"

DArray::DArray(){
    //DebufPrintf("DArray ctor\n");
    m_size = m_maxSize = 0;
    m_data = 0;
}

DArray::DArray(void* Data,ulong size){
    m_size = m_maxSize = 0;
    m_data = 0;
    addData(Data,size);
}

DArray::~DArray(){
    if(m_data){
        free(m_data);
    }
}
DArray::DArray(DArray& ArrayCp){
    m_size = m_maxSize = 0;
    m_data = 0;
    *this=ArrayCp;
}
DArray& DArray::operator=(DArray& ArrayCp){
    Resize(ArrayCp.m_maxSize);
    addData(ArrayCp.data(),ArrayCp.size());
    return *this;
}


DArray* DArray::remove(ulong offset,ulong Size){
    if((Size+offset)>this->m_size)
        return 0;
    else if((Size+offset)!=this->m_size){
        memcpy(((byte*)data())+offset,((byte*)data())+(Size+offset),this->m_size-(Size+offset));
    }

    this->m_size-=Size;

    return this;
}

DArray* DArray::addData(ulong offset,char* ToInsert,ulong size){
    char* TMP;
    if(m_maxSize<(this->size()+size)){
        Expand((this->size()+size));
    }
    TMP = (char*)malloc(this->size()-offset);
    memcpy(TMP,((byte*)data())+offset,this->size()-offset);

    memcpy(((byte*)data())+offset,ToInsert,size);
    memcpy(((byte*)data())+offset+size,TMP,this->size()-offset);
    this->m_size+=size;
    free(TMP);

    return this;
}

/*ulong DArray::operator[](ulong index){
    if(index>=Size)return -1;
    return ((ulong*)Data)[index];
};*/

uchar* DArray::Resize(ulong NewSize){
    if(NewSize!=m_maxSize){
    if(NewSize<m_size)m_size = NewSize;
    if(m_maxSize){
        m_data =  (uchar*)realloc(data(),NewSize);
    }else{
        m_data = (uchar*)malloc(NewSize);
    }

    m_maxSize = NewSize;

    }
    return (uchar*)data();
}

DArray* DArray::addData(DArray& ToAdd){
    return addData(ToAdd.data(),ToAdd.size());
}

DArray* DArray::addData(const void* data,ulong Size){
    DArray* NewArray = this;
    if((Size+m_size)>m_maxSize){
        NewArray->m_data = Resize(Size+size());
    };
    memcpy(((byte*)NewArray->data())+NewArray->size(),data,Size);
    NewArray->m_size += Size;
    return NewArray;
}

uchar* DArray::Expand(ulong NewSize){
    NewSize++;
    if(m_maxSize<NewSize){
        Resize(NewSize);
        m_data[NewSize-1]= 0;
    }
    return m_data;
}
