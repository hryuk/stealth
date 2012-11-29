#ifndef ITEMINFO_H
#define ITEMINFO_H

#include <QWidget>

namespace Ui {
class iteminfo;
}

class ItemInfo : public QWidget
{
    Q_OBJECT
    
public:
    explicit ItemInfo(QWidget *parent = 0);
    ~ItemInfo();
    
private:
    Ui::iteminfo *ui;
};

#endif // ITEMINFO_H
