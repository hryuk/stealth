#include "stealthtab4.h"
#include "ui_stealthtab4.h"

StealthTab4::StealthTab4(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StealthTab4)
{
    ui->setupUi(this);

    ui->treeWidget->header()->setSectionResizeMode(0,QHeaderView::Stretch);
    ui->treeWidget->header()->setSectionResizeMode(1,QHeaderView::Stretch);
    ui->treeWidget->header()->setSectionResizeMode(2,QHeaderView::Stretch);
}

StealthTab4::~StealthTab4()
{
    delete ui;
}
