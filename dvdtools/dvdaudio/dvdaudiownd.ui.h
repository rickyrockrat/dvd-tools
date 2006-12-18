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
		a = s;
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
						this, SLOT(readProc()));
			mpginfoProc->addArgument( "mpginfo" );
			mpginfoProc->addArgument( filename );
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
			connect( mpginfoProc, SIGNAL(readyReadStdout()),
						this, SLOT(readProc()));
			mpginfoProc->addArgument( "mpginfo" );
			mpginfoProc->addArgument( *it );
			mpginfoProc->start();

			while ( mpginfoProc->isRunning() )
			{
			}
			if ( !mpginfoProc->normalExit() )
			{
				QMessageBox::information( this, "Problem",
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
			outputName.sprintf( "vob%04d.vob", ctr++ );
			QFile fEnc( shName );
			QTime t = QTime::fromString( item->text( ID_DURATION ) );
			int duration = t.hour() * 3600 + t.minute() * 60 + t.second();

			if ( fEnc.open( IO_WriteOnly ) );
			{
				duration *= npal.toInt();
				QTextStream stream( &fEnc );
				
				outputMp2.sprintf( "mp2%04d.mp2", ctr++ );
				stream << "#!/bin/bash" << endl;
				stream << "lame --decode \"" << item->text( ID_FILENAME );
				stream << "\" - | toolame -b " << item->text( ID_BITRATE );
				stream << " -s 48 /dev/stdin " << outputMp2 << endl;
				if ( aMEncoder->isOn() )
				{
					stream << "ppmtoy4m -n " << duration << pal;
					stream << " -r -S 420mpeg2 " << name;
					stream << " | mencoder -audiofile " << outputMp2;
					stream << " -oac copy -ovc lavc -lavcopts";
					stream << " vcodec=mpeg2video:keyint=300:vbitrate=500";
					stream << " -of mpeg -mpegopts format=dvd:vframerate=";
					stream << npal << ":tsaf:vaspect=4/3 -o ";
					stream << outputName << " -" << endl;
				}
				else
				{
					// encoding with mpeg2enc
					stream << "ppmtoy4m -n " << duration;
					stream << " -r " << pal << " -S 420mpeg2 ";
					stream << name;
					stream << " | mpeg2enc -a 2 -F 3 -b 500 -n p -f 8 ";
					stream << " -o /dev/stdout | mplex -f 8 -o ";
					stream << outputName << " /dev/stdin " << outputMp2 << endl;
				}
				stream << "rm -f " << name << endl;
				stream << "rm -f " << outputMp2;
			}
			fEnc.close();
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
			encoding->demarrer( item->text(ID_FILENAME), duration );
			if ( !procEncode->normalExit() )
			{
				QMessageBox::information( this, "Problem",
					"encoding error for file " + item->text( ID_FILENAME ) );
			}
			delete procEncode;
			//fEnc.remove();
			files[ctAlbum].append( outputName );

		}
		it++;
	}
	makeMenu( albums );
	makeDvd();
}


