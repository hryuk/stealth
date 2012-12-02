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
