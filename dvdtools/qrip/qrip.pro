TARGET = qrip

TEMPLATE	= app
LANGUAGE	= C++

CONFIG	+= qt warn_off
CONFIG	+= debug

HEADERS	+= \
	qripcddb.h \
	wndQrip.h

SOURCES	+= main.cpp \
	qripcddb.cpp \
	wndQrip.cpp

FORMS	= \
	wndQrip.ui

RESOURCES = qrip.qrc

target.path = /usr/bin

INSTALLS += target

LIBS = -lcdio -lcddb

INCLUDEPATH += cdio

unix {
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
}


