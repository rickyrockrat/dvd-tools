TEMPLATE	= app
LANGUAGE	= C++

CONFIG	+= qt warn_on

SOURCES	+= main.cpp recwnd.cpp detailwnd.cpp

HEADERS += recwnd.h detailwnd.h

FORMS	= recwnd.ui detailwnd.ui

recorder.path = /usr/bin
recorder.files = recorder

INSTALLS += recorder

RESOURCES = recwnd.qrc

LIBS += -lQtNetwork

unix {
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
}


