#ifndef TESTPLUGIN_H
#define TESTPLUGIN_H

#include "plugininterface.h"
#include <QWidget>

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

private:
    Ui::GUI* ui;
};

#endif // TESTPLUGIN_H
