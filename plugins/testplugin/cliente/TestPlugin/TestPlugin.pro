#-------------------------------------------------
#
# Project created by QtCreator 2012-11-29T12:17:15
#
#-------------------------------------------------

QT+=network
TARGET = testplugin
TEMPLATE = lib

DEFINES += TESTPLUGIN_LIBRARY

SOURCES += testplugin.cpp

HEADERS += testplugin.h\
        plugininterface.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE12644D8
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = TestPlugin.dll
    addFiles.path = !:/sys/bin
    DEPLOYMENT += addFiles
}

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

RESOURCES += \
    Resource.qrc

FORMS += \
    gui.ui
