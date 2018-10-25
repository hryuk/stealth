#include "crypto.h"

QByteArray Crypto::sha1(QByteArray data)
{
    if(data.isEmpty()) return 0;

    qDebug()<<"Calculando hash SHA-1";
    QByteArray hash=QCryptographicHash::hash(data,QCryptographicHash::Sha1);

    qDebug()<<"Hash SHA-1 calculado 0x"+QString(hash.toHex());

    return hash;
}

QByteArray Crypto::sha1(QString data)
{
    return sha1(data.toLatin1());
}

QByteArray Crypto::FNV1a(QByteArray data)
{
    qDebug()<<"Calculando checksum FNV1a";
    quint32 r=0;

    foreach(char c,data)
    {
        *(char*)&r^=c;
        r*=0x1EF30EB;
    }

    qDebug()<<"Checksum calculado: 0x"+QString::number(r,16);

    return QByteArray((char*)&r,4);
}
