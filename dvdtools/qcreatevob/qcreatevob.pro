TEMPLATE	= app
LANGUAGE	= C++

CONFIG	+= release qt warn_off thread x11

LIBS	+= -lfreetype

INCLUDEPATH	+= . src

HEADERS	+= src/checkclass.h \
	src/scr.h \
	src/snd.h \
	src/strings.h \
	src/tablebutton.h

SOURCES	+= src/checkclass.cpp \
	src/main.cpp \
	src/scr.cpp \
	src/snd.cpp \
	src/tablebutton.cpp

FORMS	= src/border-selector.ui \
	src/mplexingclass.ui \
	src/processdlg.ui \
	src/qcreatevob.ui \
	src/scriptedit.ui \
	src/scriptlist.ui \
	src/mydoublebox.ui

IMAGES	= images/splash.png

# ???? ?????? ? KDevelop ?????????? qmake.
# ------------------------------------------- 
# ?????????? ???????????? ???????? ???????? ???????: .
# ???? - ??????????:  qcreatevob

TRANSLATIONS += translations/qcreatevob_ru.ts \
                translations/qcreatevob_de.ts 
DEPENDPATH = src
TARGETDEPS += /bin/bash
TARGET = qcreatevob
QMAKE_CXXFLAGS += -Wall
QMAKE_CXXFLAGS += -Wno-non-virtual-dtor
include(conf.pri)
include(extra.pri)
