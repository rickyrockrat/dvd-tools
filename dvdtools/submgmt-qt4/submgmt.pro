TEMPLATE	= app
LANGUAGE	= C++

CONFIG	+= qt warn_off debug

LIBS	+= -lfreetype

HEADERS	+= subtitle.h \
	submodel.h \
	subitemdelegate.h \
	subeditor.h \
	qt4wndsub.h \
	qt4editsub.h \
	qt4genpngwnd.h

SOURCES	+= main.cpp \
	subtitle.cpp \
	submodel.cpp \
	subitemdelegate.cpp \
	subeditor.cpp \
	qt4wndsub.cpp \
	qt4editsub.cpp \
	qt4genpngwnd.cpp

FORMS	= qt4editsub.ui qt4genpngwnd.ui qt4wndsub.ui

TARGET = submgmt

RESOURCES = submgmt.qrc




submgmt.path = /usr/bin
submgmt.files = submgmt

INSTALLS += submgmt

unix {
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
  QMAKE_CXXFLAGS += -g -Wall -W -pedantic -Wno-non-virtual-dtor -Wno-long-long
}


