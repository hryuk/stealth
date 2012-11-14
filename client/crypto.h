#ifndef CRYPTO_H
#define CRYPTO_H

#include <QString>
#include <QCryptographicHash>
#include <QDataStream>
#include <QtCrypto>

#include <QMessageBox>

class Crypto
{
public:
    static QByteArray sha1(QByteArray data);
    static QByteArray sha1(QString data);

    static QByteArray AES_IV(QByteArray key);
    static QByteArray AES(QByteArray IV, QByteArray key, QByteArray data );
    static QByteArray FNV1a(QByteArray data);
};

#endif // CRYPTO_H
