#include "crypto.h"

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

QByteArray Crypto::AES_IV(QByteArray key)
{
    QCA::Initializer init;
    QCA::InitializationVector iv(16);

    return iv.toByteArray();
}

QByteArray Crypto::AES(QByteArray IV, QByteArray strKey, QByteArray data)
{
    unsigned int keylen=16;

    QCA::SecureArray password=QCA::hexToArray(strKey.toHex());

    QCA::SymmetricKey key=QCA::PBKDF2("sha1").makeKey(password,0,keylen,1);
    QCA::Cipher cipher(QString("aes128"),QCA::Cipher::CBC,QCA::Cipher::PKCS7,QCA::Encode,key,IV);

    QCA::SecureArray u=cipher.process(data);

    return u.toByteArray();
}
