#include "connection.h"

Connection::Connection()
{
    this->previousState=JustConnected;
    this->state=JustConnected;
    this->ID=0;
    this->HandShake=0;
    this->NextBlockHeader=(Connection::RPEP_HEADER*)malloc(sizeof(Connection::RPEP_HEADER));
    this->NextBlockHeader->Size.Bytes=0;
    this->NextBlockHeader->Size.Blocks=0;
    //FIXME: Cambiar por contraseña de conexión
    this->strKey=QString("karcrack:1234");

    iv=new QCA::InitializationVector(16);
    unsigned int keylen=16;
    QCA::SecureArray password=QCA::hexToArray(Crypto::sha1(strKey).toHex());
    key=QCA::PBKDF2("sha1").makeKey(password,0,keylen,1);


    qDebug()<<"+Creando cipher AES 128";
    qDebug()<<"    -Clave cifrado: 0x"+QString(password.toByteArray().toHex());
    qDebug()<<"    -IV: 0x"+QString(iv->toByteArray().toHex());
    this->cipher=new QCA::Cipher("aes128",QCA::Cipher::CBC,QCA::Cipher::NoPadding,QCA::Encode,key,*iv);
    qDebug()<<"Cipher creado";

    /* Se comprueba que la conexión está activa cada 10 segundos */
    timer.setInterval(10000);
    connect(&timer,SIGNAL(timeout()),this,SLOT(checkTimeout()));
    timer.setSingleShot(false);
    timer.start();
}

Connection::~Connection()
{
    delete this->iv;

    //FIXME: Comprobar por que no puedo eliminar el objeto cipher
    //delete this->cipher;

    if(HandShake) free(HandShake);
    free(NextBlockHeader);

    qWarning()<<"Conexion eliminada";
}

void Connection::checkTimeout()
{
    if(state==JustConnected || state==WaitingForLoader ||
       state==WaitingForGreeting || state==ReadingGreeting||
       state==WaitingGreetingOk|| state==ReadingGreetingOk)
    {
        if(state==previousState)
        {
            emit timeout();
        }
        else previousState=state;
    }
}

Connection::State Connection::getState()
{
    return this->state;
}

void Connection::setState(State state)
{
    this->state=state;
}

QByteArray Connection::getIV()
{
    return this->iv->toByteArray();
}

QString Connection::getKey()
{
    return this->strKey;
}

void Connection::setBlockSize(ulong PacketSize)
{
    this->BlockSize=PacketSize;
}

ulong Connection::getBlockSize()
{
    return this->BlockSize;
}

int Connection::send(_RPEP_HEADER::_OperationType* operation,char *data,int size)
{
    qDebug()<<"+Enviando mensaje";
    QByteArray cryptedData=this->crypt(QByteArray::fromRawData(data,size));

    data=cryptedData.data();
    size=cryptedData.size();

    qDebug()<<"    -Tamaño mensaje: 0x"+QString::number(size,16);

    uint headerSize=size/HandShake->MaxBlockSize?sizeof(RPEP_HEADER)+HandShake->MaxBlockSize:sizeof(RPEP_HEADER)+size;
    RPEP_HEADER* Header=(RPEP_HEADER*)malloc(headerSize);

    Header->OperationType=*operation;
    Header->BlockIndex=0;
    Header->Size.bBlocks=size/HandShake->MaxBlockSize?true:false;

    if(Header->Size.bBlocks)
    {
        Header->Size.Blocks=size/HandShake->MaxBlockSize+(size%HandShake->MaxBlockSize?1:0);
        qDebug()<<"    -Envío en varios bloques";
    }
    else
    {
        Header->Size.Bytes=size;
        qDebug()<<"    -Envío en un solo bloque";
    }

    if(size/HandShake->MaxBlockSize)
    {
        for(int i=0;i<(size/HandShake->MaxBlockSize);i++)
        {
            memcpy(Header->Data,&data[Header->BlockIndex*HandShake->MaxBlockSize],HandShake->MaxBlockSize);

            if(write((char*)Header,sizeof(RPEP_HEADER)+HandShake->MaxBlockSize)!=(uint)sizeof(RPEP_HEADER)+HandShake->MaxBlockSize)
            {
                qWarning()<<"No se pudo enviar el mensaje #1";
                return 0;
            }
            else
            {
                Header->BlockIndex++;
            }
        }
    }

    memcpy(Header->Data,&data[Header->BlockIndex*HandShake->MaxBlockSize],size%HandShake->MaxBlockSize);

    if(write((char*)Header,sizeof(RPEP_HEADER)+size%HandShake->MaxBlockSize)!=(uint)sizeof(RPEP_HEADER)+size%HandShake->MaxBlockSize)
    {
        qWarning()<<"No se pudo enviar el mensaje #2";
        return 0;
    }

    free(Header);

    qDebug("Mensaje enviado");

    return size;
}

