#include "stealthtab2.h"
#include "ui_stealthtab2.h"

StealthTab2::StealthTab2(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StealthTab2)
{
    ui->setupUi(this);

    /* Ajustamos el tamaño de los headers para que ocupen todo el treewidget */
    ui->treeWidget->header()->setResizeMode(0,QHeaderView::Stretch);
    ui->treeWidget->header()->setResizeMode(1,QHeaderView::Stretch);
}

StealthTab2::~StealthTab2()
{
    delete ui;
}
