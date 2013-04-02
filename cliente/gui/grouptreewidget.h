#ifndef GROUPTREEWIDGET_H
#define GROUPTREEWIDGET_H

#include <QTreeWidget>
#include <QLabel>
#include <QGridLayout>
#include <QPushButton>
#include <QHeaderView>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QTime>

#include "customtreewidget.h"
#include "iteminfo.h"
#include "connection.h"

class GroupTreeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GroupTreeWidget(bool expanded,QWidget *parent = 0);
    ~GroupTreeWidget();
    CustomTreeWidget* treewidget;

private:
    bool expanded;
    QTimer* timer;
    QRect lastFrameSize;
    int AnimationSteps;   
    QFrame* frame;
    QLabel* iconLabel;

public slots:
    void addItem(Connection*);
};

#endif // CUSTOMTREEWIDGET_H
