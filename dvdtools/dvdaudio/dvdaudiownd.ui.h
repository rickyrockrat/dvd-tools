/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you want to add, delete, or rename functions or slots, use
** Qt Designer to update this file, preserving your code.
**
** You should not define a constructor or destructor in this file.
** Instead, write your code in functions called init() and destroy().
** These will automatically be called by the form's constructor and
** destructor.
*****************************************************************************/
//

#include "common.h"
#include <iostream>
#include <unistd.h>	// pour sleep

void dvdaudioWnd::init()
{
	lvDVD->clear();
	lvDVD->setSorting(-1);
	dvdItem = 0;
	albumItem = 0;
	trackItem = 0;
	procEncode = 0;
	mpginfoProc = 0;
	bgcolor = Qt::black;
	albumCtr = 0;
	trackDir = QDir::currentDirPath();
	font = QFont( "Bitstream Vera Sans Mono", 28 );

	aAddAlbum->setIconSet( QIconSet( QPixmap::fromMimeSource( "ic_album.png" ) ) );
	aAddTracks->setIconSet( QIconSet( QPixmap::fromMimeSource( "ic_track.png" ) ) );
	aBgColor->setIconSet( QIconSet( QPixmap::fromMimeSource( "ic_bgcolor.png" ) ) );
	aDirectPlay->setIconSet( QIconSet( QPixmap::fromMimeSource( "ic_directplay.png" ) ) );
	aEncode->setIconSet( QIconSet( QPixmap::fromMimeSource( "ic_run.png" ) ) );
	aExit->setIconSet( QIconSet( QPixmap::fromMimeSource( "ic_exit.png" ) ) );
	aFont->setIconSet( QIconSet( QPixmap::fromMimeSource( "ic_font.png" ) ) );
	aMEncoder->setIconSet( QIconSet( QPixmap::fromMimeSource( "ic_mencoder.png" ) ) );
	aNew->setIconSet( QIconSet( QPixmap::fromMimeSource( "ic_new.png" ) ) );
	aNewDVD->setIconSet( QIconSet( QPixmap::fromMimeSource( "ic_newdvd.png" ) ) );
	aOpen->setIconSet( QIconSet( QPixmap::fromMimeSource( "ic_open.png" ) ) );
	aPreview->setIconSet( QIconSet( QPixmap::fromMimeSource( "ic_preview.png" ) ) );
	aRemove->setIconSet( QIconSet( QPixmap::fromMimeSource( "ic_remove.png" ) ) );
	aSave->setIconSet( QIconSet( QPixmap::fromMimeSource( "ic_save.png" ) ) );
	aGenOnly->setIconSet( QIconSet( QPixmap::fromMimeSource( "ic_gen.png" ) ) );

	connect( aNew, SIGNAL(activated()), this, SLOT(newProject()) );
	connect( aOpen, SIGNAL(activated()), this, SLOT(openProject()) );
	connect( aSave, SIGNAL(activated()), this, SLOT(saveProject()) );

	connect( aAddAlbum, SIGNAL(activated()), this, SLOT(newAlbum()) );
	connect( aAddTracks, SIGNAL(activated()), this, SLOT(addTracks()) );
	connect( aBgColor, SIGNAL(activated()), this, SLOT(selectBgColor()) );
	connect( aEncode, SIGNAL(activated()), this, SLOT(encode()) );
	connect( aExit, SIGNAL(activated()), this, SLOT(close()) );
	connect( aFont, SIGNAL(activated()), this, SLOT(selectFont()) );
	connect( aNewDVD, SIGNAL(activated()), this, SLOT(newDVD()) );
	connect( aPreview, SIGNAL(activated()), this, SLOT(preview()) );
	connect( aRemove, SIGNAL(activated()), this, SLOT(removeItem()) );
	connect( pbMenuBg, SIGNAL(clicked()), this, SLOT(selectBgPic()) );
	connect( lvDVD, SIGNAL(clicked(QListViewItem *)),
	this, SLOT(lvClicked(QListViewItem*)));

	pbOccup->setTotalSteps( 4400000 );
}

void dvdaudioWnd::newDVD()
{
	lvDVD->clear();
	lvDVD->addColumn( "ident", 0 );
	lvDVD->addColumn( "duration" );
	lvDVD->addColumn( "artist" );
	lvDVD->addColumn( "album" );
	lvDVD->addColumn( "track" );
	lvDVD->addColumn( "bitrate" );
	lvDVD->addColumn( "sample" );
	lvDVD->addColumn( "year" );
	lvDVD->addColumn( "genre" );
	lvDVD->addColumn( "filename", 0 );

	lvDVD->setRootIsDecorated( true );

	dvdItem = new QListViewItem( lvDVD, "New DVD Audio", CC_DVD );
	dvdItem->setRenameEnabled( 0, true );
	dvdItem->setOpen( true );

	workingDir = QFileDialog::getExistingDirectory(
			QDir::currentDirPath(), this, "tempdir",
			"Choose the temporary directory" );
}

