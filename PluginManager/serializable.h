#ifndef SERIALIZABLE_H
#define SERIALIZABLE_H

#include "objectstream.h"

class SerializeNode;
class Serializable{
    public:
        Serializable();
        virtual int serialize(SerializeNode& s) = 0;
        virtual int unserialize(SerializeNode& s) = 0;
        virtual char* getTypeName() = 0;
};

#endif // SERIALIZABLE_H
