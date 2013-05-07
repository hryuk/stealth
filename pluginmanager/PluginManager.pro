TARGET=pluginmanager
TEMPLATE =lib
#CONFIG += console
CONFIG -= qt

DEFINES += DEBUG

Release:DESTDIR = ../release
Release:OBJECTS_DIR = ../release/.obj-pluginmanager
Release:MOC_DIR = ../release/.moc-pluginmanager
Release:RCC_DIR = ../release/.rcc-pluginmanager
Release:UI_DIR = ../release/.ui-pluginmanager

Debug:DESTDIR = ../debug
Debug:OBJECTS_DIR = ../debug/.obj-pluginmanager
Debug:MOC_DIR = ../debug/.moc-pluginmanager
Debug:RCC_DIR = ../debug/.rcc-pluginmanager
Debug:UI_DIR = ../debug/.ui-pluginmanager

QMAKE_CXXFLAGS_RTTI_ON -= -frtti
QMAKE_CXXFLAGS_RTTI_OFF = -fno-rtti

QMAKE_CFLAGS_RELEASE = /GR-
QMAKE_CXXFLAGS = /GS- /GR- /QIfist
QMAKE_LFLAGS += /SAFESEH:NO
#QMAKE_CXXFLAGS += -nostdlib -fno-exceptions -fno-rtti -fno-omit-frame-pointer

win32 {
		  build_pass:CONFIG(debug, debug|release) {
			message("debug")
		 } else {
			message("release")
		 }
}


DEFINES +=BUILD_DLL _WIN32
DEFINES -= UNICODE QT_LARGEFILE_SUPPORT

#QMAKE_LFLAGS +=  -nostartfiles -fno-exceptions -nostdlib

#LIBS += -L$$quote(../ArkLib) -lArkLib -lntdll -lWs2_32 --static
LIBS += -lkernel32 -lWs2_32 -luser32 -lAdvapi32


SOURCES +=  rtl.cpp\
			main.cpp \
			RPEP.cpp \
			PluginManager.cpp \
			darray.cpp \
	arraylist.cpp


HEADERS +=	main.h  \
			RPEP.h \
			RPEP_Struc.h \
			PluginManager.h \
			plugininterface.h \
			basicTypes.h \
			darray.h \
	arraylist.h
