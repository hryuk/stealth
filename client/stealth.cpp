#include "stealth.h"
#include "ui_stealth.h"

Stealth::Stealth(QWidget *parent) : QMainWindow(parent),
                                    ui(new Ui::Stealth)
{
    ui->setupUi(this);

    server=new Server();
    mngMessage=new MessageManager();
    mngConnection=new ConnectionManager(this,mngMessage);

    //Cuando el server reciba una nueva conexión, el manager se encargará de inicializarla
    QObject::connect(server,SIGNAL(newConnection(Connection*)),mngConnection,SLOT(SetupConnection(Connection*)));

    QApplication::setStyle(QStyleFactory::create("Plastique"));

    for(int i=0;i<3;i++)
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
        ui->MainLayout->addWidget(ctw);
        connect(this,SIGNAL(destroyed()),ctw,SLOT(deleteLater()));
        connect(ctw,SIGNAL(expandedChanged(GroupTreeWidget*)),this,SLOT(closeCurrentExpanded(GroupTreeWidget*)));
        lstGroupWidgets.append(ctw);
    }
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
