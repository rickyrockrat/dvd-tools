#!/bin/bash
motcle=$1
destfile=$2
baseurl=$3
wget -O videos $3
grep "new VideoListItem" videos |cut -f 4 -d "," | cut -f 2 -d"'" >liste.txt
grep $motcle liste.txt
if [ $? -eq 0 ]
then
	url=`grep $motcle liste.txt | head -1`
	wget -O page.html $url
	grep " videoUrl='" page.html
	if [ $? -eq 0 ]
	then
		vurl=`grep " videoUrl='" page.html | head -1 | cut -f 2 -d"'"`
		mplayer -dumpstream -dumpfile "$destfile" $vurl
	fi
fi
