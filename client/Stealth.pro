#-------------------------------------------------
#
# Project created by QtCreator 2011-12-08T18:20:34
#
#-------------------------------------------------

QT+=core gui network crypto

TARGET=Stealth
TEMPLATE=app

SOURCES += main.cpp\
        gui/stealth.cpp \
    connection/connectionmanager.cpp \
    connection/connection.cpp \
    connection/server.cpp \
    connection/messagemanager.cpp \
    connection/datastream/crypto.cpp \
    gui/grouptreewidget.cpp \
    gui/customtreewidget.cpp \
    plugins/pluginmanager.cpp \
    gui/iteminfo.cpp \
    gui/pluginwindow.cpp \
    gui/debugshell.cpp \
    gui/stealthtab1.cpp \
    gui/stealthtab2.cpp \
    gui/stealthtab3.cpp \
    gui/stealthtab4.cpp \
    gui/slidingstackedwidget.cpp

HEADERS  += gui/stealth.h \
    connection/connectionmanager.h \
    connection/connection.h \
    connection/server.h \
    connection/messagemanager.h \
    connection/datastream/crypto.h \
    gui/grouptreewidget.h \
    gui/customtreewidget.h \
    plugins/pluginmanager.h \
    plugins/plugininterface.h \
    gui/iteminfo.h \
    gui/pluginwindow.h \
    gui/debugshell.h \
    gui/stealthtab1.h \
    gui/stealthtab2.h \
    gui/stealthtab3.h \
    gui/stealthtab4.h \
    gui/slidingstackedwidget.h

FORMS+= stealth.ui \
    gui/iteminfo.ui \
    gui/pluginwindow.ui \
    gui/debugshell.ui \
    gui/stealthtab1.ui \
    gui/stealthtab2.ui \
    gui/stealthtab3.ui \
    gui/stealthtab4.ui

RESOURCES += \
    Resource.qrc

INCLUDEPATH += $$quote(qca-2.0.3/include/QtCrypto)
INCLUDEPATH += $$quote(connection)
INCLUDEPATH += $$quote(connection/datastream)
INCLUDEPATH += $$quote(gui)
INCLUDEPATH += $$quote(plugins)

win32 {
    LIBS += -L$$quote(qca-2.0.3/lib) -lqca2
}
unix {
    LIBS += -L$$quote(qca-2.0.3/lib) -lqca
}





