TARGET=remoteshell_server
DESTDIR=../cliente/res
TEMPLATE = lib
#CONFIG += console
CONFIG -= qt

QMAKE_CXXFLAGS_RTTI_ON -= -frtti
QMAKE_CXXFLAGS_RTTI_OFF = -fno-rtti
#QMAKE_CXXFLAGS += -nostdlib -fno-exceptions -fno-rtti -fno-omit-frame-pointer -std=gnu++0x


DEFINES +=BUILD_DLL _WIN32
DEFINES -= UNICODE QT_LARGEFILE_SUPPORT

#QMAKE_LFLAGS +=  -nostartfiles -fno-exceptions -nostdlib -entry=__start@12

#LIBS += -L$$quote(../ArkLib) -lArkLib -lntdll -lWs2_32 --static
LIBS += -lkernel32 -lWs2_32 -lmsvcrt -luser32 -lAdvapi32

SOURCES +=\
        rtl.cpp \
	main.cpp \
	remoteshell.cpp

HEADERS += \
        plugininterface.h\
	main.h \
        remoteshell.h \
        basicTypes.h

