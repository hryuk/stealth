#ifndef GROUPTREEWIDGET_H
#define GROUPTREEWIDGET_H

#include <QTreeWidget>
#include <QLabel>
#include <QGridLayout>
#include <QPushButton>
#include <QHeaderView>
#include <QGraphicsView>
#include <QGraphicsScene>

#include "customtreewidget.h"

class GroupTreeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GroupTreeWidget(bool expanded,QWidget *parent = 0);
    ~GroupTreeWidget();

private:
    bool expanded;
    QTimer* timer;
    QRect lastFrameSize;
    int AnimationSteps;
    CustomTreeWidget* treewidget;
    QFrame* frame;
    QLabel* iconLabel;

private slots:
    void on_btnExpand_clicked();
    void on_treewidget_itemClicked(QTreeWidgetItem *item, int column);

public slots:
    void setExpanded(bool expanded);
    bool isExpanded();

signals:
    void expandedChanged(GroupTreeWidget* newExpanded);
};

#endif // CUSTOMTREEWIDGET_H