void dvdaudioWnd::readProc()
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
			int npages = albums.count() / 8 + 1;
			QString outputName, outputsName, outputhName, outputXml, outputMenu;
			QString outputMpeg;

			stXml << "<dvdauthor dest=\"DVD\">" << endl;
			stXml << "<vmgm>" << endl;
			stXml << "<menus>" << endl;
			stXml << "</menus>" << endl;
			stXml << "</vmgm>" << endl;
			stXml << "<titleset>" << endl;
			stXml << "<menus>" << endl;

			stream << "#!/bin/bash" << endl;
			// create a silence mp2 file
			stream << "dd if=/dev/zero bs=4 count=1602 | toolame -b 128 ";
			stream << "-s 48 /dev/stdin silence.mp2" << endl;
				
			// converting background to ppm
			if ( bgPic.isNull( ) )
			{
				stream << "convert -size 720x576 -depth 8 xc:black bg.ppm";
				stream << endl;
			}
			else
			{
				stream << "convert " << bgPic;
				stream << " -depth 8 -resize 720x576 bg.ppm" << endl;
			}
			// encoding bg to mpeg2
			outputMpeg.sprintf( "menu_base.m2v" );
			stream << "ppmtoy4m -n 1 -F25:1 -A 59:54 bg.ppm -S 420mpeg2 ";
			stream << " | mpeg2enc -f 8 -o " << outputMpeg << endl;
			stream << "rm -f bg.ppm" << endl;


			cleanAlbumNames( albums );
			for ( int pg = 0; pg < npages; pg++ )
			{
				outputName.sprintf( "menu_%04d.png", pg );
				genMenuCommand( albums, stream, "white", outputName, pg, npages );

				outputhName.sprintf( "menuh_%04d.png", pg );
				genMenuCommand( albums, stream, "yellow", outputhName, pg, npages );

				outputsName.sprintf( "menus_%04d.png", pg );
				genMenuCommand( albums, stream, "red", outputsName, pg, npages );
	
				outputXml.sprintf( "menus_%04d.xml", pg );
				outputMenu.sprintf( "menus_%04d.mpg", pg );
				// spumux control file
				stream << "echo '<subpictures>" << endl;
				stream << "<stream>" << endl;
				stream << "<spu" << endl;
				stream << "force=\"yes\"" << endl;
				stream << "start=\"00:00:00.00\"" << endl;
				stream << "image=\"" << outputName << "\"" << endl;
				stream << "select=\"" << outputsName << "\"" << endl;
				stream << "highlight=\"" << outputhName << "\"" << endl;
				stream << "autooutline=\"infer\"" << endl;
				stream << "outlinewidth=\"12\"" << endl;
				stream << "autoorder=\"rows\">" << endl;
				stream << "</spu>" << endl;
				stream << "</stream>" << endl;
				stream << "</subpictures>' >" << outputXml << endl;
				stream << "mplex -f 8 -o /dev/stdout " << outputMpeg;
				stream << " silence.mp2 | ";
				stream << "spumux " << outputXml;
				stream << " > " << outputMenu << endl;
				stream << "rm -f " << outputMpeg << endl;
				stream << "rm -f " << outputName << endl;
				stream << "rm -f " << outputhName << endl;
				stream << "rm -f " << outputsName << endl;
				stream << "rm -f " << outputXml << endl;

				stXml << "<pgc>" << endl;
				stXml << "<vob file=\"" << outputMenu << "\"></vob>" << endl;
				for ( int ctr = 0; (ctr < 8) && ( ((pg*8)+ctr) < albums.count()); ctr++ )
				{
					stXml << "<button>jump title " << ctr+1;
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
			stream << "rm -f silence.mp2" << endl;

			stXml << "</menus>" << endl;
			stXml << "<titles>" << endl;
			for ( int ti = 0; ti < albums.count(); ti++ )
			{
				stXml << "<pgc>" << endl;
				for ( int i = 0; i < files[ti].count(); i++ )
				{
					stXml << "<vob file=\"" << files[ti][i] << "\"></vob>" << endl;
				}
				stXml << "</pgc>" << endl;
			}
			stXml << "</titles>" << endl;
			stXml << "</titleset>" << endl;
			stXml << "</dvdauthor>" << endl;

		    stream << "dvdauthor -x dvd.xml" << endl;
			// Here we should clean the vob files, 
			// the dvd.xml file and the ouputMenu file, BUT
			// by no deleting them, user can rerun manually dvdauthor
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

void dvdaudioWnd::genMenuCommand( QStringList albums, QTextStream &stream, QString color, QString outputName, int pg, int npages )
{
	//convert -size 720x576 xc:transparent -pointsize 28 -fill red -stroke red -strokewidth 2 -colors 3 +antialias -draw "text 80,150 \"Album 1\"" -draw "text 80,200 \"Album 2\"" -quality 100 menuh.png
	int ctr;
	stream << "convert -size 720x576 xc:transparent -pointsize " << sbFontSize->value();
	stream << " -fill " << color << " -stroke " << color;
	stream << " -strokewidth 2 -colors 3 +antialias";
	for ( ctr = 0; (ctr < 8) && ( ((pg*8)+ctr) < albums.count()); ctr++ )
	{
		stream << " -draw 'text 80," << 100+ctr*50 << " \"";
		stream << albums[(pg*8)+ctr] << "\"'";
	}
	if ( ( pg != (npages-1) ) && ( npages > 1 ) )
	{
		// next
		stream << " -draw 'text 600,550" << " \"";
		stream << "Next>>" << "\"'";
	}
	if ( ( pg != 0 ) && ( npages > 1 ) )
	{
		// prev
		stream << " -draw 'text 400,550" << " \"";
		stream << "<<Prev" << "\"'";
	}
	stream << " -quality 100 " << outputName << endl;
}

void dvdaudioWnd::selectBgPic( )
{
	bgPic = QFileDialog::getOpenFileName( "/usr", "Images (*.png *.xpm *.jpg)",
		this,
		"open file dialog",
		"Choose a background image for the menu" );
}

