#include <QtGui/QApplication>
#include "stealth.h"

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

int main(int argc, char *argv[])
{
    remove("debug_log.txt");
    qInstallMsgHandler(mMsgOut);
    QApplication a(argc, argv);
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("latin1"));
    Stealth w;
    w.show();

    return a.exec();
}
