#ifndef STEALTHTAB3_H
#define STEALTHTAB3_H

#include <QWidget>

namespace Ui {
class StealthTab3;
}

class StealthTab3 : public QWidget
{
    Q_OBJECT
    
public:
    explicit StealthTab3(QWidget *parent = 0);
    ~StealthTab3();
    
private:
    Ui::StealthTab3 *ui;
};

#endif // STEALTHTAB3_H
