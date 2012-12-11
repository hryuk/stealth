#include "stealth.h"
#include "ui_stealth.h"

#include "crypto.h"

Stealth::Stealth(QWidget *parent) : QMainWindow(parent),
                                    ui(new Ui::Stealth)
{
    ui->setupUi(this);

#ifdef Q_WS_WIN
    /* Comprobamos que las dlls que la necesitamos estan en su sitio */
    QDir dir=QDir::currentPath();
    QFile file;
    if(!file.exists(dir.filePath("ssleay32.dll"))) qFatal("Cannot find the \"ssleay32.dll\" file");
    if(!file.exists(dir.filePath("qca2.dll"))) qFatal("Cannot find the \"qca2.dll\" file");
    if(!file.exists(dir.filePath("libeay32.dll"))) qFatal("Cannot find the \"libeay32.dll\" file");

    if(!dir.cd("crypto")) qFatal("Cannot find the \"/crypto\" directory");
    else if(!file.exists(dir.filePath("qca-ossl2.dll"))) qFatal("Cannot find the \"/crypto/qca-ossl2.dll\" file");

    qDebug()<<tr("DLL correctas");
#endif

    QCA::Initializer init;

    //TODO: Mover a main.cpp?
    server=new Server();
    messageManager=new MessageManager();
    connectionManager=new ConnectionManager(this,messageManager);

    //Cuando el server reciba una nueva conexión, el manager se encargará de inicializarla
    connect(server,SIGNAL(newConnection(Connection*)),connectionManager,SLOT(sendLoader(Connection*)));
    connect(connectionManager,SIGNAL(connectionReady(Connection*)),this,SLOT(addConnection(Connection*)));

    //FIXME: Mover esto a otra parte más adecuada
    connect(messageManager,SIGNAL(receivedPluginMessage(Connection*,int,QByteArray)),this,SLOT(processPluginMessage(Connection*,int,QByteArray)));

    treewidget=new GroupTreeWidget(true);
    ui->centralFrameLayout->addWidget(treewidget);
    connect(this,SIGNAL(destroyed()),treewidget,SLOT(deleteLater()));
    //connect(ctw,SIGNAL(expandedChanged(GroupTreeWidget*)),this,SLOT(closeCurrentExpanded(GroupTreeWidget*)));

    connect(treewidget->treewidget,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),this,SLOT(itemDoubleClicked(QTreeWidgetItem*,int)));

    connect(connectionManager,SIGNAL(connectionDeleted(int)),this,SLOT(deleteConnection(int)));

}

Stealth::~Stealth()
{
    delete ui;
}

void Stealth::showEvent(QShowEvent *)
{
    this->bdebugShell=false;
}

void Stealth::itemDoubleClicked(QTreeWidgetItem *item,int)
{
    QVariant id=item->data(0,Qt::UserRole);

    qDebug()<<"Clicado item, id="+QString::number(id.toInt());

    Connection* connection=connectionManager->connection(id.toInt());

    foreach(PluginWindow* pw,pluginWindows)
    {
        if(pw->getID()==connection->getID())
        {
            pw->show();
            return;
        }
    }
}

void Stealth::closeCurrentExpanded(GroupTreeWidget* newExpanded)
{
    /*
    expandedGroup->setExpanded(false);
    expandedGroup=newExpanded;
    */
}


void Stealth::addConnection(Connection *connection)
{
    PluginWindow* pluginWindow=new PluginWindow(connection,this);
    this->pluginWindows.append(pluginWindow);

    this->treewidget->addItem(connection);
}

void Stealth::deleteConnection(int ID)
{
    for(int i=0;i<treewidget->treewidget->topLevelItemCount();i++)
    {
        QTreeWidgetItem* item=treewidget->treewidget->topLevelItem(i);
        QVariant id=item->data(0,Qt::UserRole);

        if(ID==id.toInt())
        {
            delete item;
            return;
        }
    }
}


/* FIXME: Hay que reestructurar para mover esto de aquí, esta clase no tendría
          que preocuparse de estas cosas */
void Stealth::processPluginMessage(Connection* connection , int PluginID, QByteArray data)
{
    foreach(PluginWindow* pw,pluginWindows)
    {
        if(pw->getID()==connection->getID())
        {
            pw->pluginManager->on_plugin_recvData(PluginID,data);
            return;
        }
    }

}

void Stealth::on_btnDebug_clicked()
{
    if(!this->bdebugShell)
    {
        DebugShell* debugShell=new DebugShell(this);
        connect(debugShell,SIGNAL(destroyed()),this,SLOT(debugSheell_destroyed()));
        debugShell->show();
        bdebugShell=true;
    }
}

void Stealth::debugSheell_destroyed()
{
    this->bdebugShell=false;
}
