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
#include "qt4dvdaudiownd.h"

#include "qt4encodewnd.h"
#include "common.h"
#include <iostream>
#include <unistd.h>	// pour sleep
#include <qtextcodec.h>
#include <QFileDialog>
#include <QTextStream>
#include <QTime>

qt4dvdaudioWnd::qt4dvdaudioWnd( QWidget *parent )
	: QMainWindow( parent )
{
	setupUi( this );
	init();
}

qt4dvdaudioWnd::~qt4dvdaudioWnd()
{
	/*
	delete dvdItem;
	delete albumItem;
	delete trackItem;
	delete procEncode;
	delete mpginfoProc;
	dvdItem = 0;
	albumItem = 0;
	trackItem = 0;
	procEncode = 0;
	mpginfoProc = 0;
	*/
}

void qt4dvdaudioWnd::init()
{
	lvDVD->clear();
	dvdItem = 0;
	albumItem = 0;
	trackItem = 0;
	procEncode = 0;
	mpginfoProc = 0;
	bgcolor = Qt::black;
	albumCtr = 0;
	trackDir = QDir::currentPath();
	font = QFont( "Bitstream Vera Sans Mono", 28 );

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
	connect( lvDVD, SIGNAL(clicked(QTreeWidgetItem *)),
		this, SLOT(lvClicked(QTreeWidgetItem*)));
	connect( pbUp, SIGNAL(clicked()), this, SLOT(upClicked()));
	connect( pbDown, SIGNAL(clicked()), this, SLOT(downClicked()));

	pbOccup->setMaximum( 4400000 );

	aNew->setEnabled(false);
	aAddAlbum->setEnabled(false);
	aAddTracks->setEnabled(false);
}

void qt4dvdaudioWnd::downClicked()
{
	QTreeWidgetItem *item = lvDVD->currentItem();
	if ( item )
	{
		lvDVD->setCurrentItem( lvDVD->itemBelow( item ) );
	}
}

void qt4dvdaudioWnd::upClicked()
{
	QTreeWidgetItem *item = lvDVD->currentItem();
	if ( item )
	{
		lvDVD->setCurrentItem( lvDVD->itemAbove( item ) );
	}
}

void qt4dvdaudioWnd::newDVD()
{
	lvDVD->clear();
	lvDVD->setColumnCount( 10 );
	lvDVD->setColumnWidth( 1, 0 );
	lvDVD->setColumnWidth( 10, 0 );
	QStringList heads;
	heads << "ident";
	heads << "duration";
	heads << "artist";
	heads << "album";
	heads << "track";
	heads << "bitrate";
	heads << "sample";
	heads << "year";
	heads << "genre";
	heads << "filename";
	lvDVD->setHeaderLabels( heads );

	lvDVD->setRootIsDecorated( true );

	QStringList root;
	root << "New DVD Audio" << CC_DVD;
	dvdItem = new QTreeWidgetItem( lvDVD, root );
	lvDVD->expandItem( dvdItem );

	workingDir = QFileDialog::getExistingDirectory(
			this, "tempdir",
			"Choose the temporary directory" );
	aAddAlbum->setEnabled(true);
}

