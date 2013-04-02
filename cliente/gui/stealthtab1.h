#ifndef STEALTHTAB1_H
#define STEALTHTAB1_H

#include <QWidget>

#include "grouptreewidget.h"

namespace Ui {
class StealthTab1;
}

class StealthTab1 : public QWidget
{
    Q_OBJECT
    
public:
    explicit StealthTab1(QWidget *parent = 0);
    ~StealthTab1();
    GroupTreeWidget* treewidget;
    
private:
    Ui::StealthTab1 *ui;
};

#endif // STEALTHTAB1_H
