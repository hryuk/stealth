#ifndef DEBUGSHELL_H
#define DEBUGSHELL_H

#include <QMainWindow>
#include <QFile>
#include <QTimer>

namespace Ui {
class DebugShell;
}

class DebugShell : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit DebugShell(QWidget *parent = 0);
    ~DebugShell();
    
private:
    Ui::DebugShell *ui;
    QFile* logFile;
    QTimer timer;

private slots:
    void updateLog();
};

#endif // DEBUGSHELL_H
