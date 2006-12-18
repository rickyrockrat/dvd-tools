trans.files=translations/*.qm
trans.path=$$prefix.path/share/qcreatevob/lang
image.files=desktop/*.png
image.path=/usr/share/icons
menu.files=desktop/*.desktop
menu.path=/usr/share/applnk/Multimedia
images.files=images/*
images.path=$$prefix.path/share/qcreatevob/images

INSTALLS += trans 	image 	menu 	images
