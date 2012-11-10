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
    Crypto(QByteArray data);

    QByteArray getData();

    QByteArray sha1();
    QByteArray sha1(QByteArray data);
    QByteArray sha1(QString data);

    QByteArray AES(QByteArray key);
    QByteArray FNV1a(QByteArray data);

    void setData(QByteArray data);

private:
    QByteArray data;
};

#endif // CRYPTO_H
