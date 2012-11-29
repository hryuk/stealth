#include "pluginwindow.h"
#include "ui_pluginwindow.h"

PluginWindow::PluginWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PluginWindow)
{
    ui->setupUi(this);
}

PluginWindow::~PluginWindow()
{
    delete ui;
}
