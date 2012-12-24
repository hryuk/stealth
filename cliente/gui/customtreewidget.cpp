#include "customtreewidget.h"

CustomTreeWidget::CustomTreeWidget(QWidget *parent) : QTreeWidget(parent)
{
}

void CustomTreeWidget::drawBranches ( QPainter * painter, const QRect & rect, const QModelIndex & index ) const
{
    //Reimplementado solo para ocultar las ramas
}
