#ifndef ARRAYLIST_H
#define ARRAYLIST_H

#include "basicTypes.h"
#include <stdio.h>
#include <stdlib.h>

template <class T> class ArrayList{
        T* m_data;
        ulong m_size;
        ulong m_capacity;
    public:
        ArrayList();
        ArrayList(const ArrayList& toCopy);
        ~ArrayList();
        T& operator[](ulong Index);
        T* data(){
            return m_data;
        }

        ArrayList& operator+=(const T &elemento){
            return add(elemento);
        }
        ArrayList& operator=(const ArrayList& toCopy);
        ArrayList& add(const T& elemento);
        //ArrayList& remove(T &elemento);
        ArrayList& remove(ulong Index);
        ulong size()const{
            return m_size;
        }
        ulong resize(int newSize);
        ulong capacity();
};
template <class T> ArrayList<T>::ArrayList(){
    m_data = 0;
    m_capacity = m_size = 0;
}
template <class T> ArrayList<T>::~ArrayList(){
    for(uint i = 0;i<size();i++){
        m_data[i].~T();
    }
    free(m_data);
}

template <class T> T& ArrayList<T>::operator[](ulong index){
    T* returned = NULL;
    if(m_size>index){
        returned = &m_data[index];
    }else{
		DebufPrintf("out of range: index %x this: %p\n",(uint)index,this);
        DebugBreak();
    }
    return *returned;
}
template <class T> ArrayList<T>& ArrayList<T>::add(const T &elemento){
    if(m_size++){
        m_data = (T*)realloc(m_data,sizeof(T)*m_size);
    }else{
        m_data = (T*)malloc(sizeof(T)*m_size);
    }
    m_data[m_size-1] = elemento;
    return *this;
}
template <class T> ArrayList<T>& ArrayList<T>::remove(const ulong index){
    m_data[index].~T();
    for(uint i = (index+1);i<m_size;i++){
        m_data[i-1] = m_data[i];
    }
    m_size--;
    return *this;
}
#endif // ARRAYLIST_H
