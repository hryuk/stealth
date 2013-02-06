#include "pluginwindow.h"
#include "ui_pluginwindow.h"

PluginWindow::PluginWindow(Connection *connection, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PluginWindow)
{

    ui->setupUi(this);

    this->connection=connection;
    this->ID=connection->getID();
    this->pluginManager=new PluginManager();

    connect(pluginManager,SIGNAL(sendData(int,QByteArray)),connection,SLOT(sendPluginData(int,QByteArray)));

    QVBoxLayout* layout=new QVBoxLayout();
    layout->setMargin(0);

    //foreach(PluginInterface* plugin,pluginManager->plugins)
    PluginInterface* plugin=pluginManager->plugins.at(0);
    {
        QPushButton* button=new QPushButton(plugin->getIcon(),plugin->getPluginName(),this);
        button->setCheckable(true);
        button->setAutoExclusive(true);
        button->setFocusPolicy(Qt::NoFocus);
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
    connection->sendPlugin(0,pluginManager->plugins.at(0)->serverPlugin());
}

int PluginWindow::getID()
{
    return this->ID;
}
