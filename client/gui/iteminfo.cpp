#include "iteminfo.h"
#include "ui_iteminfo.h"

ItemInfo::ItemInfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::iteminfo)
{
    ui->setupUi(this);
}

ItemInfo::~ItemInfo()
{
    delete ui;
}