void dvdaudioWnd::newAlbum()
{
	QString a;
	QString s = QFileDialog::getExistingDirectory(
		trackDir,
		this,
		"get existing directory",
		"Choose a directory",
		true );
	if ( s.isNull() )
	{
		a.sprintf( "Album %d", ++albumCtr );
	}
	else
	{
		a = s.section( '/', -2 );
	}
	if ( albumItem )
		albumItem = new QListViewItem( dvdItem, albumItem, a, CC_ALBUM );
	else
		albumItem = new QListViewItem( dvdItem, a, CC_ALBUM );
	albumItem->setRenameEnabled( 0, true );

	if ( !s.isNull() )
	{
		QDir d( s );
		QStringList l = d.entryList("*.mp3", QDir::Files);
		QString filename;
		for ( int i = 0; i < l.count(); i++ )
		{
			filename = d.absPath() + "/" + l[i];
			mpginfoProc = new QProcess(this );
			connect( mpginfoProc, SIGNAL(readyReadStdout()),
						this, SLOT(readProcMplayer()));
			mpginfoProc->addArgument( "mplayer" );
			mpginfoProc->addArgument( "-vo" );
			mpginfoProc->addArgument( "null" );
			mpginfoProc->addArgument( "-ao" );
			mpginfoProc->addArgument( "null" );
			mpginfoProc->addArgument( "-frames" );
			mpginfoProc->addArgument( "0" );
			mpginfoProc->addArgument( "-identify" );
			mpginfoProc->addArgument( filename );
			mpginfoProc->start();

			/*
			connect( mpginfoProc, SIGNAL(readyReadStdout()),
						this, SLOT(readProcMpginfo()));
			mpginfoProc->addArgument( "mpginfo" );
			mpginfoProc->addArgument( filename );
			*/
			mpginfoProc->start();

			while ( mpginfoProc->isRunning() )
			{
				sleep( 2 );
			}
			if ( !mpginfoProc->normalExit() )
			{
				QMessageBox::information( this, "Problem",
					"mpginfo error for file " + filename );
			}
			else
			{
				trackItem->setText( ID_FILENAME, filename );
				if ( trackItem->text( ID_NAME ).isNull() )
					trackItem->setText( ID_NAME, l[i] );
			}
			delete mpginfoProc;
			QFileInfo fi( filename );
			pbOccup->setProgress( pbOccup->progress()+int(fi.size()*1.2/1024));
		}
	}
}

void dvdaudioWnd::lvClicked( QListViewItem *item )
{
	if ( item )
	{
		if ( item->text( ID_IDENT ) == CC_ALBUM )
			albumItem = item;
		if ( item->text( ID_IDENT ) == CC_TRACK )
		{
			trackItem = item;
			aPreview->setEnabled( true );
		}
		else
		{
			aPreview->setEnabled( false );
		}
		aRemove->setEnabled( true );
	}
	else
	{
		aPreview->setEnabled( false );
		aRemove->setEnabled( false );
	}
}

void dvdaudioWnd::addTracks()
{
	QFileDialog fd( trackDir,
					"*.mp3",
					this,
					"Add tracks", true );
	fd.setMode( QFileDialog::ExistingFiles );
	if ( fd.exec() == QDialog::Accepted )
	{
		trackDir = fd.dir()->absPath();
		QStringList l = fd.selectedFiles();
		QStringList::Iterator it;
		for ( it = l.begin(); it != l.end(); it++ )
		{
			mpginfoProc = new QProcess(this );
			/* ID_LENGTH reported by mplayer is also wrong...
			connect( mpginfoProc, SIGNAL(readyReadStdout()),
						this, SLOT(readProcMplayer()));
			mpginfoProc->addArgument( "mplayer" );
			mpginfoProc->addArgument( "-vo" );
			mpginfoProc->addArgument( "null" );
			mpginfoProc->addArgument( "-ao" );
			mpginfoProc->addArgument( "null" );
			mpginfoProc->addArgument( "-frames" );
			mpginfoProc->addArgument( "0" );
			mpginfoProc->addArgument( "-identify" );
			mpginfoProc->addArgument( *it );
			*/

			connect( mpginfoProc, SIGNAL(readyReadStdout()),
						this, SLOT(readProcMpginfo()));
			mpginfoProc->addArgument( "mpginfo" );
			mpginfoProc->addArgument( *it );

			mpginfoProc->start();
			while ( mpginfoProc->isRunning() )
			{
			}
			if ( !mpginfoProc->normalExit() )
			{
				QMessageBox::information( this, "Problem",
					/*"mplayer identify error for file " + *it );*/
					"mpginfo error for file " + *it );
			}
			else
			{
				trackItem->setText( ID_FILENAME, *it );
			}
			delete mpginfoProc;

			QFileInfo fi( *it );
			pbOccup->setProgress( pbOccup->progress()+int(fi.size()*1.2/1024) );

		}
	}
}

void dvdaudioWnd::selectBgColor()
{
	bgcolor = QColorDialog::getColor();
}

void dvdaudioWnd::selectFont()
{
	bool ok;
	font = QFontDialog::getFont(&ok, font );
}

void dvdaudioWnd::newProject()
{
}

