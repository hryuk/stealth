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
    QByteArray FNV1a_get_offset_basis(QByteArray data);

    void setData(QByteArray data);

private:
    QByteArray data;
    unsigned char S[256];
    unsigned int i, j;
};

#endif // CRYPTO_H
