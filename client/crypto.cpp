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
*    Encriptacion AES
###########################################################*/

QByteArray Crypto::AES(QByteArray bakey)
{
    QCA::Initializer init;

    QCA::SymmetricKey key(bakey);
    QCA::InitializationVector iv(16);

    QCA::Cipher cipher(QString("aes128"),QCA::Cipher::CBC,QCA::Cipher::PKCS7,QCA::Encode,key,iv);

    QCA::SecureArray u=cipher.process(this->data);

    if (cipher.ok())
    {
        QMessageBox::information(0,":P","Encriptacion correcta");
        QMessageBox::information(0,":P",QString::number(u.toByteArray().size()));
    }

    this->data.clear();
    this->data.append(u.toByteArray());

    return iv.toByteArray();
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
