#include "crypto.h"

#include <QMessageBox>

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

QByteArray Crypto::AES_IV()
{
    QCA::Initializer init;
    QCA::InitializationVector iv(16);

    return iv.toByteArray();
}

QByteArray Crypto::AES(QByteArray IV, QString strKey, QByteArray data)
{
    QCA::Initializer init;

    unsigned int keylen=16;

    QCA::SecureArray password=QCA::hexToArray(Crypto::sha1(strKey).toHex());

    QCA::SymmetricKey key=QCA::PBKDF2("sha1").makeKey(password,0,keylen,1);
    QCA::Cipher cipher("aes128",QCA::Cipher::CBC,QCA::Cipher::NoPadding,QCA::Encode,key,IV);

    QCA::SecureArray u=cipher.process(data);

    return u.toByteArray();
}
