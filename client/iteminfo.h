#ifndef ITEMINFO_H
#define ITEMINFO_H

#include <QWidget>

namespace Ui {
    class ItemInfo;
}

class ItemInfo : public QWidget
{
    Q_OBJECT

public:
    explicit ItemInfo(QWidget *parent = 0);
    ~ItemInfo();

private:
    Ui::ItemInfo *ui;
};

#endif // ITEMINFO_H
