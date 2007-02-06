TEMPLATE	= app
LANGUAGE	= C++

CONFIG	+= qt warn_off

HEADERS	+= common.h

SOURCES	+= main.cpp

FORMS	= encodewnd.ui \
	dvdmenuwnd.ui \
	newtitlewnd.ui \
	previewwnd.ui

IMAGES	= pix/ic_title.png \
	pix/ic_bgcolor.png \
	pix/ic_font.png \
	pix/ic_newdvd.png \
	pix/ic_preview.png \
	pix/ic_remove.png \
	pix/ic_video.png \
	pix/ic_exit.png \
	pix/ic_run.png \
	pix/ic_mencoder.png \
	pix/ic_directplay.png \
	pix/ic_new.png \
	pix/ic_open.png \
	pix/ic_save.png \
	pix/black.jpg

dvdmenu.path = /usr/bin
dvdmenu.files = dvdmenu

INSTALLS += dvdmenu




unix {
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
  QMAKE_CXXFLAGS = -g
}


