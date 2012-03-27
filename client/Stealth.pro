#-------------------------------------------------
#
# Project created by QtCreator 2011-12-08T18:20:34
#
#-------------------------------------------------

QT += core gui network

TARGET = Stealth
TEMPLATE = app


SOURCES += main.cpp\
        stealth.cpp \
    connectionmanager.cpp \
    connection.cpp \
    server.cpp \
    messagemanager.cpp \
    crypto.cpp \
    iteminfo.cpp \
    grouptreewidget.cpp \
    customtreewidget.cpp

HEADERS  += stealth.h \
    connectionmanager.h \
    connection.h \
    server.h \
    messagemanager.h \
    crypto.h \
    iteminfo.h \
    grouptreewidget.h \
    customtreewidget.h

FORMS    += stealth.ui \
    iteminfo.ui

RESOURCES += \
    Resource.qrc

INCLUDEPATH += $$quote(qca-2.0.3/include/QtCrypto)

LIBS += -L$$quote(qca-2.0.3/lib) -lqca2
