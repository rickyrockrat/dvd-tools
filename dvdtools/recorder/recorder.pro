TEMPLATE	= app
LANGUAGE	= C++

CONFIG	+= debug qt warn_on

SOURCES	+= main.cpp recwnd.cpp detailwnd.cpp prefswnd.cpp

HEADERS += recwnd.h detailwnd.h prefswnd.h

FORMS	= recwnd.ui detailwnd.ui prefswnd.ui

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


