#include "pluginwindow.h"
#include "ui_pluginwindow.h"

PluginWindow::PluginWindow(Connection *connection, QList<PluginInterface *> plugins, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PluginWindow)
{
    this->connection=connection;
    this->ID=connection->getID();

    ui->setupUi(this);

    QVBoxLayout* layout=new QVBoxLayout();
    layout->setMargin(0);

    foreach(PluginInterface* plugin,plugins)
    {
        QPushButton* button=new QPushButton(plugin->getIcon(),plugin->getPluginName(),this);
        button->setCheckable(true);
        button->setAutoExclusive(true);
        if(layout->count()==0)
        {
            button->setChecked(true);
            ui->centralFrame->layout()->addWidget(plugin->getGUI());
        }
        layout->addWidget(button);
    }

    layout->addStretch(1);
    ui->scrollAreaWidget->setLayout(layout);
}

PluginWindow::~PluginWindow()
{
    delete ui;
}

void PluginWindow::showEvent(QShowEvent *)
{
    setWindowTitle(connection->peerAddress().toString());
}

int PluginWindow::getID()
{
    return this->ID;
}