void qt4dvdaudioWnd::newAlbum()
{
	if ( !dvdItem ) return;
	QString a;
	QString s = QFileDialog::getExistingDirectory(
		this,
		"get existing directory",
		"Choose a directory" );
	if ( s.isNull() )
	{
		a.sprintf( "Album %d", ++albumCtr );
	}
	else
	{
		a = s.section( '/', -2 );
	}
	QStringList ai;
	ai << a << CC_ALBUM;
	albumItem = new QTreeWidgetItem( albumItem, ai );
	aAddTracks->setEnabled(true);

	if ( !s.isNull() )
	{
		QDir d( s );
		QStringList filters;
		filters << "*.mp3" << "*.wav";
		QStringList l = d.entryList(filters);
		QString filename;
		for ( int i = 0; i < l.count(); i++ )
		{
			filename = d.currentPath() + "/" + l[i];
			mpginfoProc = new QProcess(this );
			connect( mpginfoProc, SIGNAL(readyReadStdout()),
						this, SLOT(readProcMplayer()));
			QStringList args;
			args <<  "mplayer";
			args <<  "-vo";
			args <<  "null";
			args <<  "-ao";
			args <<  "null";
			args <<  "-frames";
			args <<  "0";
			args <<  "-identify";
			args <<  filename;
			mpginfoProc->start( "mplayer", args);

			/*
			connect( mpginfoProc, SIGNAL(readyReadStdout()),
						this, SLOT(readProcMpginfo()));
			mpginfoProc->addArgument( "mpginfo" );
			mpginfoProc->addArgument( filename );
			*/

			while ( mpginfoProc->state() == QProcess::Running )
			{
				sleep( 2 );
			}
			if ( mpginfoProc->exitStatus() != QProcess::NormalExit )
			{
				QMessageBox::information( this, "Problem",
					"mpginfo error for file " + filename );
			}
			else
			{
				trackItem->setText( ID_FILENAME, filename );
				if ( trackItem->text( ID_NAME ).isNull() )
					trackItem->setText( ID_NAME, l[i] );
				delete mpginfoProc;
				// use sox to get exact duration...
				mpginfoProc = new QProcess(this );
				connect( mpginfoProc, SIGNAL(readyReadStderr()),
							this, SLOT(readProcSox()));
				QStringList arg1;
				arg1 << "sox";
				arg1 << filename;
				arg1 << "-n";
				arg1 << "stat";

				mpginfoProc->start( "sox", arg1 );

				while ( mpginfoProc->state() == QProcess::Running )
				{
					sleep( 2 );
				}
				if ( mpginfoProc->exitStatus() != QProcess::NormalExit )
				{
					QMessageBox::information( this, "Problem",
						"sox error for file " + filename );
				}
			}
			delete mpginfoProc;
			QFileInfo fi( filename );
			pbOccup->setValue( pbOccup->value()+int(fi.size()*1.2/1024));
		}
	}
}

void qt4dvdaudioWnd::lvClicked( QTreeWidgetItem *item )
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

void qt4dvdaudioWnd::addTracks()
{
	if ( !dvdItem ) return;
	QFileDialog fd( this,
					"Add tracks", "*.mp3 *.wav" );
	//FIXME fd.setMode( QFileDialog::ExistingFiles );
	if ( fd.exec() == QDialog::Accepted )
	{
		trackDir = fd.directory().currentPath();
		QStringList l = fd.selectedFiles();
		QStringList::Iterator it;
		for ( it = l.begin(); it != l.end(); it++ )
		{
			mpginfoProc = new QProcess(this );
			connect( mpginfoProc, SIGNAL(readyReadStdout()),
						this, SLOT(readProcMplayer()));
			QStringList args;
			args << "mplayer";
			args << "-vo";
			args << "null";
			args << "-ao";
			args << "null";
			args << "-frames";
			args << "0";
			args << "-identify";
			args << *it;

			mpginfoProc->start( "mplayer", args);
			while ( mpginfoProc->state() == QProcess::Running )
			{
			}
			if ( mpginfoProc->exitStatus() != QProcess::NormalExit )
			{
				QMessageBox::information( this, "Problem",
					"mplayer identify error for file " + *it );
			}
			else
			{
				trackItem->setText( ID_FILENAME, *it );

				delete mpginfoProc;
				// use sox to get exact duration...
				mpginfoProc = new QProcess(this );
				connect( mpginfoProc, SIGNAL(readyReadStderr()),
							this, SLOT(readProcSox()));
				QStringList arg2;
				arg2 << "sox";
				arg2 << *it;
				arg2 << "-n";
				arg2 << "stat";

				mpginfoProc->start( "sox", arg2 );

				while ( mpginfoProc->state() == QProcess::Running )
				{
					sleep( 2 );
				}
				if ( mpginfoProc->exitStatus() != QProcess::NormalExit )
				{
					QMessageBox::information( this, "Problem",
						"sox error for file " + *it );
				}
			}
			delete mpginfoProc;

			QFileInfo fi( *it );
			pbOccup->setValue( pbOccup->value()+int(fi.size()*1.18/1024) );
		}
	}
}

void qt4dvdaudioWnd::selectBgColor()
{
	bgcolor = QColorDialog::getColor();
}

void qt4dvdaudioWnd::selectFont()
{
	bool ok;
	font = QFontDialog::getFont(&ok, font );
}

void qt4dvdaudioWnd::newProject()
{
}

