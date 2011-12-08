#ifndef STEALTH_H
#define STEALTH_H

#include <QMainWindow>

namespace Ui {
    class Stealth;
}

class Stealth : public QMainWindow
{
    Q_OBJECT

public:
    explicit Stealth(QWidget *parent = 0);
    ~Stealth();

private:
    Ui::Stealth *ui;
};

#endif // STEALTH_H