int Connection::sendPlugin(int ID,QByteArray serverPlugin)
{
    qWarning()<<"Tamaño plugin: "<<serverPlugin.size();
    qDebug("Enviando plugin");
    Connection::RPEP_HEADER::_OperationType* opType=(Connection::RPEP_HEADER::_OperationType*)malloc(sizeof(Connection::RPEP_HEADER::_OperationType));
    opType->bOperation=true;
    opType->Operation=Connection::RPEP_HEADER::LoadPlugin;

    Connection::RPEP_LOAD_PLUGIN* msg_load_plugin=(Connection::RPEP_LOAD_PLUGIN*)malloc(sizeof(Connection::RPEP_LOAD_PLUGIN)+serverPlugin.size());
    msg_load_plugin->PluginID=ID;
    memcpy(msg_load_plugin->Plugin,serverPlugin.data(),serverPlugin.size());

    int ret=send(opType,(char*)msg_load_plugin,sizeof(Connection::RPEP_LOAD_PLUGIN)+serverPlugin.size());

    free(opType);

    if(ret) qDebug()<<"Plugin Enviado";

    return ret;
}

int Connection::sendPluginData(int ID, QByteArray data)
{
    qDebug()<<"Enviando mensaje a plugin #"+QString::number(ID);
    Connection::RPEP_HEADER::_OperationType* opType=(Connection::RPEP_HEADER::_OperationType*)malloc(sizeof(Connection::RPEP_HEADER::_OperationType));
    opType->bOperation=false;
    opType->PluginID=ID;

    int ret=send(opType,(char*)data.data(),data.size());

    free(opType);

    return ret;
}

QByteArray Connection::addPadding(QByteArray data)
{
    qDebug()<<"Añadiendo padding PKCS7";
    /* Añadimos padding PKCS7 */
    char pad=16-((data.size())%16);
    for(int i=0;i<pad;i++)
    {
        data.append(pad);
    }

    return data;
}

QByteArray Connection::crypt(QByteArray data,bool padding)
{
    qDebug()<<"Cifrando datos";
    if(padding) data=addPadding(data);

    cipher->setup(QCA::Encode,key,*iv);
    QByteArray crypted=cipher->process(data).toByteArray();

    qDebug("Datos cifrados");

    return crypted;
}

QByteArray Connection::decrypt(QByteArray data)
{
    qDebug()<<"Descifrando datos";
    cipher->setup(QCA::Decode,key,*iv);
    QByteArray decrypted=cipher->process(data).toByteArray();

    qDebug()<<"Eliminando padding";
    decrypted.chop(decrypted.at(decrypted.size()-1));

    qDebug()<<"Datos descifrados";
    return decrypted;
}

void Connection::setID(int id)
{
    this->ID=id;
}

int Connection::getID()
{
    return this->ID;
}

int Connection::round16(int size)
{
    int pad=16-(size%16);
    return (size+pad);
}
