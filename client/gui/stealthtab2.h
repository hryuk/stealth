#ifndef STEALTHTAB2_H
#define STEALTHTAB2_H

#include <QWidget>

namespace Ui {
class StealthTab2;
}

class StealthTab2 : public QWidget
{
    Q_OBJECT
    
public:
    explicit StealthTab2(QWidget *parent = 0);
    ~StealthTab2();
    
private:
    Ui::StealthTab2 *ui;
};

#endif // STEALTHTAB2_H