void dvdaudioWnd::openProject()
{

	QString s = QFileDialog::getOpenFileName(
                    workingDir,
                    "Xml (*.xml)",
                    this,
                    "open file dialog",
                    "Choose a file to open" );
 	if ( !s.isNull() )
	{
		QFile f( s );
		if ( !f.open( IO_ReadOnly ) )
		{
			QMessageBox::information( this, "error", "Can't open file");
			return;
		}
		QDomDocument doc;
		QString errs;
		int errnl, errnc;
		if ( doc.setContent( &f, false, &errs, &errnl, &errnc ) )
		{
			f.close();
			lvDVD->clear();
			QDomElement docElem = doc.documentElement();
			QDomAttr dvdname = docElem.attributeNode( "NAME" );
			//std::cout << "dvd=" << dvdname.value() << std::endl;
			lvDVD->addColumn( "ident", 0 );
			lvDVD->addColumn( "duration" );
			lvDVD->addColumn( "artist" );
			lvDVD->addColumn( "album" );
			lvDVD->addColumn( "track" );
			lvDVD->addColumn( "bitrate" );
			lvDVD->addColumn( "sample" );
			lvDVD->addColumn( "year" );
			lvDVD->addColumn( "genre" );
			lvDVD->addColumn( "filename", 0 );

			lvDVD->setRootIsDecorated( true );
			dvdItem = new QListViewItem( lvDVD, dvdname.value(), CC_DVD );
			dvdItem->setOpen(true);
			dvdItem->setRenameEnabled( ID_NAME, true );

			QDomNode n = docElem.firstChild();
			while( !n.isNull() )
			{
				QDomElement e = n.toElement(); 
				if( !e.isNull() )
				{
					if ( e.tagName() == "ALBUM" )
					{
						QDomAttr albumname = e.attributeNode( "NAME" );
						//std::cout << "album=" << albumname.value() << std::endl; 
						if ( albumItem )
							albumItem = new QListViewItem( dvdItem, albumItem, albumname.value(), CC_ALBUM );
						else
							albumItem = new QListViewItem( dvdItem, albumname.value(), CC_ALBUM );
						albumItem->setRenameEnabled( ID_NAME, true );
						QDomNode n1 = e.firstChild();
						while( !n1.isNull() )
						{
							QDomElement e1 = n1.toElement(); 
							if( !e1.isNull() )
							{
								if ( e1.tagName() == "TRACK" )
								{
									QDomAttr att = e1.attributeNode( "ID_NAME" );
									//std::cout << "track=" << att.value() << std::endl; 
									if ( trackItem )
										trackItem = new QListViewItem( albumItem, trackItem, att.value() );
									else
										trackItem = new QListViewItem( albumItem, att.value() );
									trackItem->setText( ID_IDENT, CC_TRACK ); 
									att = e1.attributeNode( "ID_DURATION" );
									trackItem->setText( ID_DURATION, att.value() );
									att = e1.attributeNode( "ID_ARTIST" );
									trackItem->setText( ID_ARTIST, att.value() );
									att = e1.attributeNode( "ID_ALBUM" );
									trackItem->setText( ID_ALBUM, att.value());
									att = e1.attributeNode( "ID_TRACK" );
									trackItem->setText( ID_TRACK, att.value());
									att = e1.attributeNode( "ID_BITRATE" );
									trackItem->setText( ID_BITRATE, att.value() );
									att = e1.attributeNode( "ID_SAMPLE" );
									trackItem->setText( ID_SAMPLE, att.value() );
									att = e1.attributeNode( "ID_YEAR" );
									trackItem->setText( ID_YEAR, att.value());
									att = e1.attributeNode( "ID_GENRE" );
									trackItem->setText( ID_GENRE, att.value());
									att = e1.attributeNode( "ID_FILENAME" );
									trackItem->setText( ID_FILENAME, att.value());

									trackItem->setRenameEnabled( ID_NAME, true );
									trackItem->setRenameEnabled( ID_ARTIST, true );
									trackItem->setRenameEnabled( ID_ALBUM, true );
									trackItem->setRenameEnabled( ID_TRACK, true );
									trackItem->setRenameEnabled( ID_BITRATE, true );
									trackItem->setRenameEnabled( ID_YEAR, true );
									trackItem->setRenameEnabled( ID_GENRE, true );

									albumItem->setOpen( true );
								}
							}
							n1 = n1.nextSibling();
						}
					}
				}
				n = n.nextSibling();
			}
		}
		else
		{
			QString s;
			s.sprintf( "(l=%d,c=%d)", errnl, errnc );
			QMessageBox::information( this, "error", errs+s);
		}
		// erreur ?
	}
}

void dvdaudioWnd::saveProject()
{
	if ( dvdItem )
	{
		QDomDocument* doc = new QDomDocument();
		///// add the <?xml> line
		QDomProcessingInstruction instr =
			doc->createProcessingInstruction("xml","version=\"1.0\" encoding=\"UTF-8\" ");
		doc->appendChild(instr);
		///// add the <!DOCTYPE> line
		QDomImplementation impl;
		QDomDocumentType type = impl.createDocumentType("DvDAudio",0, "");
		doc->appendChild(type);

		//QDomDocument doc( "DvDAudio" );
		QDomElement rootElem = doc->createElement( "DVDAUDIO" );
		rootElem.setAttribute( "NAME", dvdItem->text(ID_NAME) );
		doc->appendChild( rootElem );

		QListViewItemIterator it( lvDVD );
		QListViewItem *item;
		QDomElement el;
		QDomElement albumEl;
		int ctr = 0;

		while ( it.current() )
		{
			item = it.current();
			if ( item->text( ID_IDENT ) == CC_ALBUM )
			{

				albumEl = doc->createElement( "ALBUM" );
				albumEl.setAttribute( "NAME", item->text( ID_NAME ) );
				rootElem.appendChild( albumEl );
			}
			else if ( item->text( ID_IDENT ) == CC_TRACK )
			{
				el = doc->createElement( "TRACK" );
				el.setAttribute( "ID_NAME", item->text( ID_NAME ) );
				el.setAttribute( "ID_DURATION", item->text( ID_DURATION ) );
				el.setAttribute( "ID_ARTIST", item->text( ID_ARTIST ) );
				el.setAttribute( "ID_ALBUM", item->text( ID_ALBUM ) );
				el.setAttribute( "ID_TRACK", item->text( ID_TRACK ) );
				el.setAttribute( "ID_BITRATE", item->text( ID_BITRATE ) );
				el.setAttribute( "ID_SAMPLE", item->text( ID_SAMPLE ) );
				el.setAttribute( "ID_YEAR", item->text( ID_YEAR ) );
				el.setAttribute( "ID_GENRE", item->text( ID_GENRE ) );
				el.setAttribute( "ID_FILENAME", item->text(ID_FILENAME) );
				albumEl.appendChild( el );
			}
			it++;
		}
		QString s = QFileDialog::getSaveFileName(
				workingDir,
				"Xml (*.xml)",
				this,
				"save file dialog",
				"Choose a filename to save under" );
		QFile f( s );
		if ( !f.open(IO_WriteOnly) )
		{
			QMessageBox::information( this, "error", "Cannot open file" );
		}
		QTextStream st( &f );
		st.setEncoding( QTextStream::UnicodeUTF8 );
		st << doc->toString().utf8();
		f.close();
	}
}

