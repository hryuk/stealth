#ifndef STEALTHTAB4_H
#define STEALTHTAB4_H

#include <QWidget>

namespace Ui {
class StealthTab4;
}

class StealthTab4 : public QWidget
{
    Q_OBJECT
    
public:
    explicit StealthTab4(QWidget *parent = 0);
    ~StealthTab4();
    
private:
    Ui::StealthTab4 *ui;
};

#endif // STEALTHTAB4_H
