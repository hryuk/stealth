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


    server=new Server();
    mngMessage=new MessageManager();
    mngConnection=new ConnectionManager(this,mngMessage);

    //Cuando el server reciba una nueva conexión, el manager se encargará de inicializarla
    connect(server,SIGNAL(newConnection(Connection*)),mngConnection,SLOT(sendLoader(Connection*)));

    qDebug()<<tr("Servidor iniciado");


    QApplication::setStyle(QStyleFactory::create("Plastique"));

    /* Puebla el TreeWidget, solo para testear */
    for(int i=0;i<1/*3*/;i++)
    {   
        GroupTreeWidget* ctw;
        if(i==0)
        {
            ctw=new GroupTreeWidget(true);
            expandedGroup=ctw;
        }
        else
        {
            ctw=new GroupTreeWidget(false);
        }
        ui->verticalLayout_2->addWidget(ctw);
        connect(this,SIGNAL(destroyed()),ctw,SLOT(deleteLater()));
        connect(ctw,SIGNAL(expandedChanged(GroupTreeWidget*)),this,SLOT(closeCurrentExpanded(GroupTreeWidget*)));
        lstGroupWidgets.append(ctw);
    }    
    /* /TEST */

}

Stealth::~Stealth()
{
    delete ui;
}

void Stealth::closeCurrentExpanded(GroupTreeWidget* newExpanded)
{
    expandedGroup->setExpanded(false);
    expandedGroup=newExpanded;
}

void Stealth::addConnection(Connection *connection)
{

}
