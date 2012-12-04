#include "debugshell.h"
#include "ui_debugshell.h"

DebugShell::DebugShell(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DebugShell)
{
    ui->setupUi(this);

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

    QStringList lines=debug_data.split("\n");
    foreach(QString line,lines)
    {
        if(line.startsWith("Debug:")) ui->textEdit->append("<font color=\"blue\">[i]["+time.toString()+"] "+line.replace("Debug: ","").replace("\"","")+"</font>");
        else if(line.startsWith("Critical:")) ui->textEdit->append("<font color=\"orange\">[x]["+time.toString()+"] "+line.replace("Critical: ","").replace("\"","")+"</font>");
        else if(line.startsWith("Warning:")) ui->textEdit->append("<font color=\"yellow\">[!]["+time.toString()+"] "+line.replace("Warning: ","").replace("\"","")+"</font>");
        else if(line.startsWith("Fatal:")) ui->textEdit->append("<font color=\"red\">[X]["+time.toString()+"] "+line.replace("Fatal: ","").replace("\"","")+"</font>");
    }
}
