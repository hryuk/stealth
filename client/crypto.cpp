#include "crypto.h"

//Constructor, copia los datos a un nuevo QByteArray
//para hacer sobre el las operaciones criptograficas
Crypto::Crypto(QByteArray data)
{
    this->data.append(data);
}

/*###########################################################
*    CheckSum Jenkins
*    Jenkins (http://goo.gl/cMRaH)
#############################################################*/
quint32 Crypto::jenkins_one_at_a_time(char *data,unsigned int len)
{
    quint32 hash,i;

    for(hash=i=0;i<len;++i)
    {
        hash+=data[i];
        hash+=(hash<<10);
        hash^=(hash>>6);
    }

    hash+=(hash<<3);
    hash^=(hash>>11);
    hash+=(hash<<15);

    return hash;
}

quint32 Crypto::jenkins_one_at_a_time()
{
    if(data.isEmpty()) return 0;

    return jenkins_one_at_a_time(data.data(),data.size());
}

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

QByteArray Crypto::sha1()
{
    if(data.isEmpty()) return 0;

    return sha1(data);
}

QByteArray Crypto::sha1(QString data)
{
    return sha1(data.toAscii());
}

/*###########################################################
*    Encriptacion RC4
*    http://es.wikipedia.org/wiki/RC4
###########################################################*/

QByteArray Crypto::RC4(QByteArray key)
{
    if(data.isEmpty()||key.isEmpty()) return 0;

    RC4((unsigned char*)data.data(),data.size(),(unsigned char*)key.data(),key.size());

    return data;
}

void Crypto::swap(unsigned char *s, unsigned int i, unsigned int j)
{
    unsigned char temp = s[i];
    s[i] = s[j];
    s[j] = temp;
}

void Crypto::rc4_init(unsigned char *key, unsigned int key_length)
{
    for (i = 0; i < 256; i++)
        S[i] = i;

    for (i = j = 0; i < 256; i++) {
        j = (j + key[i % key_length] + S[i]) & 255;
        swap(S, i, j);
    }

    i = j = 0;
}

unsigned char Crypto::rc4_output()
{
    i = (i + 1) & 255;
    j = (j + S[i]) & 255;

    swap(S, i, j);

    return S[(S[i] + S[j]) & 255];
}

void Crypto::RC4(unsigned char* data,unsigned int datasize,unsigned char* key,unsigned int keysize)
{
            rc4_init(key,keysize);

            for(unsigned int i=0;i<datasize;i++)
            {
                data[i]^=rc4_output();
            }
}

QByteArray Crypto::getData()
{
    return this->data;
}

void Crypto::setData(QByteArray data)
{
    this->data.clear();
    this->data.append(data);
}
