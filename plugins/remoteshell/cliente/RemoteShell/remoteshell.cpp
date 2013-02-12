#include "remoteshell.h"
#include "ui_gui.h"

Q_EXPORT_PLUGIN2(remoteshell,RemoteShell)

RemoteShell::RemoteShell(QWidget *parent) : QWidget(parent), ui(new Ui::GUI)
{
    ui->setupUi(this);
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
    ui->textEdit->append(QString::fromUtf16((ushort*)data.data()));
    ui->lineEdit->setFocus();
}

QByteArray RemoteShell::serverPlugin()
{
    QFile filePlugin("remoteshell_server.dll");
    filePlugin.open(QIODevice::ReadOnly);
    QByteArray Plugin=filePlugin.readAll();
    filePlugin.close();

    return Plugin;
}

void RemoteShell::on_lineEdit_returnPressed()
{
    emit sendData(ui->lineEdit->text().toAscii());

    ui->lineEdit->clear();
    ui->lineEdit->setFocus();
}
