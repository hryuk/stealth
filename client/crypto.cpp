#include "crypto.h"

//Constructor, copia los datos a un nuevo QByteArray
//para hacer sobre el las operaciones criptograficas
Crypto::Crypto(QByteArray data)
{
    this->data.append(data);
}

/*###########################################################
*    Hash SHA1
*    Utilizando QCryptographicHash
#############################################################*/
QByteArray Crypto::sha1(QByteArray data)
{
    if(data.isEmpty()) return 0;
    QByteArray hash=QCryptographicHash::hash(data,QCryptographicHash::Sha1);

    return hash;
}

QByteArray Crypto::sha1()
{
    if(data.isEmpty()) return 0;

    return sha1(data);
}

QByteArray Crypto::sha1(QString data)
{
    return sha1(data.toAscii());
}

QByteArray Crypto::FNV1a(QByteArray data)
{
    quint32 r=0;

    foreach(char c,data)
    {
        *(char*)&r^=c;
        r*=0x1EF30EB;
    }


    return QByteArray((char*)&r,4);
}

/*###########################################################
*    Encriptacion AES
###########################################################*/

QByteArray Crypto::AES(QByteArray bakey)
{
    QCA::Initializer init;

    unsigned int keylen=16;
    QCA::InitializationVector iv(16);
    QCA::SecureArray password=QCA::hexToArray(bakey.toHex());

    QCA::SymmetricKey key=QCA::PBKDF2("sha1").makeKey(password,0,keylen,1);
    QCA::Cipher cipher(QString("aes128"),QCA::Cipher::CBC,QCA::Cipher::PKCS7,QCA::Encode,key,iv);

    QCA::SecureArray u=cipher.process(this->data);

    this->data.clear();
    this->data.append(u.toByteArray());

    return iv.toByteArray();
}

QByteArray Crypto::getData()
{
    return this->data;
}

void Crypto::setData(QByteArray data)
{
    this->data.clear();
    this->data.append(data);
}
