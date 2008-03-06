TEMPLATE	= app
LANGUAGE	= C++

CONFIG	+= qt warn_off

HEADERS	+= common.h \
	qt4encodewnd.h \
	qt4dvdmenuwnd.h \
	qt4newtitlewnd.h \
	qt4previewwnd.h

SOURCES	+= main.cpp \
	qt4encodewnd.cpp \
	qt4dvdmenuwnd.cpp \
	qt4newtitlewnd.cpp

FORMS	= qt4encodewnd.ui \
	qt4dvdmenuwnd.ui \
	qt4newtitlewnd.ui \
	qt4previewwnd.ui

RESOURCES = dvdmenu.qrc

dvdmenu.path = /usr/bin
dvdmenu.files = dvdmenu

INSTALLS += dvdmenu

LIBS += -lQtXml




unix {
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
  QMAKE_CXXFLAGS = -g
}


