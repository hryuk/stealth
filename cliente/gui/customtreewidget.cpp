#include "customtreewidget.h"

CustomTreeWidget::CustomTreeWidget(QWidget *parent) : QTreeWidget(parent)
{
}

void CustomTreeWidget::drawBranches (QPainter*,const QRect&,const QModelIndex&) const
{
    /* Reimplementado solo para ocultar las ramas */
}