void dvdaudioWnd::encode()
{
	QDir::setCurrent( workingDir );
	if ( !QDir::current().mkdir( "DVD" ) )
	{
		QMessageBox::information( this, "DvdAuthor",
				"Problem making dvdauthor dir" );
		return;
	}

	QListViewItemIterator it( lvDVD );
	QListViewItemIterator it1( lvDVD );
	QListViewItem *item;
	int ctr = 0;
	QString name;
	QString shName;
	QString outputName;
	QString outputMp2;
	QString pal( " -F 25:1 -A 59:54 " );
	QString npal( "25" );
	trackPreviewWnd *tpw;
	QStringList albums;
	while ( it1.current() )
	{
		item = it1.current();
		if ( item->text( ID_IDENT ) == CC_ALBUM )
		{
			albums << item->text( ID_NAME );
		}
		it1++;
	}
	files = new QStringList[ albums.count() ];
	int ctAlbum = -1;
	while ( it.current() )
	{
		item = it.current();
		if ( item->text( ID_IDENT ) == CC_ALBUM )
		{
			ctAlbum++;
		}
		else if ( item->text( ID_IDENT ) == CC_TRACK )
		{
			tpw =new trackPreviewWnd( item, this, "preview", Qt::WType_Dialog );
			tpw->setFont( font );
			tpw->setBgColor( bgcolor );
			setOptions( tpw );
			name.sprintf( "pic%04d.ppm", ctr );
			QPixmap pic = QPixmap::grabWidget( tpw );
			QImage img = pic.convertToImage();
			img.convertDepth( 8 );
			img.save( name, "PPM" );
			delete tpw;

			shName.sprintf( "enc%04d.sh", ctr );
			outputName.sprintf( "vob%04d.vob", ctr );
			QFile fEnc( shName );

			if ( fEnc.open( IO_WriteOnly ) );
			{
				QTextStream stream( &fEnc );
				
				outputMp2.sprintf( "mp2%04d.mp2", ctr );
				stream << "#!/bin/bash" << endl;
				QTime t = QTime::fromString( item->text( ID_DURATION ) );
				int duration = t.hour() * 3600 + t.minute() * 60 + t.second();
				duration *= 25;		// pal
				if ( aMEncoder->isOn() )
				{
					stream << "ppmtoy4m -n " << duration << pal;
					stream << " -r -S 420mpeg2 " << name;
					stream << " | mencoder -audiofile \"" << item->text( ID_FILENAME );
					stream << "\" -srate 48000 -oac lavc -ovc lavc -lavcopts";
					stream << " acodec=mp2:abitrate=" << item->text( ID_BITRATE );
					stream << ":vcodec=mpeg2video:keyint=300:vbitrate=500";
					stream << " -of mpeg -mpegopts format=dvd:vbitrate=600:vframerate=";
					stream << npal << ":tsaf:vaspect=4/3 -o ";
					stream << outputName << " -" << endl;
				}
				else
				{
					// encoding with mpeg2enc
					stream << "lame --decode \"" << item->text( ID_FILENAME );
					stream << "\" - | toolame -b " << item->text( ID_BITRATE );
					stream << " -s 48 /dev/stdin " << outputMp2 << endl;
					stream << "set `mpginfo " << outputMp2;
					stream << "| grep \"Estimated Duration:\" | cut -f2- -d\":\" | cut -f1 -d\".\" | tr \":\" \" \"`" << endl;
					stream << "if [ -n \"$3\" ]" << endl;
					stream << "then" << endl;
					stream << "  dur=$(($1*3600+$2*60+$3))" << endl;
					stream << "elif [ -n \"$2\" ]" << endl;
					stream << "then" << endl;
					stream << "  dur=$(($1*60+$2))" << endl;
					stream << "else" << endl;
					stream << "  dur=$1" << endl;
					stream << "fi" << endl;
					stream << "nb=`expr $dur \\* 25`" << endl;
					stream << "ppmtoy4m -n $nb";
					stream << " -r " << pal << " -S 420mpeg2 ";
					stream << name;
					stream << " | mpeg2enc -a 2 -F 3 -b 500 -n p -f 8 ";
					stream << " -o /dev/stdout | mplex -f 8 -o ";
					stream << outputName << " /dev/stdin " << outputMp2 << endl;
					stream << "# rm -f " << outputMp2;
				}
				stream << "# rm -f " << name << endl;
			}
			fEnc.close();
			if ( !aGenOnly->isOn() )
			{
				procEncode = new QProcess(this );
				procEncode->setWorkingDirectory(QDir::current() );
				encodeWnd *encoding = new encodeWnd( this, "Encode", Qt::WType_Dialog );
				encoding->procEncode = procEncode;

				procEncode->addArgument( "bash" );
				procEncode->addArgument( shName );
				if ( !procEncode->start() )
				{
					QMessageBox::information( this, "Encode",
							"Problem starting encode" );
				}
				QTime t = QTime::fromString( item->text( ID_DURATION ) );
				int duration = t.hour() * 3600 + t.minute() * 60 + t.second();
				encoding->demarrer( item->text(ID_FILENAME), duration*25 );
				if ( !procEncode->normalExit() )
				{
					QMessageBox::information( this, "Problem",
						"encoding error for file " + item->text( ID_FILENAME ) );
				}
				delete encoding;
				delete procEncode;
			}
			//fEnc.remove();
			files[ctAlbum].append( outputName );

			ctr++;
		}
		it++;
	}
	makeMenu( albums );
	if ( !aGenOnly->isOn() )
		makeDvd();
}


