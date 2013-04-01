#include "objectstream.h"
#include "stdlib.h"

SerializeNode::SerializeNode(){
    next = back = father = childs = NULL;
    name = 0;
}
SerializeNode::SerializeNode(const char *cadena){
    next = back = father = childs = NULL;
    name = 0;
    setName("string");
    if(cadena){
        int len = strlen(cadena);
        data.addData(cadena,len);
    }
}
SerializeNode::SerializeNode(const char caracter){
    next = back = father = childs = NULL;
    name = 0;
    //Pongo el nombre del typo
    setName("char");
    //Agrego el valor al array
    data.addData(&caracter,sizeof(caracter));
}
SerializeNode::SerializeNode(const unsigned char byte){
    next = back = father = childs = NULL;
    name = 0;
    //Pongo el nombre del typo
    setName("byte");
    //Agrego el valor al array
    data.addData(&byte,sizeof(byte));
}
SerializeNode::SerializeNode(const unsigned short num){
    next = back = father = childs = NULL;
    name = 0;
    //Pongo el nombre del typo
    setName("word");
    //Agrego el valor al array
    data.addData(&num,sizeof(num));
}
SerializeNode::SerializeNode(const unsigned long num){
    next = back = father = childs = NULL;
    name = 0;
    //Pongo el nombre del typo
    setName("dword");
    //Agrego el valor al array
    data.addData(&num,sizeof(num));
}
SerializeNode::SerializeNode(const double nun){
    next = back = father = childs = NULL;
    name = 0;
    setName("double");
    //Agrego el valor al array
    data.addData(&nun,sizeof(nun));
}
SerializeNode::SerializeNode(Serializable &Object){
    next = back = father = childs = NULL;
    name = 0;
    //Pongo el nombre del typo
    setName(Object.getTypeName());

    Object.serialize(*this);
}

SerializeNode::~SerializeNode(){
    for(SerializeNode* currentNode = childs,*nextNode;currentNode;currentNode = nextNode){
        nextNode = currentNode->next;
        delete currentNode;
    }
    if(father && father->childs == this)father->childs = next;
    if(next)next->back = back;
    if(back)back->next = next;
}

//Setea el nombre del tipo que representa este nodo
void SerializeNode::setName(const char *name){
    if(!this->name){
        free(this->name);
    }
    if(name){
        int len = strlen(name)+1;
        this->name = (char*)malloc(len*sizeof(*name));
        memcpy(this->name,name,len*sizeof(*name));
    }
}
//Recupera el nodo del tipo que representa el nodo
const char* SerializeNode::getName() const{
    return name;
}
void SerializeNode::setData(DArray &data){
    this->data = data;
}

void SerializeNode::add(SerializeNode &node){
    if(childs){
        //pongo el nodo en la ultima posicion
        for(SerializeNode* currentNode = childs;currentNode;currentNode = currentNode->next){
            if(currentNode->next){
                node.back = currentNode;
                currentNode->next = &node;
                break;
            }
        }
    }else{
        //Al no tener hijos lo pongo apuntando en childs
        childs = &node;
    }
    node.father = this;
}

void SerializeNode::del(SerializeNode &node){
    if(node.father && node.father->childs == &node){
        childs = node.next;
    }
    if(node.next)node.next->back = node.back;
    if(node.back)node.back->next = node.next;
}

int SerializeNode::queryValue(char &caracter){
    int result = -1;
    if(!strcmp("char",getName())){
        if(data.size == sizeof(caracter))caracter = *((char*)data.data);
    }
    return result;
}
int SerializeNode::queryValue(unsigned char &byte){
    int result = -1;
    if(!strcmp("byte",getName())){
        if(data.size == sizeof(byte))byte = *((unsigned char*)data.data);
    }
    return result;
}
int SerializeNode::queryValue(char *&cadena){
    int result = -1;
    if(!strcmp("string",getName())){
        cadena = (char*)malloc(data.size+1);
        memcpy(cadena,data.data,data.size);
        result = data.size;
    }
    return result;
}
int SerializeNode::queryValue(unsigned short &num){
    int result = -1;
    if(!strcmp("word",getName())){
        if(data.size == sizeof(num)){
            num = *((unsigned short*)data.data);
            result = data.size;
        }
    }
    return result;
}
int SerializeNode::queryValue(unsigned long &num){
    int result = -1;
    if(!strcmp("dword",getName())){
        if(data.size == sizeof(num)){
            num = *((unsigned long*)data.data);
            result = data.size;
        }
    }
    return result;
}
int SerializeNode::queryValue(double &num){
    int result = -1;
    if(!strcmp("double",getName())){
        if(data.size == sizeof(num)){
            num = *((double*)data.data);
            result = data.size;
        }
    }
    return result;
}
int SerializeNode::queryValue(Serializable &Object){
    int result = -1;
    if(!strcmp(this->getName(),Object.getTypeName())){
        result = Object.unserialize(*this);
    }
    return result;
}
int SerializeNode::toBytes(DArray &buff){
    ushort nameLen = strlen(name);
    ushort size = nameLen+sizeof(nameLen);
    //int sizeIndex = buff.size;

    for(SerializeNode* currentNode = childs;currentNode;currentNode = currentNode->next){
        size += currentNode->toBytes(buff);
    }

    return size;
}


SerializeObjects::SerializeObjects(){

}
SerializeObjects* SerializeObjects::parse(DArray &/*byteObjects*/){
    return null;
}
DArray SerializeObjects::toBytes(){
    DArray buff;

    this->arbol->toBytes(buff);

    return buff;
}
