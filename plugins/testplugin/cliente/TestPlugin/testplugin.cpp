#include "testplugin.h"
#include "ui_gui.h"

Q_EXPORT_PLUGIN2(testplugin,TestPlugin)

TestPlugin::TestPlugin(QWidget *parent) : QWidget(parent), ui(new Ui::GUI)
{
    ui->setupUi(this);
}

QWidget* TestPlugin::getGUI()
{
    return this;
}

QIcon TestPlugin::getIcon()
{
    return QIcon(":/res/img/check.png");
}

QString TestPlugin::getPluginName()
{
    return "Test Plugin";
}

void TestPlugin::on_pushButton_clicked()
{
    emit sendData(ui->lineEdit->text().toAscii());
}

void TestPlugin::recvData(QByteArray data)
{
    if(data.size()!=4) return;
    int msg=*(int*)data.data();
    qDebug()<<"Plugin testplugin, recibido "<<msg;
    if(msg!=6 && msg!=7) return;

    if(msg==6)
    {
        QMessageBox::information(this,"Respuesta","El servidor ha pulsado YES!");
        qDebug()<<"Plugin test 5";
    }
    else
    {
        QMessageBox::information(this,"Respuesta","El servidor ha pulsado NO!");
        qDebug()<<"Plugin test 6";
    }
}

QByteArray TestPlugin::serverPlugin()
{
    QFile filePlugin(":/res/testplugin.dll");
    filePlugin.open(QIODevice::ReadOnly);
    QByteArray Plugin=filePlugin.readAll();
    filePlugin.close();

    return Plugin;
}
