#ifndef CRYPTO_H
#define CRYPTO_H

#include <QString>
#include <QCryptographicHash>
#include <QDataStream>

class Crypto
{
public:
    Crypto(QByteArray data);

    QByteArray getData();

    quint32 jenkins_one_at_a_time();

    QByteArray sha1();
    QByteArray sha1(QByteArray data);
    QByteArray sha1(QString data);

    QByteArray RC4(QByteArray key);

    void setData(QByteArray data);

private:
    QByteArray data;
    unsigned char S[256];
    unsigned int i, j;

    //RC4
    void swap(unsigned char *s, unsigned int i, unsigned int j);
    void rc4_init(unsigned char *key, unsigned int key_length);
    unsigned char rc4_output();
    void RC4(unsigned char* data,unsigned int datasize,unsigned char* key,unsigned int keysize);

    //Jenkins
    quint32 jenkins_one_at_a_time(char *data,unsigned int len);
};

#endif // CRYPTO_H
