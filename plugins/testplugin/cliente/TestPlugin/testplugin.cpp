#include "testplugin.h"
#include "ui_gui.h"

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

Q_EXPORT_PLUGIN2(testplugin,TestPlugin)
