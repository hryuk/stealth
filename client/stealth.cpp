#include "stealth.h"
#include "ui_stealth.h"

Stealth::Stealth(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Stealth)
{
    ui->setupUi(this);
}

Stealth::~Stealth()
{
    delete ui;
}