void dvdaudioWnd::readProcMplayer()
{
	QStringList abit, arate, ed, kb, ti, ar, al, tr, ye, ge;
	QStringList list = QStringList::split( "\n",
										mpginfoProc->readStdout() );

	ed = QStringList::split( "=", list.grep( "ID_LENGTH").join("") );
	abit = QStringList::split( "=", list.grep( "ID_AUDIO_BITRATE").join("=") );
	arate = QStringList::split( "=", list.grep( "ID_AUDIO_RATE").join("=") );
	ti = QStringList::split( "=", list.grep( "ID_CLIP_INFO_VALUE0").join("") );
	ar = QStringList::split( "=", list.grep( "ID_CLIP_INFO_VALUE1").join("") );
	al = QStringList::split( "=", list.grep( "ID_CLIP_INFO_VALUE2").join("") );
	tr = QStringList::split( "=", list.grep( "ID_CLIP_INFO_VALUE5").join("") );
	ye = QStringList::split( "=", list.grep( "ID_CLIP_INFO_VALUE3").join("") );
	ge = QStringList::split( "=", list.grep( "ID_CLIP_INFO_VALUE6").join("") );

	int x = strtol( ed[1].ascii(), NULL, 10 );
	QTime t = QTime().addSecs( x );

	if ( trackItem )
		trackItem = new QListViewItem( albumItem, trackItem, ti[1] );
	else
		trackItem = new QListViewItem( albumItem, ti[1] );
	trackItem->setText( ID_IDENT, CC_TRACK ); 
	trackItem->setText( ID_DURATION, t.toString() );
	trackItem->setText( ID_ARTIST, ar[1] );
	trackItem->setText( ID_ALBUM, al[1] );
	trackItem->setText( ID_TRACK, tr[1] );
	trackItem->setText( ID_BITRATE, QString::number( abit[3].toInt()/1000) );
	trackItem->setText( ID_SAMPLE, arate[3] );
	trackItem->setText( ID_YEAR, ye[1] );
	trackItem->setText( ID_GENRE, ge[1] );

	trackItem->setRenameEnabled( ID_NAME, true );
	trackItem->setRenameEnabled( ID_ARTIST, true );
	trackItem->setRenameEnabled( ID_ALBUM, true );
	trackItem->setRenameEnabled( ID_TRACK, true );
	trackItem->setRenameEnabled( ID_BITRATE, true );
	trackItem->setRenameEnabled( ID_YEAR, true );
	trackItem->setRenameEnabled( ID_GENRE, true );

	albumItem->setOpen( true );
}

