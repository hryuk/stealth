#include "qterminal.h"

#include <QMessageBox>

QTerminal::QTerminal(QWidget *parent) :
    QTextEdit(parent)
{
    editionFlag=0;
    setContextMenuPolicy(Qt::NoContextMenu);
}

void QTerminal::keyPressEvent(QKeyEvent *e)
{
    if(textCursor().hasSelection())
    {
        if(e->modifiers()== Qt::CTRL)
        {
            if(e->matches(QKeySequence::Cut))
            {
                e->accept();
                if(!isSelectionEditable()) copy();
                else cut();
            }
            else if(e->matches(QKeySequence::Copy) )
            {
                e->accept();
                copy();
            }
        }
        return;
    }

    if(e->modifiers()==Qt::CTRL)
    {
        if(e->matches(QKeySequence::Undo)) return;
    }

    switch(e->key())
    {
        case Qt::Key_Return:
        case Qt::Key_Enter:
            emit returnPressed();
            this->insertOutput("\r\n");
        break;

        case Qt::Key_Backspace:
        case Qt::Key_Left:
            handleBack(e);
        break;

        case Qt::Key_Home:
            //handleHome();
        break;

        default:
            if (!isEditable())
            {
                moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
            }

            QTextEdit::keyPressEvent(e);
        break;
    }
}

void QTerminal::dropEvent(QDropEvent* event)
{
    if(isEditable())
    {
        QTextEdit::dropEvent(event);
    }
}

void QTerminal::handleBack(QKeyEvent* event)
{
    if(isEditable())
    {
        QTextEdit::keyPressEvent(event);
    }
}

QString QTerminal::getCommand()
{
    QTextCursor c=textCursor();
    c.setPosition(editionFlag,QTextCursor::KeepAnchor);
    return c.selectedText();
}

void QTerminal::insertOutput(QString text)
{
    this->insertPlainText(text);

    QTextCursor tc=this->textCursor();
    tc.movePosition(QTextCursor::End);
    this->setTextCursor(tc);

    this->editionFlag=this->textCursor().position();
}

bool QTerminal::isEditable()
{
    if(textCursor().position()>editionFlag) return true;
    else return false;
}

bool QTerminal::isSelectionEditable()
{
    int pos1=textCursor().selectionStart();
    int pos2=textCursor().selectionEnd();

    if(pos1>=editionFlag && pos2>=editionFlag) return true;
    else return false;
}

void QTerminal::paste()
{
    if(isEditable()) QTextEdit::paste();
}

void QTerminal::cut()
{
    if(textCursor().hasSelection() && isSelectionEditable()) QTextEdit::cut();
}
