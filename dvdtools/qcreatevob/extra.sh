#!/bin/bash
#

KDE_dir=$(kde-config --prefix)

if test $KDE_dir; then
ICON_dir=$KDE_dir/share/icons
MENU_dir=$KDE_dir/share/applnk/Multimedia
else
  if test -d /usr/share/icons; then
  ICON_dir=/usr/share/icons
  else
  ICON_dir=\$\$prefix.path/share/icons
  fi

  if test -d /usr/share/applnk/; then
  MENU_dir=/usr/share/applnk/Multimedia
  else
  MENU_dir=\$\$prefix.path/share/applnk/Multimedia
  fi
fi

cat >extra.pri <<EOT
trans.files=translations/*.qm
trans.path=\$\$prefix.path/share/qcreatevob/lang
image.files=desktop/*.png
image.path=$ICON_dir
menu.files=desktop/*.desktop
menu.path=$MENU_dir
images.files=images/*
images.path=\$\$prefix.path/share/qcreatevob/images

INSTALLS += trans \
	image \
	menu \
	images
EOT
