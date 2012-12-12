#ifndef OBJECTSTREAM_H
#define OBJECTSTREAM_H

#include "../ArkLib/src/datatypes.h"
#include "serializable.h"

class Serializable;
class SerializeNode{
        SerializeNode* next,*back,*father,*childs;
        char* name;
        DArray data;
    public:
        //Los constructores permiten pasar un objeto serializable o cualquier dato primitivo a SerializeNode
        //Con los datos complejos se va creando un arbol
        SerializeNode();
        SerializeNode(const char *cadena);
        SerializeNode(const char caracter);
        SerializeNode(const unsigned char byte);
        SerializeNode(const unsigned short num);
        SerializeNode(const unsigned long num);
        SerializeNode(const double nun);
        SerializeNode(Serializable& Object);

        ~SerializeNode();
        int toBytes(DArray &buff);

        //Añade y quita nodos al arbol
        void add(SerializeNode& node);
        void del(SerializeNode& node);

        const char* getName() const;
        void setName(const char *);

        void setData(DArray& data);
        DArray& getData();
        //Interface para desserializar el nodo y convertirlo en un tipo de dato determinado
        //Si los tipos de datos no coinciden se devuelve -1
        int queryValue(char &caracter);
        int queryValue(char* &cadena);
        int queryValue(unsigned char &byte);
        int queryValue(unsigned short &num);
        int queryValue(unsigned long &num);
        int queryValue(double &num);
        int queryValue(Serializable &Object);
};
class SerializeObjects{
        SerializeNode* arbol;
    public:
        SerializeObjects();

        //Genera un SerializeObjects a partir de un Array de bytes que representen los objetos
        static SerializeObjects* parse(DArray& byteObjects);
        //Conbierte el arbol de un objeto en un array de bites
        DArray toBytes();
};

#endif // OBJECTSTREAM_H