void qt4dvdaudioWnd::openProject()
{

	QString s = QFileDialog::getOpenFileName(
                    this,
                    "open file dialog",
                    "Xml (*.xml)",
                    "Choose a file to open" );
 	if ( !s.isNull() )
	{
		QFile f( s );
		if ( !f.open( QIODevice::ReadOnly ) )
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
			QStringList sl;
			sl << "ident";
			sl << "duration";
			sl << "artist";
			sl << "album";
			sl << "track";
			sl << "bitrate";
			sl << "sample";
			sl << "year";
			sl << "genre";
			sl << "filename";

			lvDVD->setRootIsDecorated( true );
			dvdItem = new QTreeWidgetItem( lvDVD, sl );
			lvDVD->expandItem( dvdItem );

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
						QStringList ai;
						ai << albumname.value() << CC_ALBUM;
						
						albumItem = new QTreeWidgetItem( dvdItem, ai );
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
									QStringList ti;
									ti << att.value();
									ti << CC_TRACK; 
									att = e1.attributeNode( "ID_DURATION" );
									ti << att.value();
									att = e1.attributeNode( "ID_ARTIST" );
									ti << att.value();
									att = e1.attributeNode( "ID_ALBUM" );
									ti << att.value();
									att = e1.attributeNode( "ID_TRACK" );
									ti << att.value();
									att = e1.attributeNode( "ID_BITRATE" );
									ti << att.value();
									att = e1.attributeNode( "ID_SAMPLE" );
									ti << att.value();
									att = e1.attributeNode( "ID_YEAR" );
									ti << att.value();
									att = e1.attributeNode( "ID_GENRE" );
									ti << att.value();
									att = e1.attributeNode( "ID_FILENAME" );
									ti << att.value();

									trackItem = new QTreeWidgetItem( albumItem, ti );
									lvDVD->expandItem( albumItem );
									aAddTracks->setEnabled(true);
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

void qt4dvdaudioWnd::saveProject()
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

		QTreeWidgetItemIterator it( lvDVD );
		QTreeWidgetItem *item;
		QDomElement el;
		QDomElement albumEl;

		while ( *it )
		{
			item = *it;
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
				this,
				"save file dialog",
				"Xml (*.xml)",
				"Choose a filename to save under" );
		QFile f( s );
		if ( !f.open(QIODevice::WriteOnly) )
		{
			QMessageBox::information( this, "error", "Cannot open file" );
		}
		QTextStream st( &f );
		st.setCodec( "UTF-8" );
		st << doc->toString();
		f.close();
	}
}

