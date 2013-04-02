#include "stealthtab1.h"
#include "ui_stealthtab1.h"

StealthTab1::StealthTab1(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StealthTab1)
{
    ui->setupUi(this);

    treewidget=new GroupTreeWidget(true,this);
    ui->gridLayout->addWidget(treewidget);
}

StealthTab1::~StealthTab1()
{
    delete ui;
}
