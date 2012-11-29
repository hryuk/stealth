#-------------------------------------------------
#
# Project created by QtCreator 2011-12-08T18:20:34
#
#-------------------------------------------------

QT+=core gui network

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
    gui/pluginwindow.cpp

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
    gui/pluginwindow.h

FORMS+= stealth.ui \
    gui/iteminfo.ui \
    gui/pluginwindow.ui

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