void qt4dvdaudioWnd::encode()
{
	QDir::setCurrent( workingDir );
	if ( !QDir::current().mkdir( "DVD" ) )
	{
		QMessageBox::information( this, "DvdAuthor",
				"Problem making dvdauthor dir" );
		return;
	}

	QTreeWidgetItemIterator it( lvDVD );
	QTreeWidgetItemIterator it1( lvDVD );
	QTreeWidgetItem *item;
	int ctr = 0;
	QString name;
	QString shName;
	QString outputName;
	QString outputMp2;
	QString outputPcm;
	QString pal( " -F 25:1 -A 59:54 " );
	QString npal( "25" );
	trackPreviewWnd *tpw;
	QStringList albums;
	while ( *it1 )
	{
		item = *it1;
		if ( item->text( ID_IDENT ) == CC_ALBUM )
		{
			albums << item->text( ID_NAME );
		}
		it1++;
	}
	files = new QStringList[ albums.count() ];
	int ctAlbum = -1;
	while ( *it )
	{
		item = *it;
		if ( item->text( ID_IDENT ) == CC_ALBUM )
		{
			ctAlbum++;
		}
		else if ( item->text( ID_IDENT ) == CC_TRACK )
		{
			tpw =new trackPreviewWnd( item, this );
			tpw->setFont( font );
			tpw->setBgColor( bgcolor );
			setOptions( tpw );
			name.sprintf( "pic%04d.ppm", ctr );
			QPixmap pic = QPixmap::grabWidget( tpw );
			QImage img = pic.toImage();
			//FIXME img.setDepth( 8 );
			img.save( name, "PPM" );
			delete tpw;

			shName.sprintf( "enc%04d.sh", ctr );
			outputName.sprintf( "vob%04d.vob", ctr );
			QFile fEnc( shName );

			if ( fEnc.open( QIODevice::WriteOnly ) );
			{
				QTextStream stream( &fEnc );
				
				outputMp2.sprintf( "mp2%04d.mp2", ctr );
				outputPcm.sprintf( "pcm%04d.lpcm", ctr );
				stream << "#!/bin/bash" << endl;
				QTime t = QTime::fromString( item->text( ID_DURATION ) );
				double msecs = item->text( ID_DURATION ).section( ".", 1, 1 ).toDouble();
				double dur = t.hour() * 3600 + t.minute() * 60 + t.second() + msecs/1000.0;
				std::cout << "ms=" << t.msec() << " dur=" << dur << std::endl;
				int duration = dur * 25;		// pal
				if ( item->text( ID_FILENAME ).contains( ".wav" ) )
				{	// wav file
					stream << "#sox \"" << item->text( ID_FILENAME );
					stream << "\" -t raw -x -s -w -c2 -r48000 ";
					stream << outputPcm << endl;
					stream << "ppmtoy4m  -n " << duration << pal << " -r -S 420mpeg2 " << name << " | mpeg2enc -f 8 -b 600 -q 31 -G 100 -o /dev/stdout | mplex -f 8 -o " << outputName << " -L 48000:2:16 /dev/stdin " << outputPcm << endl;
				}
				else
				{
					if ( aMEncoder->isChecked() )
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
				}
				stream << "# rm -f " << name << endl;
			}
			fEnc.close();
			if ( !aGenOnly->isChecked() )
			{
				procEncode = new QProcess(this );
				procEncode->setWorkingDirectory(QDir::currentPath() );
				qt4encodeWnd *encoding = new qt4encodeWnd( this );
				encoding->procEncode = procEncode;

				QStringList arg;

				arg << "bash";
				arg << shName;
				procEncode->start( "bash", arg);
				if ( procEncode->state() != QProcess::Running )
				{
					QMessageBox::information( this, "Encode",
							"Problem starting encode" );
				}
				QTime t = QTime::fromString( item->text( ID_DURATION ) );
				int duration = t.hour() * 3600 + t.minute() * 60 + t.second();
				encoding->demarrer( item->text(ID_FILENAME), duration*25 );
				if ( procEncode->exitStatus() != QProcess::NormalExit )
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
	if ( !aGenOnly->isChecked() )
		makeDvd();
}


void qt4dvdaudioWnd::readProcSox()
{
	QByteArray ba = mpginfoProc->readAll();
	int idx = ba.indexOf( "Length" );
	int idx1 = ba.indexOf( '\n', idx );
	if ( ( idx != -1 ) && ( idx1 != -1 ) )
	{
		QByteArray bb = ba.mid( idx+6, idx1-idx ); 
		double x = strtod( bb.constData(), NULL );
		double msecs = (x - (int)x) * 1000;
		//std::cout << " x=" << x << " ms=" << msecs << std::endl;
		QTime t = QTime().addSecs( (int)x ).addMSecs( msecs );
		trackItem->setText( ID_DURATION, t.toString("hh:mm:ss.zzz") );
	}
	/*
	QStringList list = QString::split( "\n",
						codec->toUnicode( mpginfoProc->read(4096) ) );
	QStringList dur = QString::split( ":", list.grep( "Length").join("") );
	double x = strtod( dur[1].ascii(), NULL );
	double msecs = (x - (int)x) * 1000;
	//std::cout << " x=" << x << " ms=" << msecs << std::endl;
	QTime t = QTime().addSecs( (int)x ).addMSecs( msecs );
	trackItem->setText( ID_DURATION, t.toString("hh:mm:ss.zzz") );
	*/
}

void qt4dvdaudioWnd::readProcMplayer()
{
	QStringList abit, arate, ed, kb, ti, ar, al, tr, ye, ge, fn;
	QByteArray ba = mpginfoProc->readAll();
	QList<QByteArray> list = ba.split( '\n' );
	QList<QByteArray>::Iterator it = list.begin();
	int idx;
	QString filename, abitrate, audrate, title, album;
	QString artist, genre, year, track;
	for ( ; it != list.end(); it++ )
	{
		if ( ( idx = it->indexOf( "ID_FILENAME" ) ) != -1 )
		{
			filename = it->remove( idx+1, 12 );
		}
		if ( ( idx = it->indexOf( "ID_AUDIO_BITRATE") ) != -1 )
		{
			abitrate = it->remove( idx+1, 17 );
		}
		if ( ( idx = it->indexOf( "ID_AUDIO_RATE") ) != -1 )
		{
			audrate = it->remove( idx+1, 14 );
		}
		if (( idx = it->indexOf( "ID_CLIP_INFO_VALUE0") ) != -1 )
		{
			title = it->remove( idx+1, 20 );
		}
		if (( idx = it->indexOf( "ID_CLIP_INFO_VALUE1") ) != -1 )
		{
			artist = it->remove( idx+1, 20 );
		}
		if (( idx = it->indexOf( "ID_CLIP_INFO_VALUE2") ) != -1 )
		{
			album = it->remove( idx+1, 20 );
		}
		if (( idx = it->indexOf( "ID_CLIP_INFO_VALUE5") ) != -1 )
		{
			track = it->remove( idx+1, 20 );
		}
		if (( idx = it->indexOf( "ID_CLIP_INFO_VALUE3") ) != -1 )
		{
			year = it->remove( idx+1, 20 );
		}
		if (( idx = it->indexOf( "ID_CLIP_INFO_VALUE6") ) != -1 )
		{
			genre = it->remove( idx+1, 20 );
		}
	}
	QStringList item;
	if ( title.isEmpty() )
	{
		item << filename;
	}
	else
	{
		item << title;
	}
	item << CC_TRACK << artist << album << track << abitrate << audrate ;
	item << year << genre;

	trackItem = new QTreeWidgetItem( albumItem, item );
	lvDVD->expandItem( albumItem );
}

void qt4dvdaudioWnd::readProcMpginfo()
{
	QByteArray ba = mpginfoProc->readAll();
	QList<QByteArray> list = ba.split( '\n' );
	QList<QByteArray>::Iterator it;
	int idx;
	QString ed, kb, ti, ar, al, tr, ye, ge;
	for (it = list.begin(); it != list.end(); it++ )
	{
		if ( ( idx = it->indexOf( "Estimated Duration:") ) != -1 )
		{
			ed = it->remove( idx, idx+19 );
		}
		if ( ( idx = it->indexOf( "kbps  ") ) != -1 )
		{
			kb = it->remove( idx, idx+5 );
		}
		if ( ( idx = it->indexOf( "title   :") ) != -1 )
		{
			ti = it->remove( idx, idx+10 );
		}
		if ( ( idx = it->indexOf( "artist  :") ) != -1 )
		{
			ar = it->remove( idx, idx+10 );
		}
		if ( ( idx = it->indexOf( "album   :") ) != -1 )
		{
			al = it->remove( idx, idx+10 );
		}
		if ( ( idx = it->indexOf( "track   :") ) != -1 )
		{
			tr = it->remove( idx, idx+10 );
		}
		if ( ( idx = it->indexOf( "year    :") ) != -1 )
		{
			ye = it->remove( idx, idx+10 );
		}
		if ( ( idx = it->indexOf( "genre   :") ) != -1 )
		{
			ge = it->remove( idx, idx+10 );
		}
	}
	// TODO : what if the file lasts more than 59 minutes ?
	int minutes, secondes, centiemes;
	QTime t;
	if ( ed.indexOf( ":" ) != -1 )
	{
		minutes = ed.section( ":", 0, 0 ).toInt();
		secondes = ed.section( ":", 1, 1 ).section( ".", 0, 0 ).toInt();
		centiemes = ed.section( ".", 1, 1 ).toInt();
		t = QTime( 0, minutes, secondes, centiemes );
	}
	else
	{
		// less than a minute ?
		secondes = ed.section( ".", 0, 0 ).toInt();
		centiemes = ed.section( ".", 1, 1 ).toInt();
		t = QTime( 0, 0, secondes, centiemes );
	}

	//int kbps = kb[0].toInt();
	//int sample = kb[2].toInt();

	QStringList item;

	item << ti << CC_TRACK << t.toString() << ar << al << tr << kb;
	item << kb << ye << ge;

	trackItem = new QTreeWidgetItem( albumItem, item );

	lvDVD->expandItem( albumItem );
}

QString qt4dvdaudioWnd::stringListeEnd( QStringList &liste, int depart )
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

void qt4dvdaudioWnd::preview()
{
	if ( trackItem )
	{
		trackPreviewWnd *tpw = new trackPreviewWnd( trackItem, this );
		tpw->setFont( font );
		tpw->setBgColor( bgcolor );
		setOptions( tpw );
		tpw->show();
	}
}

void qt4dvdaudioWnd::removeItem( )
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
		bool hasAlbums = false;
		QTreeWidgetItemIterator it( lvDVD );
		QTreeWidgetItem *item;
		while ( *it )
		{
			item = *it;
			if ( item->text( ID_IDENT ) == CC_ALBUM )
			{
				hasAlbums = true;
			}
			it++;
		}
		aAddTracks->setEnabled(hasAlbums);
	}
	else if ( dvdItem )
	{
		delete dvdItem;
		dvdItem = 0;
		aAddAlbum->setEnabled(false);
		aAddTracks->setEnabled(false);
	}
}

void qt4dvdaudioWnd::makeMenu( QStringList albums )
{
	if ( albums.count() > 0 )
	{
		QString shName( "menu.sh" );
		QString xmlName( "dvd.xml" );
		QFile fEnc( shName );
		QFile fXml( xmlName );
		if ( ( fEnc.open( QIODevice::WriteOnly ) )
		&& ( fXml.open( QIODevice::WriteOnly ) ) )
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
			stXml << "<pre>g0 = " << aDirectPlay->isChecked();
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
				bgPix.fill( Qt::black );
				QPainter p( &bgPix );
				p.setPen( QPen( Qt::blue, 8 ) );
				p.setBrush( QBrush( Qt::blue, Qt::SolidPattern ) );
				QFont ff = font;
				ff.setPointSize( sbFontSize->value() + 2 );
				p.setFont( ff );
				p.drawText( x, 40, dvdItem->text( ID_NAME ) );
				QImage im = bgPix.toImage()/*FIXME .convertDepth( 8 )*/;
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
				bgPix = bgPix.scaled( 720, 576 );
				QPainter p( &bgPix );
				p.setPen( QPen( Qt::blue, 8 ) );
				p.setBrush( QBrush( Qt::blue, Qt::SolidPattern ) );
				QFont ff = font;
				ff.setPointSize( sbFontSize->value() + 2 );
				p.setFont( ff );
				p.drawText( x, 40, dvdItem->text( ID_NAME ) );
				QImage im = bgPix.toImage() /*FIXME .convertDepth( 8 )*/;
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
				genMenuCommand( albums, Qt::white, outputName, pg, npages, &vrects);

				outputhName.sprintf( "menuh%04d.png", pg );
				genMenuCommand( albums, Qt::yellow, outputhName, pg, npages,&vrects);

				outputsName.sprintf( "menus%04d.png", pg );
				genMenuCommand( albums, Qt::red, outputsName, pg, npages, &vrects );
	
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

void qt4dvdaudioWnd::makeDvd( )
{
	QProcess *dvdauthorProc = new QProcess( this );
	QStringList args;

	args << "bash" << "menu.sh";
	dvdauthorProc->start( "bash", args );
	if ( dvdauthorProc->state( ) != QProcess::Running )
	{
		QMessageBox::information( this, "DvdAuthor",
				"Problem authoring DVD" );
	}
	qt4encodeWnd *encoding = new qt4encodeWnd( this );
	encoding->procEncode = dvdauthorProc;
	encoding->demarrerA( );
	if ( !dvdauthorProc->exitStatus() == QProcess::NormalExit )
	{
		QMessageBox::information( this, "Problem",
			"authoring error for file " );
	}
	delete dvdauthorProc;
}

void qt4dvdaudioWnd::setOptions( trackPreviewWnd *t )
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

void qt4dvdaudioWnd::cleanAlbumNames(QStringList &albums)
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

void qt4dvdaudioWnd::genMenuCommand( QStringList albums, const QColor &color, QString outputName, int pg, int npages, std::vector<QRect> *vr )
{
	//convert -size 720x576 xc:transparent -pointsize 28 -fill red -stroke red -strokewidth 2 -colors 3 +antialias -draw "text 80,150 \"Album 1\"" -draw "text 80,200 \"Album 2\"" -quality 100 menuh.png
	int ctr;
	vr->clear();
	QPixmap pm( 720, 576 );
	pm.fill( Qt::black );
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
	if ( ( pg != 0 ) && ( npages > 1 ) )
	{
		p.drawText( 400, 550, "<<Prev" );
		QRect r = fm.boundingRect( "<<Prev" );
		vr->push_back( QRect( 400, 550-r.height(), r.width(), r.height() ) );
	}
	if ( ( pg != (npages-1) ) && ( npages > 1 ) )
	{
		p.drawText( 600, 550, "Next>>" );
		QRect r = fm.boundingRect( "Next>>" );
		vr->push_back( QRect( 600, 550-r.height(), r.width(), r.height() ) );
	}
	QImage im = pm.toImage();
	clipColors( im, color );
	im.save( outputName, "PNG" );
}

void qt4dvdaudioWnd::selectBgPic( )
{
	bgPic = QFileDialog::getOpenFileName( this,
		"open file dialog",
		"Choose a background image for the menu" );
}

void qt4dvdaudioWnd::clipColors( QImage &img, const QColor &destcol )
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
				img.setPixel( x, y, QRgb( Qt::black ) );
			}
			else
				img.setPixel( x, y, destrgb );
		}
	}
}

//vim: ts=4 sw=4
