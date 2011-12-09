#include "stealth.h"
#include "ui_stealth.h"

Stealth::Stealth(QWidget *parent) : QMainWindow(parent),
                                    ui(new Ui::Stealth)
{
    ui->setupUi(this);

    server=new Server();
    mngMessage=new MessageManager();
    mngConnection=new ConnectionManager(mngMessage);

    //Cuando el server reciba una nueva conexión, el manager se encargará de inicializarla
    QObject::connect(server,SIGNAL(newConnection(Connection*)),mngConnection,SLOT(SetupConnection(Connection*)));
}

Stealth::~Stealth()
{
    delete ui;
}
