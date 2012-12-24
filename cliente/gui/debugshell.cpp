#include "debugshell.h"
#include "ui_debugshell.h"

DebugShell::DebugShell(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DebugShell)
{
    ui->setupUi(this);

    setAttribute(Qt::WA_DeleteOnClose,true);

    logFile=new QFile("debug_log.txt");
    if(!logFile->open(QIODevice::ReadOnly)) return;

    updateLog();

    timer.setInterval(1000);
    timer.setSingleShot(false);
    connect(&timer,SIGNAL(timeout()),this,SLOT(updateLog()));
    timer.start();
}

DebugShell::~DebugShell()
{
    logFile->close();
    delete logFile;
    delete ui;
}

void DebugShell::updateLog()
{

        QTime time=QTime::currentTime();
        QString debug_data=logFile->readAll();

        if(ui->pushButton_2->isChecked())
        {
            QStringList lines=debug_data.split("\n");
            foreach(QString line,lines)
            {
                if(line.startsWith("Debug:")) ui->textEdit->append("<font color=\"#B3FFB3\">[i]["+time.toString()+"] "+line.replace("Debug: ","").replace("\"","")+"</font>");
                else if(line.startsWith("Critical:")) ui->textEdit->append("<font color=\"#E60039\">[x]["+time.toString()+"] "+line.replace("Critical: ","").replace("\"","")+"</font>");
                else if(line.startsWith("Warning:")) ui->textEdit->append("<font color=\"#FFD147\">[!]["+time.toString()+"] "+line.replace("Warning: ","").replace("\"","")+"</font>");
                else if(line.startsWith("Fatal:")) ui->textEdit->append("<font color=\"red\">[X]["+time.toString()+"] "+line.replace("Fatal: ","").replace("\"","")+"</font>");
            }
        }
}

void DebugShell::on_pushButton_2_toggled(bool checked)
{
    if(checked)
    {
        ui->pushButton_2->setIcon(QIcon(":/res/img/cancel.png"));
    }
    else
    {
        ui->pushButton_2->setIcon(QIcon(":/res/img/tick.png"));
    }
}

void DebugShell::on_pushButton_clicked()
{
    ui->textEdit->clear();
}
