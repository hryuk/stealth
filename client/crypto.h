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

    quint32 jenkins_one_at_a_time();

    QByteArray sha1();
    QByteArray sha1(QByteArray data);
    QByteArray sha1(QString data);

    QByteArray AES(QByteArray key);

    void setData(QByteArray data);

private:
    QByteArray data;
    unsigned char S[256];
    unsigned int i, j;

    //Jenkins
    quint32 jenkins_one_at_a_time(char *data,unsigned int len);
};

#endif // CRYPTO_H
