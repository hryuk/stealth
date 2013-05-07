#include "pluginwindow.h"
#include "ui_pluginwindow.h"

PluginWindow::PluginWindow(Connection* connection,QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::PluginWindow)
{

    ui->setupUi(this);

    setGeometry(
            (int)(QApplication::desktop()->width() -
                (QApplication::desktop()->width() -
                (QApplication::desktop()->width() / 2)) * 1.35) / 2,
            (int)(QApplication::desktop()->height() -
                (QApplication::desktop()->height() -
                (QApplication::desktop()->height() / 2)) * 1.35) / 2,
            (int)((QApplication::desktop()->width() -
                (QApplication::desktop()->width() / 2)) * 1.35),
            (int)((QApplication::desktop()->height() -
                (QApplication::desktop()->height() / 2)) * 1.35));

    this->connection=connection;
    this->ID=connection->getID();
    this->pluginManager=new PluginManager();

    connect(pluginManager,SIGNAL(sendData(int,QByteArray)),connection,SLOT(sendPluginData(int,QByteArray)));

    leftBoxLayout=new QVBoxLayout();
    leftBoxLayout->setMargin(0);

    qDebug()<<"Cargados "<<pluginManager->plugins.count()<<" plugins";

    foreach(PluginInterface* plugin,pluginManager->plugins)
    //PluginInterface* plugin=pluginManager->plugins.at(0); /*Cargar solo un plugin */
    {
        QPushButton* button=new QPushButton(plugin->getIcon(),plugin->getPluginName(),this);
        button->setCheckable(true);
        button->setAutoExclusive(true);
        button->setFocusPolicy(Qt::NoFocus);
        if(leftBoxLayout->count()==0)
        {
            button->setChecked(true);
            ui->stackedWidget->insertWidget(0,plugin->getGUI());
            ui->stackedWidget->setCurrentIndex(0);
        }
        connect(button,SIGNAL(clicked(bool)),this,SLOT(leftBoxButton_clicked(bool)));
        leftBoxLayout->addWidget(button);
    }

    leftBoxLayout->addStretch(1);
    ui->scrollAreaWidget->setLayout(leftBoxLayout);
}

PluginWindow::~PluginWindow()
{
    delete ui;
}

void PluginWindow::showEvent(QShowEvent*)
{
    /* Ponemos la dirección del servidor como título de la ventana
        y enviamos el primer plugin al servidor para que lo cargue */
    setWindowTitle(connection->peerAddress().toString());
    connection->sendPlugin(0,pluginManager->plugins.at(0)->serverPlugin());
    pluginManager->setPluginRunning(0);
}

void PluginWindow::leftBoxButton_clicked(bool /*checked*/)
{
    int buttonIndex=leftBoxLayout->indexOf(qobject_cast<QPushButton*>(sender()));

    /*FIXME: De esto deberia encargarse el pluginmanager*/
    if(!pluginManager->isPluginRunning(buttonIndex))
    {
        ui->stackedWidget->insertWidget(ui->stackedWidget->count()-1,pluginManager->plugins.at(buttonIndex)->getGUI());
        connection->sendPlugin(buttonIndex,pluginManager->plugins.at(buttonIndex)->serverPlugin());
        pluginManager->setPluginRunning(buttonIndex);
        ui->stackedWidget->setCurrentIndex(buttonIndex);
    }
    else
    {
        ui->stackedWidget->setCurrentIndex(buttonIndex);
    }
}

int PluginWindow::getID()
{
    return this->ID;
}
