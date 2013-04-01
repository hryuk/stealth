TARGET=pluginmanager
TEMPLATE = lib
#CONFIG += console
CONFIG -= qt

QMAKE_CXXFLAGS_RTTI_ON -= -frtti
QMAKE_CXXFLAGS_RTTI_OFF = -fno-rtti
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
LIBS += -lkernel32 -lWs2_32 -lmsvcrt -luser32 -lAdvapi32


SOURCES +=  rtl.cpp\
			main.cpp \
			RPEP.cpp \
			PluginManager.cpp \
			serializable.cpp \
			objectstream.cpp \
			darray.cpp


HEADERS +=	main.h  \
			RPEP.h \
			RPEP_Struc.h \
			PluginManager.h \
			serializable.h \
			objectstream.h \
			plugininterface.h \
			basicTypes.h \
			darray.h
