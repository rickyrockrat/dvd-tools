TEMPLATE	= app
LANGUAGE	= C++

CONFIG	+= qt warn_off debug

LIBS	+= -lkdeui -lfreetype

DEFINES	+= KDE_SUBMGMT

HEADERS	+= subtitle.h

SOURCES	+= main.cpp

FORMS	= wndsub.ui

IMAGES	= fileopen.xpm \
	submgmt_icon.png

unix {
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
  QMAKE_CXXFLAGS += -g -Wall -W -pedantic -Wno-non-virtual-dtor -Wno-long-long
}


