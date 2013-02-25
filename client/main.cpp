#include <QtGui/QApplication>
#include "stealth.h"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4996)
#endif

/* Todos los qDebug, qWarning, qCritical y qFatal van a parar aquí */
void mMsgOut(QtMsgType type, const char *msg)
{
    FILE* hFile=fopen("debug_log.txt","a+");

    switch (type)
    {
        case QtDebugMsg:
            fprintf(hFile, "Debug: %s\n",msg);
            break;
        case QtWarningMsg:
            fprintf(hFile, "Warning: %s\n",msg);
            break;
        case QtCriticalMsg:
            fprintf(hFile, "Critical: %s\n",msg);
            break;
        case QtFatalMsg:
            fprintf(hFile, "Fatal: %s\n",msg);
            fclose(hFile);
            abort();
    }
    fclose(hFile);
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif

int main(int argc, char *argv[])
{
    /* Eliminamos el log anterior que pueda haber */
    remove("debug_log.txt");

    /* Indicamos la función que manejará los qDebug, QWarning, etc */
    qInstallMsgHandler(mMsgOut);

    QApplication a(argc, argv);
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("latin1"));
    Stealth w;
    w.show();

    return a.exec();
}
