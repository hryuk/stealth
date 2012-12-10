#ifndef TESTPLUGIN_H
#define TESTPLUGIN_H

#include "plugininterface.h"
#include <QWidget>
#include <QMessageBox>
#include <QFile>

namespace Ui {
class GUI;
}

class TestPlugin :  public QWidget,PluginInterface
{
    Q_OBJECT
    Q_INTERFACES(PluginInterface)

public:

    explicit TestPlugin(QWidget *parent = 0);

    QWidget* getGUI();
    QString getPluginName();
    QIcon getIcon();
    void recvData(QByteArray);
    QByteArray serverPlugin();

private:
    Ui::GUI* ui;

private slots:
    void on_pushButton_clicked();

signals:
    void sendData(QByteArray);
};

#endif // TESTPLUGIN_H
