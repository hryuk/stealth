#include <QtWidgets/QApplication>
#include "stealth.h"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4996)
#endif

/* Todos los qDebug, qWarning, qCritical y qFatal van a parar aquí */
void mMsgOut(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    FILE* hFile=fopen("debug_log.txt","a+");
    QByteArray localMsg = msg.toLocal8Bit();

    switch (type)
    {
        case QtDebugMsg:
            fprintf(hFile, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
            break;
        case QtInfoMsg:
            fprintf(hFile, "Info: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
            break;
        case QtWarningMsg:
            fprintf(hFile, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
            break;
        case QtCriticalMsg:
            fprintf(hFile, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
            break;
        case QtFatalMsg:
            fprintf(hFile, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
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
    qInstallMessageHandler(mMsgOut);

    QApplication a(argc, argv);
    Stealth w;
    w.show();

    return a.exec();
}
