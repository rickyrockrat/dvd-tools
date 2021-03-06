TEMPLATE	= app
LANGUAGE	= C++

CONFIG	+= qt warn_off

HEADERS	+= trackpreview.h \
	common.h

SOURCES	+= main.cpp \
	trackpreview.cpp

FORMS	= encodewnd.ui \
	dvdaudiownd.ui

IMAGES	= pix/ic_album.png \
	pix/ic_bgcolor.png \
	pix/ic_gen.png \
	pix/ic_font.png \
	pix/ic_newdvd.png \
	pix/ic_preview.png \
	pix/ic_remove.png \
	pix/ic_track.png \
	pix/ic_exit.png \
	pix/ic_run.png \
	pix/ic_mencoder.png \
	pix/ic_directplay.png \
	pix/ic_new.png \
	pix/ic_open.png \
	pix/ic_up.png \
	pix/ic_down.png \
	pix/ic_save.png

dvdaudio.path = /usr/bin
dvdaudio.files = dvdaudio

INSTALLS += dvdaudio




unix {
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
  QMAKE_CXXFLAGS = -g
}