void dvdaudioWnd::readProcMpginfo()
{
	QStringList list = QStringList::split( "\n",
	mpginfoProc->readStdout() );

	QStringList ed, kb, ti, ar, al, tr, ye, ge;

	const QString sp( " " );
	ed = QStringList::split( sp, list.grep( "Estimated Duration:").join("") );
	kb = QStringList::split( sp, list.grep( "kbps  ").join("") );
	ti = QStringList::split( sp, list.grep( "title   :").join("") );
	ar = QStringList::split( sp, list.grep( "artist  :").join("") );
	al = QStringList::split( sp, list.grep( "album   :").join("") );
	tr = QStringList::split( sp, list.grep( "track   :").join("") );
	ye = QStringList::split( sp, list.grep( "year    :").join("") );
	ge = QStringList::split( sp, list.grep( "genre   :").join("") );

	// TODO : what if the file lasts more than 59 minutes ?
	int minutes, secondes, centiemes;
	QTime t;
	if ( ed[2].find( ":" ) != -1 )
	{
		minutes = ed[2].section( ":", 0, 0 ).toInt();
		secondes = ed[2].section( ":", 1, 1 ).section( ".", 0, 0 ).toInt();
		centiemes = ed[2].section( ".", 1, 1 ).toInt();
		t = QTime( 0, minutes, secondes, centiemes );
	}
	else
	{
		// less than a minute ?
		secondes = ed[2].section( ".", 0, 0 ).toInt();
		centiemes = ed[2].section( ".", 1, 1 ).toInt();
		t = QTime( 0, 0, secondes, centiemes );
	}

	//int kbps = kb[0].toInt();
	//int sample = kb[2].toInt();

	QString titre = stringListeEnd( ti, 2 );
	QString artist = stringListeEnd( ar, 2 );
	QString album = stringListeEnd( al, 2 );
	QString track = stringListeEnd( tr, 2 );
	QString year = stringListeEnd( ye, 2 );
	QString genre = stringListeEnd( ge, 2 );
	if ( trackItem )
		trackItem = new QListViewItem( albumItem, trackItem, titre );
	else
		trackItem = new QListViewItem( albumItem, titre );
	trackItem->setText( ID_IDENT, CC_TRACK ); 
	trackItem->setText( ID_DURATION, t.toString() );
	trackItem->setText( ID_ARTIST, artist );
	trackItem->setText( ID_ALBUM, album );
	trackItem->setText( ID_TRACK, track );
	trackItem->setText( ID_BITRATE, kb[0] );
	trackItem->setText( ID_SAMPLE, kb[2] );
	trackItem->setText( ID_YEAR, year );
	trackItem->setText( ID_GENRE, genre );

	trackItem->setRenameEnabled( ID_NAME, true );
	trackItem->setRenameEnabled( ID_ARTIST, true );
	trackItem->setRenameEnabled( ID_ALBUM, true );
	trackItem->setRenameEnabled( ID_TRACK, true );
	trackItem->setRenameEnabled( ID_BITRATE, true );
	trackItem->setRenameEnabled( ID_YEAR, true );
	trackItem->setRenameEnabled( ID_GENRE, true );

	albumItem->setOpen( true );
}

QString dvdaudioWnd::stringListeEnd( QStringList &liste, int depart )
{
	int ctr;
	QString resultat;
	for ( ctr = depart; ctr < liste.count(); ctr++ )
	{
		resultat += liste[ ctr ];
	if ( ctr != liste.count() ) resultat += " ";
	}
	return resultat;
}

void dvdaudioWnd::preview()
{
	if ( trackItem )
	{
		trackPreviewWnd *tpw = new trackPreviewWnd( trackItem, this, "preview", Qt::WType_Dialog );
		tpw->setFont( font );
		tpw->setBgColor( bgcolor );
		setOptions( tpw );
		tpw->show();
	}
}

void dvdaudioWnd::removeItem( )
{
	if ( trackItem )
	{
		delete trackItem;
		trackItem = 0;
	}
	else if ( albumItem )
	{
		delete albumItem;
		albumItem = 0;
	}
	else if ( dvdItem )
	{
		delete dvdItem;
		dvdItem = 0;
	}
}

