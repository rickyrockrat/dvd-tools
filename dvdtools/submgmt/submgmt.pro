TEMPLATE	= app
LANGUAGE	= C++

CONFIG	+= qt warn_off debug

LIBS	+= -lfreetype

HEADERS	+= subtitle.h sublistboxitem.h

SOURCES	+= main.cpp subtitle.cpp sublistboxitem.cpp

FORMS	= wndsub.ui \
	genpngwnd.ui \
	editsub.ui

IMAGES	= fileopen.xpm \
	submgmt_icon.png

TARGET = submgmt






submgmt.path = /usr/bin
submgmt.files = submgmt

INSTALLS += submgmt

unix {
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
  QMAKE_CXXFLAGS += -g -Wall -W -pedantic -Wno-non-virtual-dtor -Wno-long-long
}


