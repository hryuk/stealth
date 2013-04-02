#ifndef QTERMINAL_H
#define QTERMINAL_H

#include <QTextEdit>
#include <QKeyEvent>
#include <QDropEvent>

class QTerminal : public QTextEdit
{
    Q_OBJECT
public:
    explicit QTerminal(QWidget *parent = 0);
    void insertOutput(QString text);
    QString getCommand();

private:
    int editionFlag;
    void handleBack(QKeyEvent *event);
    void handleEnter(QKeyEvent* event);
    bool isEditable();
    bool isSelectionEditable();

protected:
    void keyPressEvent(QKeyEvent* e);
    void dropEvent(QDropEvent* event);
    void paste();
    void cut();

signals:
    void returnPressed();
    
};

#endif // QTERMINAL_H