void dvdaudioWnd::makeMenu( QStringList albums )
{
	if ( albums.count() > 0 )
	{
		QString shName( "menu.sh" );
		QString xmlName( "dvd.xml" );
		QFile fEnc( shName );
		QFile fXml( xmlName );
		if ( ( fEnc.open( IO_WriteOnly ) )
		&& ( fXml.open( IO_WriteOnly ) ) )
		{
			QTextStream stream( &fEnc );
			QTextStream stXml( &fXml );
			int npages = albums.count() / 8;
			if ( ( albums.count() % 8 ) != 0 )
			{
				npages++;
			}
			QString outputName, outputsName, outputhName, outputXml, outputMenu;
			QString outputMpeg;

			stXml << "<dvdauthor dest=\"DVD\">" << endl;
			stXml << "<vmgm>" << endl;
			stXml << "<menus>" << endl;
			stXml << "<pgc entry=\"title\" pause=\"0\" >" << endl;
			stXml << "<pre>g0 = " << aDirectPlay->isOn();
			stXml << "; jump titleset 1 menu; </pre>" << endl;
			stXml << "</pgc>" << endl;
			stXml << "</menus>" << endl;
			stXml << "</vmgm>" << endl;
			stXml << "<titleset>" << endl;
			stXml << "<menus>" << endl;

			stream << "#!/bin/bash" << endl;
			// create a silence mp2 file
			stream << "dd if=/dev/zero bs=4 count=1602 | toolame -b 128 ";
			stream << "-s 48 /dev/stdin silence.mp2" << endl;
				
			// converting background to ppm
			QFont tf = font;
			tf.setPointSize( sbFontSize->value()+2);
			QFontMetrics fm( tf );
			QRect r = fm.boundingRect( dvdItem->text( ID_NAME ) );
			int x = ( 720 - r.width() ) / 2;
			if ( bgPic.isNull( ) )
			{
				QPixmap bgPix( 720, 576 );
				bgPix.fill( black );
				QPainter p( &bgPix );
				p.setPen( QPen( blue, 8 ) );
				p.setBrush( QBrush( blue, Qt::SolidPattern ) );
				QFont ff = font;
				ff.setPointSize( sbFontSize->value() + 2 );
				p.setFont( ff );
				p.drawText( x, 40, dvdItem->text( ID_NAME ) );
				QImage im = bgPix.convertToImage().convertDepth( 8 );
				im.save( "bg.ppm", "PPM" );

				/*
				stream << "convert -size 720x576 -depth 8 xc:black ";
				stream << "-fill blue -pointsize " << sbFontSize->value()+2;
				stream << " -draw \"text " << x;
				stream << ",40 '" << dvdItem->text( ID_NAME );
				stream << "'\" bg.ppm";
				stream << endl;
				*/
			}
			else
			{
				QPixmap bgPix( bgPic );
				bgPix.resize( 720, 576 );
				QPainter p( &bgPix );
				p.setPen( QPen( blue, 8 ) );
				p.setBrush( QBrush( blue, Qt::SolidPattern ) );
				QFont ff = font;
				ff.setPointSize( sbFontSize->value() + 2 );
				p.setFont( ff );
				p.drawText( x, 40, dvdItem->text( ID_NAME ) );
				QImage im = bgPix.convertToImage().convertDepth( 8 );
				im.save( "bg.ppm", "PPM" );

				/*
				stream << "convert " << bgPic;
				stream << " -fill blue -pointsize " << sbFontSize->value()+2;
				stream << " -draw \"text " << x;
				stream << ",40 '" << dvdItem->text( ID_NAME );
				stream << "'\" -depth 8 -resize 720x576 bg.ppm" << endl;
				*/
			}
			// encoding bg to mpeg2
			outputMpeg.sprintf( "menubase.m2v" );
			stream << "ppmtoy4m -n 1 -F25:1 -A 59:54 bg.ppm -S 420mpeg2 ";
			stream << " | mpeg2enc -f 8 -o " << outputMpeg << endl;
			stream << "# rm -f bg.ppm" << endl;


			cleanAlbumNames( albums );
			std::vector<QRect> vrects;
			for ( int pg = 0; pg < npages; pg++ )
			{
				stream << "echo Generating menu " << pg << " of " << npages << endl;
				outputName.sprintf( "menun%04d.png", pg );
				genMenuCommand( albums, white, outputName, pg, npages, &vrects);

				outputhName.sprintf( "menuh%04d.png", pg );
				genMenuCommand( albums, yellow, outputhName, pg, npages,&vrects);

				outputsName.sprintf( "menus%04d.png", pg );
				genMenuCommand( albums, red, outputsName, pg, npages, &vrects );
	
				outputXml.sprintf( "menus%04d.xml", pg );
				outputMenu.sprintf( "menus%04d.mpg", pg );
				// spumux control file
				stream << "echo '<subpictures>" << endl;
				stream << "<stream>" << endl;
				stream << "<spu" << endl;
				stream << "force=\"yes\"" << endl;
				stream << "start=\"00:00:00.00\"" << endl;
				stream << "image=\"" << outputName << "\"" << endl;
				stream << "select=\"" << outputsName << "\"" << endl;
				stream << "highlight=\"" << outputhName << "\"" << endl;
				stream << "transparent=\"000000\"" << endl;
				stream << " >" << endl;
				int x0 = 0, x1 = 0, y0 = 0, y1 = 0;
				for (int i = 0; i < vrects.size(); i++ )
				{
					QRect r = vrects[i];
					// turn values to even
					x0 = ((r.x()-1)/2)*2;
					y0 = ((r.y()-1)/2)*2;
					x1 = ((r.x()+r.width())/2)*2;
					if ( x1 > 720 ) x1 = 720;
					y1 = ((r.y()+r.height())/2)*2;
					if ( y1 > 576 ) y1 = 576;

					stream << "<button name=\"" << i+1 << "\" x0=\"";
					stream << x0 << "\" y0=\"" << y0;
					stream << "\" x1=\"" << x1;
					stream << "\" y1=\"" << y1<< "\" />";
					stream << endl;
				}
				stream << "</spu>" << endl;
				stream << "</stream>" << endl;
				stream << "</subpictures>' >" << outputXml << endl;
				stream << "mplex -f 8 -o /dev/stdout " << outputMpeg;
				stream << " silence.mp2 | ";
				stream << "spumux " << outputXml;
				stream << " > " << outputMenu << endl;
				stream << "# rm -f " << outputName << endl;
				stream << "# rm -f " << outputhName << endl;
				stream << "# rm -f " << outputsName << endl;
				stream << "# rm -f " << outputXml << endl;

				//stXml << "<pgc entry=\"root\" pause=\"0\">" << endl;
				stXml << "<pgc pause=\"0\">" << endl;
				stXml << "<pre>if ( g0 gt 0 ) { g0 = 0; jump title 1 chapter 1; }</pre>" << endl;
				stXml << "<vob file=\"" << outputMenu << "\" pause=\"inf\"></vob>" << endl;
				for ( int ctr = 0; (ctr < 8) && ( ((pg*8)+ctr) < albums.count()); ctr++ )
				{
					stXml << "<button>jump title " << (pg*8)+ctr+1;
					stXml << " chapter 1;</button>" << endl;
				}
				// prev menu
				if ( pg > 0 )
				{
					stXml << "<button>jump menu " << pg;
					stXml << ";</button>" << endl;
				}
				// next menu
				if ( pg != npages - 1 )
				{
					stXml << "<button>jump menu " << pg+2;
					stXml << ";</button>" << endl;
				}
				stXml << "</pgc>" << endl;
			}
			stream << "# rm -f " << outputMpeg << endl;
			stream << "# rm -f silence.mp2" << endl;

			stXml << "</menus>" << endl;
			stXml << "<titles>" << endl;
			for ( int ti = 0; ti < albums.count(); ti++ )
			{
				stXml << "<pgc>" << endl;
				for ( int i = 0; i < files[ti].count(); i++ )
				{
					stXml << "<vob file=\"" << files[ti][i] << "\"></vob>" << endl;
				}
				if ( ti == (albums.count()-1) )
				{
					stXml << "<post>call menu;</post>" << endl;
				}
				else
				{
					stXml << "<post>jump title " << ti+2 << ";</post>" << endl;
				}
				stXml << "</pgc>" << endl;
			}
			stXml << "</titles>" << endl;
			stXml << "</titleset>" << endl;
			stXml << "</dvdauthor>" << endl;

		    stream << "dvdauthor -x dvd.xml" << endl;
			// Here we should clean the vob files, 
			// the dvd.xml file and the ouputMenu file, BUT
			// by not deleting them, user can rerun manually dvdauthor
			// if something went wrong and is not obliged to reencode
			// everything
		}
		fEnc.close();
		fXml.close();
	}
}

