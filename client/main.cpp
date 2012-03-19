#include <QtGui/QApplication>
#include "stealth.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Stealth w;
    w.show();

    return a.exec();
}
