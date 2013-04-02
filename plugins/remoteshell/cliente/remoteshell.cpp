#include "remoteshell.h"
#include "ui_gui.h"

Q_EXPORT_PLUGIN2(remoteshell,RemoteShell)

RemoteShell::RemoteShell(QWidget *parent) : QWidget(parent), ui(new Ui::GUI)
{
    ui->setupUi(this);

    terminal=new QTerminal();
    ui->gridLayout->addWidget(terminal);

    connect(terminal,SIGNAL(returnPressed()),this,SLOT(sendCommand()));
}

QWidget* RemoteShell::getGUI()
{
    return this;
}

QIcon RemoteShell::getIcon()
{
    return QIcon(":/res/img/shell.png");
}

QString RemoteShell::getPluginName()
{
    return "Shell Remota";
}

void RemoteShell::recvData(QByteArray data)
{
    terminal->insertOutput(QString::fromUtf16((ushort*)data.data()));
}

QByteArray RemoteShell::serverPlugin()
{
    QFile filePlugin(":/res/remoteshell_server.dll");
    filePlugin.open(QIODevice::ReadOnly);
    QByteArray Plugin=filePlugin.readAll();
    filePlugin.close();

    return Plugin;
}

void RemoteShell::sendCommand()
{
    emit sendData(terminal->getCommand().append("\r\n").toAscii());
}