void dvdaudioWnd::makeDvd( )
{
	QProcess *dvdauthorProc = new QProcess( this );
	dvdauthorProc->addArgument( "bash" );
	dvdauthorProc->addArgument( "menu.sh" );
	if ( !dvdauthorProc->start() )
	{
		QMessageBox::information( this, "DvdAuthor",
				"Problem authoring DVD" );
	}
	encodeWnd *encoding = new encodeWnd( this, "author", Qt::WType_Dialog );
	encoding->procEncode = dvdauthorProc;
	encoding->demarrerA( );
	if ( !dvdauthorProc->normalExit() )
	{
		QMessageBox::information( this, "Problem",
			"authoring error for file " );
	}
	delete dvdauthorProc;
}

void dvdaudioWnd::setOptions( trackPreviewWnd *t )
{
	t->setDrawGenre( cbGenre->isChecked() );
	t->setDrawArtist( cbArtist->isChecked() );
	t->setDrawAlbum( cbAlbum->isChecked() );
	t->setDrawYear( cbYear->isChecked() );
	t->setDrawTitle( cbTitle->isChecked() );
	t->setDrawTrack( cbTrack->isChecked() );
	t->setDrawDuration( cbDuration->isChecked() );
	t->setDrawBitrate( cbBitrate->isChecked() );
}

void dvdaudioWnd::cleanAlbumNames(QStringList &albums)
{
	// Since we heavily use shell and quotes, both single and double,
	// let's clean album title, just to be on the safe side
	for ( int i = 0; i < albums.count(); i++ )
	{
		QString &s = albums[i].replace( '\'', "" );
		albums[i] = s;
		s = albums[i].replace( '"', "" );
		albums[i] = s;
	}
}

void dvdaudioWnd::genMenuCommand( QStringList albums, const QColor &color, QString outputName, int pg, int npages, std::vector<QRect> *vr )
{
	//convert -size 720x576 xc:transparent -pointsize 28 -fill red -stroke red -strokewidth 2 -colors 3 +antialias -draw "text 80,150 \"Album 1\"" -draw "text 80,200 \"Album 2\"" -quality 100 menuh.png
	int ctr;
	vr->clear();
	QPixmap pm( 720, 576 );
	pm.fill( black );
	QPainter p( &pm );
	QFont ff = font;
	ff.setPointSize( sbFontSize->value() );
	QFontMetrics fm( ff );
	p.setFont( ff );
	p.setPen( QPen( color, 5 ) );
	p.setBrush( QBrush( color, Qt::SolidPattern ) );

	for ( ctr = 0; (ctr < 8) && ( ((pg*8)+ctr) < albums.count()); ctr++ )
	{
		p.drawText( 80, 100+ctr*50, albums[(pg*8)+ctr] );
		QRect r = fm.boundingRect( albums[(pg*8)+ctr] );
		vr->push_back( QRect( 80, (100+ctr*50)-r.height(), r.width(), r.height() ) );
	}
	if ( ( pg != (npages-1) ) && ( npages > 1 ) )
	{
		p.drawText( 600, 550, "Next>>" );
		QRect r = fm.boundingRect( "Next>>" );
		vr->push_back( QRect( 600, 550-r.height(), r.width(), r.height() ) );
	}
	if ( ( pg != 0 ) && ( npages > 1 ) )
	{
		p.drawText( 400, 550, "<<Prev" );
		QRect r = fm.boundingRect( "<<Prev" );
		vr->push_back( QRect( 400, 550-r.height(), r.width(), r.height() ) );
	}
	QImage im = pm.convertToImage();
	clipColors( im, color );
	im.save( outputName, "PNG" );
}

void dvdaudioWnd::selectBgPic( )
{
	bgPic = QFileDialog::getOpenFileName( "/usr", "Images (*.png *.xpm *.jpg)",
		this,
		"open file dialog",
		"Choose a background image for the menu" );
}

void dvdaudioWnd::clipColors( QImage &img, const QColor &destcol )
{
	int clip = 10;
	QRgb destrgb = destcol.rgb();
	for ( int x = 0; x < img.width(); x++ )
	{
		for ( int y = 0; y < img.height(); y++ )
		{
			QRgb rgb = img.pixel( x, y );
			QColor c( rgb );
			if ( ( (c.red()-clip) <= 0 )
				&& ( (c.green()-clip) <= 0 )
				&& ( (c.blue()-clip) <= 0 ) )
			{
				img.setPixel( x, y, black.rgb() );
			}
			else
				img.setPixel( x, y, destrgb );
		}
	}
}
