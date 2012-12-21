TEMPLATE = lib
#CONFIG += console
CONFIG -= qt

QMAKE_CXXFLAGS_RTTI_ON -= -frtti
QMAKE_CXXFLAGS_RTTI_OFF = -fno-rtti
QMAKE_CXXFLAGS += -nostdlib -fno-exceptions -fno-rtti

if win32{
   QMAKE_CXXFLAGS += -std=gnu++0x
}
else {
	QMAKE_CXXFLAGS += -std=c++0x
	DEFINES +=_WIN32
}

DEFINES +=BUILD_DLL _WIN32
DEFINES -= UNICODE QT_LARGEFILE_SUPPORT

QMAKE_LFLAGS +=  -nostartfiles -fno-exceptions -nostdlib

#LIBS += -L$$quote(../ArkLib) -lArkLib -lntdll -lWs2_32 --static
LIBS += -lntdll -lkernel32 -lWs2_32 -lmsvcrt -luser32 -lAdvapi32

SOURCES += \
	../PluginManager/rtl.cpp \
	testplugin.cpp \
	main.cpp

HEADERS += ../PluginManager/plugininterface.h\
	testplugin.h \
	main.h

