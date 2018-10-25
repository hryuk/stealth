QT+=core gui network widgets

#Para deshabilitar los mensajes de debug
#CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT

TARGET=Stealth
TEMPLATE=app

Release:DESTDIR = ../release
Release:OBJECTS_DIR = ../release/.obj-cliente
Release:MOC_DIR = ../release/.moc-cliente
Release:RCC_DIR = ../release/.rcc-cliente
Release:UI_DIR = ../release/.ui-cliente

Debug:DESTDIR = ../debug
Debug:OBJECTS_DIR = ../debug/.obj-cliente
Debug:MOC_DIR = ../debug/.moc-cliente
Debug:RCC_DIR = ../debug/.rcc-cliente
Debug:UI_DIR = ../debug/.ui-cliente

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
    LIBS += -L$$quote($$_PRO_FILE_PWD_/qca-2.0.3/lib) -lqca2
}
unix {
    LIBS += -L$$quote(qca-2.0.3/lib) -lqca-qt5
}
