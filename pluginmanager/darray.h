#ifndef DARRAY_H
#define DARRAY_H

#undef __DECLSPEC_SUPPORTED
#include "basicTypes.h"
#include "rtl.h"

class  DArray{
    protected:
        ulong m_maxSize;
        ulong m_size;
        byte* m_data;
    public:
        DArray();
        DArray(void* Data,ulong size);
        ~DArray();
        DArray(DArray& ArrayCp);
        virtual DArray& operator=(DArray& ArrayCp);

        virtual uchar* Expand(ulong NewSize);
        virtual uchar* Resize(ulong NewSize);
        void* data(){
            return m_data;
        }
        ulong size()const{
            return m_size;
        }
        ulong& rSize(){
            return m_size;
        }

        ulong maxSize()const{
            return m_maxSize;
        }
        void clear(){
            m_size = 0;
            if(m_data)*m_data = 0;
        }
        void zeroFill(){
            memset(m_data,0,m_maxSize-m_size);
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
        virtual DArray* remove(ulong offset,ulong size);
        virtual DArray* addData(ulong offset,char* ToInsert,ulong size);
        virtual DArray* addData(ulong offset,DArray& ToInsert){
            return addData(offset,(char*)ToInsert.data(),ToInsert.size());
        }
};
#endif // DARRAY_H
