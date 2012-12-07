TEMPLATE = lib
#CONFIG += console
CONFIG -= qt

if win32{
   QMAKE_CXXFLAGS += -std=gnu++0x
}
else {
   QMAKE_CXXFLAGS += -std=c++0x
}

DEFINES +=BUILD_DLL

SOURCES += main.cpp \
	testplugin.cpp

HEADERS += ../PluginManager/plugininterface.h\
	testplugin.h

