TEMPLATE	= app
LANGUAGE	= C++

CONFIG	+= qt warn_off debug

TARGET = submgmt

LIBS	+= -lkdeui -lfreetype

HEADERS	+= subtitle.h

SOURCES	+= main.cpp

FORMS	= wndsub.ui

IMAGES	= fileopen.xpm \
	submgmt_icon.png

submgmt.path = /usr/bin
submgmt.files = submgmt

INSTALLS += submgmt

unix {
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
  QMAKE_CXXFLAGS += -g -Wall -W -pedantic -Wno-non-virtual-dtor -Wno-long-long
}


