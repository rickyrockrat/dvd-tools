TEMPLATE	= app
LANGUAGE	= C++

CONFIG	+= qt warn_off debug

LIBS	+= -lfreetype

HEADERS	+= subtitle.h \
	submodel.h \
	subitemdelegate.h \
	subeditor.h \
	util.h \
	qt4subeditor.h

SOURCES	+= main_se.cpp \
	subtitle.cpp \
	submodel.cpp \
	subitemdelegate.cpp \
	subeditor.cpp \
	util.cpp \
	qt4subeditor.cpp

FORMS	= qt4subeditor.ui

TARGET = subeditor

RESOURCES = subeditor.qrc




subeditor.path = /usr/bin
subeditor.files = subeditor

INSTALLS += subeditor

unix {
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
  QMAKE_CXXFLAGS += -g -Wall -W -pedantic -Wno-non-virtual-dtor -Wno-long-long
}


