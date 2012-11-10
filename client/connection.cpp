#include "connection.h"

Connection::Connection()
{
    this->state=JustConnected;
    this->NextBlockHeader.Size.Bytes=0;
    this->NextBlockHeader.Size.Blocks=0;
    this->IV=QByteArray(16,0);
}

Connection::State Connection::getState()
{
    return this->state;
}

void Connection::setState(State state)
{
    this->state=state;
}

void Connection::setIV(QByteArray IV)
{
    this->IV.clear();
    this->IV.append(IV);
}

QByteArray Connection::getIV()
{
    return this->IV;
}

void Connection::setBlockSize(ulong PacketSize)
{
    this->BlockSize=PacketSize;
}

ulong Connection::getBlockSize()
{
    return this->BlockSize;
}

int Connection::send(_RPEP_HEADER::_OperationType* operation, char *data, int size)
{
    RPEP_HEADER* Header=(RPEP_HEADER*)malloc(sizeof(RPEP_HEADER)+HandShake.MaxBlockSize);
    Header->OperationType=*operation;
    Header->BlockIndex=0;
    Header->Size.bBlocks=true;
    Header->Size.Blocks=size/this->HandShake.MaxBlockSize+(size%this->HandShake.MaxBlockSize?1:0);

    if(size/this->HandShake.MaxBlockSize)
    {
        for(int i=0;i<(size/this->HandShake.MaxBlockSize);i++)
        {
            Header->BlockIndex++;
            memcpy(Header->Data,&data[Header->BlockIndex*HandShake.MaxBlockSize],HandShake.MaxBlockSize);

            if(write((char*)Header,sizeof(RPEP_HEADER)+HandShake.MaxBlockSize)!=sizeof(RPEP_HEADER)+HandShake.MaxBlockSize)
            {
                qWarning()<<tr("No se pudieron enviar los datos #1");
                return 0;
            }
        }
    }

    Header->BlockIndex++;
    memcpy(Header->Data,&data[Header->BlockIndex*HandShake.MaxBlockSize],size%HandShake.MaxBlockSize);

    if(write((char*)Header,sizeof(RPEP_HEADER)+size%HandShake.MaxBlockSize)!=sizeof(RPEP_HEADER)+size%HandShake.MaxBlockSize)
    {
        qWarning()<<tr("No se pudieron enviar los datos #1");
        return 0;
    }

    free(Header);

    return size;
}
