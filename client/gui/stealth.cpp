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


    //TODO: Mover a main.cpp?
    server=new Server();
    mngMessage=new MessageManager();
    mngConnection=new ConnectionManager(this,mngMessage);

    //Cuando el server reciba una nueva conexión, el manager se encargará de inicializarla
    connect(server,SIGNAL(newConnection(Connection*)),mngConnection,SLOT(sendLoader(Connection*)));
    connect(mngConnection,SIGNAL(connectionReady(Connection*)),this,SLOT(addConnection(Connection*)));

    qDebug()<<tr("Servidor iniciado");


 //   QApplication::setStyle(QStyleFactory::create("Plastique"));

    treewidget=new GroupTreeWidget(true);
    ui->centralFrameLayout->addWidget(treewidget);
    connect(this,SIGNAL(destroyed()),treewidget,SLOT(deleteLater()));
    //connect(ctw,SIGNAL(expandedChanged(GroupTreeWidget*)),this,SLOT(closeCurrentExpanded(GroupTreeWidget*)));
    lstGroupWidgets.append(treewidget);

    /* TEST */
    this->addConnection(0);
    /* /TEST */

    connect(treewidget->treewidget,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),this,SLOT(itemDoubleClicked(QTreeWidgetItem*,int)));
}

Stealth::~Stealth()
{
    delete ui;
}

void Stealth::itemDoubleClicked(QTreeWidgetItem *item,int column)
{
    PluginWindow* pluginWindow=new PluginWindow(this);
    pluginWindow->show();
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
    //FIXME: Cambiar cuando se implementen los grupos
    QTreeWidgetItem* item=new QTreeWidgetItem();
    this->lstGroupWidgets.at(0)->addItem(item);
}
