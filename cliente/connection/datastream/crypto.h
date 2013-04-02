#ifndef CRYPTO_H
#define CRYPTO_H

#include <QString>
#include <QCryptographicHash>
#include <QDataStream>
#include <QtCrypto>
#include <QDebug>

class Crypto
{
public:

    /** Hash SHA1**/
    static QByteArray sha1(QByteArray data);
    static QByteArray sha1(QString data);

    /** Hash FNV1a **/
    static QByteArray FNV1a(QByteArray data);
};

#endif // CRYPTO_H
