#ifndef CUSTOMTREEWIDGET_H
#define CUSTOMTREEWIDGET_H

#include <QtGui/QTreeWidget>

class CustomTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    explicit CustomTreeWidget(QWidget *parent = 0);

private:
    void drawBranches ( QPainter * painter, const QRect & rect, const QModelIndex & index ) const;

signals:

public slots:

};

#endif // CUSTOMTREEWIDGET_H
