#-------------------------------------------------
#
# Project created by QtCreator 2011-12-08T18:20:34
#
#-------------------------------------------------

QT += core gui network

CONFIG += crypto

TARGET = Stealth
TEMPLATE = app


SOURCES += main.cpp\
        stealth.cpp \
    connectionmanager.cpp \
    connection.cpp \
    server.cpp \
    messagemanager.cpp \
    crypto.cpp

HEADERS  += stealth.h \
    connectionmanager.h \
    connection.h \
    server.h \
    messagemanager.h \
    crypto.h

FORMS    += stealth.ui

RESOURCES += \
    Resource.qrc

INCLUDEPATH += $$quote(qca-2.0.3/include/QtCrypto)













