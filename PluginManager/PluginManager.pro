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

LIBS += -L$$quote(../ArkLib) -lArkLib -lntdll -lWs2_32 --static


SOURCES +=	main.cpp \
			RPEP.cpp \
			PluginManager.cpp \
	serializable.cpp \
	objectstream.cpp

HEADERS +=	main.h  \
			RPEP.h \
			RPEP_Struc.h \
			PluginManager.h \
	serializable.h \
	objectstream.h \
	plugininterface.h
