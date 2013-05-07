#ifndef ITEMINFO_H
#define ITEMINFO_H

#include <QWidget>
#include <QHostAddress>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include "connection.h"

namespace Ui {
class iteminfo;
}

class ItemInfo : public QWidget
{
    Q_OBJECT
    
public:
    explicit ItemInfo(Connection* connection,QWidget *parent = 0);
    ~ItemInfo();
    
private:
    Ui::iteminfo *ui;
};

#endif // ITEMINFO_H
