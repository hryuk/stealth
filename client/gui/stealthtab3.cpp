#include "stealthtab3.h"
#include "ui_stealthtab3.h"

StealthTab3::StealthTab3(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StealthTab3)
{
    ui->setupUi(this);
}

StealthTab3::~StealthTab3()
{
    delete ui;
}
