#ifndef REMOTESHELL_H
#define REMOTESHELL_H

#include "plugininterface.h"
#include "qterminal.h"

#include <QWidget>
#include <QMessageBox>
#include <QFile>
#include <QDebug>
#include <QTextCodec>
#include <QTextEncoder>

namespace Ui {
class GUI;
}

class RemoteShell :  public QWidget,PluginInterface
{
    Q_OBJECT
    Q_INTERFACES(PluginInterface)

public:

    explicit RemoteShell(QWidget *parent = 0);

    QObject* createInstance();
    QWidget* getGUI();
    QString getPluginName();
    QIcon getIcon();
    void recvData(QByteArray);
    QByteArray serverPlugin();

private:
    Ui::GUI* ui;
    QString prompt;
    QTerminal* terminal;

signals:
    void sendData(QByteArray);

private slots:
    void sendCommand();
};

#endif // REMOTESHELL_H
