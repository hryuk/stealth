#include "iteminfo.h"
#include "ui_iteminfo.h"

ItemInfo::ItemInfo(Connection *connection, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::iteminfo)
{
    ui->setupUi(this);

    if(connection)
    {
        ui->label_2->setText(connection->peerAddress().toString());
    }
}

ItemInfo::~ItemInfo()
{
    delete ui;
}
