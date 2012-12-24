#ifndef DEBUGSHELL_H
#define DEBUGSHELL_H

#include <QMainWindow>
#include <QFile>
#include <QTimer>
#include <QTime>

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
    void on_pushButton_2_toggled(bool checked);
    void on_pushButton_clicked();
};

#endif // DEBUGSHELL_H
