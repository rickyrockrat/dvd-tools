TEMPLATE	= app
LANGUAGE	= C++

CONFIG	+= qt warn_on release

SOURCES	+= main.cpp

FORMS	= recwnd.ui

IMAGES	= pix/ic_open.png

recorder.path = /usr/bin
recorder.files = recorder

INSTALLS += recorder

unix {
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
}


