TARGET=pluginmanager
TEMPLATE = lib
#CONFIG += console
CONFIG -= qt

if win32{
   QMAKE_CXXFLAGS += -std=gnu++0x
}
else {
	QMAKE_CXXFLAGS += -std=c++0x
	QMAKE_INCDIR += /usr/i686-w64-mingw32/sys-root/mingw/include/
	DEFINES +=_WIN32
}

DEFINES +=BUILD_DLL _WIN32

#LIBS += -L$$quote(../ArkLib) -lArkLib -lntdll -lWs2_32 --static
LIBS += -L$$quote(../ArkLib) -lntdll -lWs2_32


SOURCES +=	main.cpp \
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
