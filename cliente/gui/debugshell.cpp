#include "debugshell.h"
#include "ui_debugshell.h"

DebugShell::DebugShell(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DebugShell)
{
    ui->setupUi(this);

    /* La ventana se eliminará de la memoria al cerrarse */
    setAttribute(Qt::WA_DeleteOnClose,true);

    /* Abrimos el archivo en el que se van guardando todos los mensajes
        de debug durante la ejecución */
    logFile=new QFile("debug_log.txt");
    if(!logFile->open(QIODevice::ReadOnly)) return;

    /* Cada segundo actualizamos el texto de la shell con los nuevos
        datos del archivo de texto */
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
        /* Obtenemos la hora actual */
        QTime time=QTime::currentTime();

        /* Leemos los datos nuevos del archivo de texto*/
        QString debug_data=QString::fromUtf8(logFile->readAll());
        if(debug_data.isEmpty()) return;

        if(ui->btnEnableLog->isChecked())
        {
            /* Procesamos cada linea, imprimiendola en un color según
                su tipo y con la fecha incluída */
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

void DebugShell::on_btnEnableLog_toggled(bool checked)
{
    if(checked)
    {
        ui->btnEnableLog->setIcon(QIcon(":/res/img/cancel.png"));
    }
    else
    {
        ui->btnEnableLog->setIcon(QIcon(":/res/img/tick.png"));
    }
}

void DebugShell::on_pushButton_clicked()
{
    ui->textEdit->clear();
}
