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

void dvdmenuWnd::init()
{
	lvDVD->clear();
	lvDVD->setSorting(-1);
	dvdItem = 0;
	titleItem = 0;
	videoItem = 0;
	procEncode = 0;
	mpginfoProc = 0;
	bgcolor = Qt::black;
	titleCtr = 0;
	videoDir = QDir::currentDirPath();
	font.setStyleStrategy( QFont::PreferAntialias );
	font = QFont( "Bitstream Vera Sans Mono", 28 );

	connect( aNew, SIGNAL(activated()), this, SLOT(newProject()) );
	connect( aOpen, SIGNAL(activated()), this, SLOT(openProject()) );
	connect( aSave, SIGNAL(activated()), this, SLOT(saveProject()) );

	connect( aAddTitle, SIGNAL(activated()), this, SLOT(addTitle()) );
	connect( aAddVideo, SIGNAL(activated()), this, SLOT(addVideo()) );
	connect( aBgColor, SIGNAL(activated()), this, SLOT(selectBgColor()) );
	connect( aEncode, SIGNAL(activated()), this, SLOT(encode()) );
	connect( aExit, SIGNAL(activated()), this, SLOT(close()) );
	connect( aFont, SIGNAL(activated()), this, SLOT(selectFont()) );
	connect( aNewDVD, SIGNAL(activated()), this, SLOT(newDVD()) );
	connect( aPreview, SIGNAL(activated()), this, SLOT(preview()) );
	connect( aRemove, SIGNAL(activated()), this, SLOT(removeItem()) );
	connect( pbMenuBg, SIGNAL(clicked()), this, SLOT(selectBgPic()) );
	connect( pbMenuSound, SIGNAL(clicked()), this, SLOT(selectSound()) );
	connect( lvDVD, SIGNAL(clicked(QListViewItem *)),
	this, SLOT(lvClicked(QListViewItem*)));

	pbOccup->setTotalSteps( 4400000 );
	r1 = QRect( 50, 70, 230, 180 );
	r2 = QRect( 350, 70, 230, 180 );
	r3 = QRect( 50, 320, 230, 180 );
	r4 = QRect( 350, 320, 230, 180 );
	tabr[0] = r1;
	tabr[1] = r2;
	tabr[2] = r3;
	tabr[3] = r4;
}

void dvdmenuWnd::newDVD()
{
	lvDVD->clear();
	lvDVD->addColumn( "ident", 0 );
	lvDVD->addColumn( "picture/chapters" );
	lvDVD->addColumn( "aspect" );
	lvDVD->addColumn( "audio1" );
	lvDVD->addColumn( "audio2" );
	lvDVD->addColumn( "sub1" );
	lvDVD->addColumn( "sub2" );
	lvDVD->addColumn( "sub3" );
	lvDVD->addColumn( "sub4" );
	lvDVD->addColumn( "filename", 0 );

	lvDVD->setRootIsDecorated( true );

	dvdItem = new QListViewItem( lvDVD, "New DVD", CC_DVD );
	dvdItem->setRenameEnabled( 0, true );
	dvdItem->setOpen( true );

	workingDir = QFileDialog::getExistingDirectory(
			QDir::currentDirPath(), this, "tempdir",
			"Choose the temporary directory" );
}

void dvdmenuWnd::addTitle()
{
	newTitle *nt = new newTitle(this, "add", true);

	if ( nt->exec() == QDialog::Accepted )
	{
		QString a = nt->title;
		if ( titleItem )
			titleItem = new QListViewItem( dvdItem, titleItem, a, CC_TITLE);
		else
			titleItem = new QListViewItem( dvdItem, a, CC_TITLE );
		titleItem->setRenameEnabled( ID_NAME, true );
		titleItem->setRenameEnabled( ID_PICTURE, true );
		titleItem->setRenameEnabled( ID_ASPECT, true );
		titleItem->setRenameEnabled( ID_AUDIO1, true );
		titleItem->setRenameEnabled( ID_AUDIO2, true );
		titleItem->setRenameEnabled( ID_SUB1, true );
		titleItem->setRenameEnabled( ID_SUB2, true );
		titleItem->setRenameEnabled( ID_SUB3, true );
		titleItem->setRenameEnabled( ID_SUB4, true );

		titleItem->setText( ID_PICTURE, nt->snapshot );
		titleItem->setText( ID_ASPECT, nt->aspect );
		titleItem->setText( ID_AUDIO1, nt->audio1 );
		titleItem->setText( ID_AUDIO2, nt->audio2 );
		titleItem->setText( ID_SUB1, nt->sub1 );
		titleItem->setText( ID_SUB2, nt->sub2 );
		titleItem->setText( ID_SUB3, nt->sub3 );
		titleItem->setText( ID_SUB4, nt->sub4 );

		titleItem->setOpen( true );
	}
}

void dvdmenuWnd::lvClicked( QListViewItem *item )
{
	if ( item )
	{
		if ( item->text( ID_IDENT ) == CC_TITLE )
			titleItem = item;
		if ( item->text( ID_IDENT ) == CC_VIDEO )
		{
			videoItem = item;
		}
		else
		{
		}
		aRemove->setEnabled( true );
	}
	else
	{
		aRemove->setEnabled( false );
	}
}

void dvdmenuWnd::addVideo()
{
	QFileDialog fd( videoDir,
					"*.vob ; *.mpg",
					this,
					"Add tracks", true );
	fd.setMode( QFileDialog::ExistingFiles );
	if ( fd.exec() == QDialog::Accepted )
	{
		videoDir = fd.dir()->absPath();
		QStringList l = fd.selectedFiles();
		QStringList::Iterator it;
		for ( it = l.begin(); it != l.end(); it++ )
		{
			if ( videoItem )
				videoItem = new QListViewItem( titleItem, videoItem, *it );
			else
				videoItem = new QListViewItem( titleItem, *it );
			videoItem->setText( ID_IDENT, CC_VIDEO ); 
			videoItem->setText( ID_FILENAME, *it );
			videoItem->setRenameEnabled( ID_NAME, true );
			videoItem->setRenameEnabled( ID_PICTURE, true );

			QFileInfo fi( *it );
			pbOccup->setProgress( pbOccup->progress()+int(fi.size()*1.2/1024) );
		}
	}
}

void dvdmenuWnd::selectBgColor()
{
	bgcolor = QColorDialog::getColor();
}

void dvdmenuWnd::selectFont()
{
	bool ok;
	font = QFontDialog::getFont(&ok, font );
}

void dvdmenuWnd::newProject()
{
}

void dvdmenuWnd::openProject()
{

	QString s = QFileDialog::getOpenFileName(
                    workingDir,
                    "Xml (*.xml)",
                    this,
                    "open file dialog",
                    "Choose a file to open" );
 	if ( !s.isEmpty() )
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
			QDomAttr dvdname = docElem.attributeNode( "TMPDIR" );
			workingDir = QString::fromUtf8( dvdname.value() );
			dvdname = docElem.attributeNode( "MENUMUSIC" );
			menuSound = QString::fromUtf8( dvdname.value() );
			dvdname = docElem.attributeNode( "MENUBG" );
			bgPic = QString::fromUtf8( dvdname.value() );

			dvdname = docElem.attributeNode( "NAME" );
			//std::cout << "dvd=" << dvdname.value() << std::endl;
			lvDVD->addColumn( "ident", 0 );
			lvDVD->addColumn( "picture/chapters" );
			lvDVD->addColumn( "aspect" );
			lvDVD->addColumn( "audio1" );
			lvDVD->addColumn( "audio2" );
			lvDVD->addColumn( "sub1" );
			lvDVD->addColumn( "sub2" );
			lvDVD->addColumn( "sub3" );
			lvDVD->addColumn( "sub4" );
			lvDVD->addColumn( "filename", 0 );

			lvDVD->setRootIsDecorated( true );
			dvdItem = new QListViewItem( lvDVD, dvdname.value(), CC_DVD );
			dvdItem->setRenameEnabled( ID_NAME, true );
			dvdItem->setOpen(true);

			QDomNode n = docElem.firstChild();
			while( !n.isNull() )
			{
				QDomElement e = n.toElement(); 
				if( !e.isNull() )
				{
					if ( e.tagName() == "TITLE" )
					{
						QDomAttr titlename = e.attributeNode( "NAME" );
						QDomAttr att;
						//std::cout << "title=" << titlename.value() << std::endl; 
						QString t = QString::fromUtf8( titlename.value() );
						if ( titleItem )
							titleItem = new QListViewItem( dvdItem, titleItem, t, CC_TITLE );
						else
							titleItem = new QListViewItem( dvdItem, t, CC_TITLE );
						titleItem->setRenameEnabled( ID_NAME, true );
						titleItem->setRenameEnabled( ID_PICTURE, true );
						titleItem->setRenameEnabled( ID_ASPECT, true );
						titleItem->setRenameEnabled( ID_AUDIO1, true );
						titleItem->setRenameEnabled( ID_AUDIO2, true );
						titleItem->setRenameEnabled( ID_SUB1, true );
						titleItem->setRenameEnabled( ID_SUB2, true );
						titleItem->setRenameEnabled( ID_SUB3, true );
						titleItem->setRenameEnabled( ID_SUB4, true );

						att = e.attributeNode( "ID_PICTURE" );
						titleItem->setText( ID_PICTURE, QString::fromUtf8( att.value()) );
						att = e.attributeNode( "ID_ASPECT" );
						titleItem->setText( ID_ASPECT, QString::fromUtf8( att.value()) );
						att = e.attributeNode( "ID_AUDIO1" );
						titleItem->setText( ID_AUDIO1, QString::fromUtf8( att.value()) );
						att = e.attributeNode( "ID_AUDIO2" );
						titleItem->setText( ID_AUDIO2, QString::fromUtf8( att.value()));
						att = e.attributeNode( "ID_SUB1" );
						titleItem->setText( ID_SUB1, QString::fromUtf8( att.value()));
						att = e.attributeNode( "ID_SUB2" );
						titleItem->setText( ID_SUB2, QString::fromUtf8( att.value() ));
						att = e.attributeNode( "ID_SUB3" );
						titleItem->setText( ID_SUB3, QString::fromUtf8( att.value() ));
						att = e.attributeNode( "ID_SUB4" );
						titleItem->setText( ID_SUB4, QString::fromUtf8( att.value()));

						QDomNode n1 = e.firstChild();
						while( !n1.isNull() )
						{
							QDomElement e1 = n1.toElement(); 
							if( !e1.isNull() )
							{
								if ( e1.tagName() == "VIDEO" )
								{
									att = e1.attributeNode( "ID_NAME" );
									//std::cout << "track=" << att.value() << std::endl; 
									if ( videoItem )
										videoItem = new QListViewItem( titleItem, videoItem, QString::fromUtf8( att.value() ) );
									else
										videoItem = new QListViewItem( titleItem, QString::fromUtf8( att.value() ) );
									videoItem->setText( ID_IDENT, CC_VIDEO ); 

									videoItem->setRenameEnabled( ID_NAME, true );
									att = e1.attributeNode( "ID_CHAPTERS" );
									videoItem->setText( ID_PICTURE, QString::fromUtf8( att.value()));
									videoItem->setRenameEnabled( ID_PICTURE, true );
									att = e1.attributeNode( "ID_FILENAME" );
									videoItem->setText( ID_FILENAME, QString::fromUtf8( att.value()) );
									titleItem->setOpen( true );
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

void dvdmenuWnd::saveProject()
{
	if ( dvdItem )
	{
		QDomDocument doc( "DvD" );
		QDomElement rootElem = doc.createElement( "DVD" );
		rootElem.setAttribute( "NAME", dvdItem->text(ID_NAME).utf8() );
		rootElem.setAttribute( "TMPDIR", workingDir.utf8() );
		rootElem.setAttribute( "MENUMUSIC", menuSound.utf8() );
		rootElem.setAttribute( "MENUBG", bgPic.utf8() );
		doc.appendChild( rootElem );

		QListViewItemIterator it( lvDVD );
		QListViewItem *item;
		QDomElement el;
		QDomElement titleEl;
		int ctr = 0;

		while ( it.current() )
		{
			item = it.current();
			if ( item->text( ID_IDENT ) == CC_TITLE )
			{

				titleEl = doc.createElement( "TITLE" );
				titleEl.setAttribute( "NAME", item->text( ID_NAME ).utf8() );
				titleEl.setAttribute( "ID_PICTURE", item->text( ID_PICTURE ).utf8() );
				titleEl.setAttribute( "ID_ASPECT", item->text( ID_ASPECT ).utf8() );
				titleEl.setAttribute( "ID_AUDIO1", item->text( ID_AUDIO1 ).utf8() );
				titleEl.setAttribute( "ID_AUDIO2", item->text( ID_AUDIO2 ).utf8() );
				titleEl.setAttribute( "ID_SUB1", item->text( ID_SUB1 ).utf8() );
				titleEl.setAttribute( "ID_SUB2", item->text( ID_SUB2 ).utf8() );
				titleEl.setAttribute( "ID_SUB3", item->text( ID_SUB3 ).utf8() );
				titleEl.setAttribute( "ID_SUB4", item->text( ID_SUB4 ).utf8() );
				rootElem.appendChild( titleEl );
			}
			else if ( item->text( ID_IDENT ) == CC_VIDEO )
			{
				el = doc.createElement( "VIDEO" );
				el.setAttribute( "ID_NAME", item->text( ID_NAME ).utf8() );
				el.setAttribute( "ID_CHAPTERS", item->text( ID_PICTURE ).utf8() );
				el.setAttribute( "ID_FILENAME", item->text(ID_FILENAME).utf8() );
				titleEl.appendChild( el );
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
		st << doc.toString().utf8();
		f.close();
	}
}

void dvdmenuWnd::encode()
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
	QString bgName;
	QString outputName;
	QString outputMp2;
	QString pal( " -F 25:1 -A 59:54 " );
	QString npal( "25" );
	//videoPreviewWnd *tpw;
	QStringList titles;
	while ( it1.current() )
	{
		item = it1.current();
		if ( item->text( ID_IDENT ) == CC_TITLE )
		{
			titles << item->text( ID_NAME );
		}
		it1++;
	}
	files = new QStringList[ titles.count() ];
	int ctTitle = -1;
	QString shName( "menu.sh" );
	QString xmlName( "dvd.xml" );
	QFile fEnc( shName );
	QFile fXml( xmlName );
	if ( ( fEnc.open( IO_WriteOnly ) )
		&& ( fXml.open( IO_WriteOnly ) ) )
	{
		QTextStream stream( &fEnc );
		QTextStream stXml( &fXml );
		while ( it.current() )
		{
			item = it.current();
			if ( item->text( ID_IDENT ) == CC_DVD )
			{
				int npages = titles.count() / 4;
				if ( ( titles.count() % 4 ) != 0 )
				{
					npages++;
				}
				stXml << "<dvdauthor dest=\"DVD\">" << endl;
				stXml << "<vmgm>" << endl;
				stXml << "<menus>" << endl;
				stXml << "<pgc entry=\"title\" pause=\"0\" >" << endl;
				ctTitle++;

				stream << "#!/bin/bash" << endl;
				if ( menuSound.isEmpty() )
				{
					// create a silent mp2 file
					stream << "dd if=/dev/zero bs=4 count=1602 | ";
				}
				else
				{
					stream << "lame --decode \"" << menuSound << "\" - | ";
				}
				stream << "toolame -b 128 -s 48 /dev/stdin menusnd.mp2" << endl;

				QString outputMpeg;
				for ( int pg = 0; pg < npages; pg++ )
				{
					QPixmap bg;
					if ( !bgPic.isEmpty() )
					{
						bg = QPixmap( bgPic );
						bg.resize( 720, 576 );
					}
					else if ( bgcolor.isValid() )
					{
						bg = QPixmap( 720, 576 );
						bg.fill( bgcolor );
					}
					else
					{
						bg = QPixmap::fromMimeSource( "black.jpg" );
					}
					name.sprintf( "bg%04d.ppm", pg );
					QPixmap pic0 = drawMenu( bg, pg, npages );
					QImage img0 = pic0.convertToImage();
					img0.convertDepth( 8 ).save( name, "PPM" );

					QString outputName, outputsName, outputhName, outputXml, outputMenu;
					outputName.sprintf( "picN%04d.png", pg );
					outputhName.sprintf( "picH%04d.png", pg );
					outputsName.sprintf( "picS%04d.png", pg );
		
					QPixmap bp = QPixmap::fromMimeSource( "black.jpg" );
					std::vector<QRect> vr;

					QPixmap pic1 = drawButtons( bp, Qt::white, "white", outputName, &stream, pg, npages,&vr );
					int nc = 2;

					QPixmap pic2 = drawButtons( bp, Qt::yellow, "yellow", outputhName, &stream, pg, npages,&vr );

					QPixmap pic3 = drawButtons( bp, Qt::red, "red", outputsName, &stream, pg, npages,&vr );

					// encoding bg to mpeg2
					outputMpeg.sprintf( "menu_base.m2v" );
					bgName.sprintf( "bg%04d.ppm", pg );
					stream << "ppmtoy4m -n 2 -r -I t -L -F 25:1 -A 59:54 ";
					stream << bgName << " -S 420mpeg2 ";
					stream << " | mpeg2enc -f 8 -o " << outputMpeg << endl;
					stream << "# rm -f " << bgName << endl;

					if ( pg > 0 )
					{
						stXml << "<pgc pause=\"0\" >" << endl;
					}
					stream << "echo Generating menu " << pg+1 << " of " << npages << endl;
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
					/*
					stream << "transparent=\"000000\" " << endl;
					stream << "autooutline=\"infer\"" << endl;
					stream << "outlinewidth=\"30\"" << endl;
					stream << "autoorder=\"rows\">" << endl;
					*/
					stream << " >" << endl;
					int x0 = 0, x1 = 0, y0 = 0, y1 = 0;
					for (int i = 0; i < vr.size(); i++ )
					{
						QRect r = vr[i];
						// turn values to even
						x0 = ((r.x()-1)/2)*2;
						y0 = ((r.y()-1)/2)*2;
						x1 = ((r.x()+r.width())/2)*2;
						y1 = ((r.y()+r.height())/2)*2;

						/*
						x0 = r.x();
						y0 = r.y();
						x1 = r.x()+r.width();
						y1 = r.y()+r.height();
						*/
						stream << "<button name=\"" << i+1 << "\" x0=\"";
						stream << x0 << "\" y0=\"" << y0;
						stream << "\" x1=\"" << x1;
						stream << "\" y1=\"" << y1<< "\" />";
						stream << endl;
                //<action [ name="name" ] />
					}

					stream << "</spu>" << endl;
					stream << "</stream>" << endl;
					stream << "</subpictures>' >" << outputXml << endl;
					stream << "mplex -f 8 -o /dev/stdout " << outputMpeg;
					stream << " menusnd.mp2 | ";
					stream << "spumux " << outputXml;
					stream << " > " << outputMenu << endl;
					stream << "[ -s " << outputMenu << " ] || exit 1" << endl;
					stream << "# rm -f " << outputName << endl;
					stream << "# rm -f " << outputhName << endl;
					stream << "# rm -f " << outputsName << endl;
					stream << "# rm -f " << outputXml << endl;

					stXml << "<vob file=\"" << outputMenu << "\" pause=\"inf\"></vob>" << endl;
					for ( int ct = 0; (ct < 4) && ( ((pg*4)+ct) < titles.count()); ct++ )
					{
						stXml << "<button>jump titleset " << (pg*4)+ct+1;
						stXml << " menu;</button>" << endl;
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
				stream << "# rm -f menusnd.mp2" << endl;

				stXml << "</menus>" << endl;
				stXml << "</vmgm>" << endl;
				stXml << "<titleset>" << endl;
				stXml << "<menus>" << endl;
				stXml << "<pgc pause=\"0\" >" << endl;
				stXml << "<pre>" << endl;
				stXml << "jump title 1;" << endl;
				stXml << "</pre>" << endl;
				stXml << "</pgc>" << endl;
				stXml << "</menus>" << endl;
				stXml << "<titles>" << endl;
			}
			else if ( item->text( ID_IDENT ) == CC_TITLE )
			{
				if ( ctr > 0 )
				{
					stXml << "</titles>" << endl;
					stXml << "</titleset>" << endl;
					stXml << "<titleset>" << endl;
					stXml << "<menus>" << endl;
					stXml << "<pgc pause=\"0\" >" << endl;
					stXml << "<pre>" << endl;
					stXml << "jump title 1;" << endl;
					stXml << "</pre>" << endl;
					stXml << "</pgc>" << endl;
					stXml << "</menus>" << endl;
					stXml << "<titles>" << endl;
				}
				if ( !it.current()->text(ID_ASPECT).isEmpty() )
				{
					stXml << "<video aspect=\"";
					stXml << it.current()->text(ID_ASPECT);
					stXml << "\"/>" << endl;
				}
				if ( !it.current()->text(ID_AUDIO1).isEmpty() )
				{
					stXml << "<audio lang=\"";
					stXml << it.current()->text(ID_AUDIO1);
					stXml << "\"/>" << endl;
				}
				else
				{
					stXml << "<audio lang=\"en\"/>" << endl;
				}
				if ( !it.current()->text(ID_AUDIO2).isEmpty() )
				{
					stXml << "<audio lang=\"";
					stXml << it.current()->text(ID_AUDIO2);
					stXml << "\"/>" << endl;
				}
				if ( !it.current()->text(ID_SUB1).isEmpty() )
				{
					stXml << "<subpicture lang=\"";
					stXml << it.current()->text(ID_SUB1);
					stXml << "\"/>" << endl;
				}
				if ( !it.current()->text(ID_SUB2).isEmpty() )
				{
					stXml << "<subpicture lang=\"";
					stXml << it.current()->text(ID_SUB2);
					stXml << "\"/>" << endl;
				}
				if ( !it.current()->text(ID_SUB3).isEmpty() )
				{
					stXml << "<subpicture lang=\"";
					stXml << it.current()->text(ID_SUB3);
					stXml << "\"/>" << endl;
				}
				if ( !it.current()->text(ID_SUB4).isEmpty() )
				{
					stXml << "<subpicture lang=\"";
					stXml << it.current()->text(ID_SUB4);
					stXml << "\"/>" << endl;
				}
				stXml << "<pgc>" << endl;
				it1 = it.current();
				it1++;
				while ( ( it1.current() ) &&
						( it1.current()->text(ID_IDENT) == CC_VIDEO ) )
				{
					stXml << "<vob file=\"" << it1.current()->text(ID_FILENAME);
					if ( !(it1.current()->text(ID_PICTURE).isEmpty()) )
					{
						stXml << "\" chapters=\"";
						stXml << it1.current()->text(ID_PICTURE).isEmpty();
					}
					stXml << "\"></vob>" << endl;
					it1++;
				}
				stXml << "<post>call vmgm menu entry title;</post>" << endl;
				stXml << "</pgc>" << endl;
				ctr++;
			}
			else if ( item->text( ID_IDENT ) == CC_VIDEO )
			{
			}
			it++;
		}
		/*
		else if ( item->text( ID_IDENT ) == CC_VIDEO )
		{

		}
		*/
		stXml << "</titles>" << endl;
		stXml << "</titleset>" << endl;
		stXml << "</dvdauthor>" << endl;

		stream << "dvdauthor -x dvd.xml" << endl;
		// Here we should clean the vob files, 
		// the dvd.xml file and the ouputMenu file, BUT
		// by no deleting them, user can rerun manually dvdauthor
		// if something went wrong and is not obliged to reencode
		// everything
		fEnc.close();
		fXml.close();
	}

	//makeMenu( titles );
	makeDvd();
}


QString dvdmenuWnd::stringListeEnd( QStringList &liste, int depart )
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

QPixmap dvdmenuWnd::drawMenu( QPixmap &fond, int currentpage, int totalpages )
{
	QPainter p;

	p.begin( &fond );
	p.setPen( QPen( white, 5 ) );
	p.setFont( font );

	QListViewItemIterator it( lvDVD );
	QListViewItem *item;
	int ct = 0;
	while ( it.current() )
	{
		item = it.current();
		if ( item->text( ID_IDENT ) == CC_DVD )
		{
			p.setPen( Qt::white );
			QFontMetrics fm( font );
			QRect titleRect = fm.boundingRect( item->text( ID_NAME ) );
			int x = ( 720-titleRect.width() ) / 2;
			p.drawText( x, 30, item->text( ID_NAME ) );
		}
		if ( ( ct / 4 ) == currentpage )
			break;
		if ( item->text( ID_IDENT ) == CC_TITLE )
		{
			ct++;
		}
		it++;
	}
	ct = 0;
	int origFontSize = font.pointSize();
	while ( it.current() )
	{
		item = it.current();
		if ( item->text( ID_IDENT ) == CC_TITLE )
		{
			QPixmap pmt( item->text( ID_PICTURE ) );
			p.drawPixmap( tabr[ ct ], pmt );
			ct++;
		}
		if ( ct == 4 ) break;
		it++;
	}


	p.end();
	return fond;
}

QPixmap &dvdmenuWnd::drawButtons( QPixmap &fond, QColor c, QString cname,
	QString fname, QTextStream *stream, int currentpage, int totalpages, std::vector<QRect> *vrects )
{
	QPainter p;

	p.begin( &fond );
	p.setPen( QPen( c, 8 ) );
	p.setFont( font );
	vrects->clear();

	QListViewItemIterator it( lvDVD );
	QListViewItem *item;
	int ct = 0;
	int origFontSize = font.pointSize();
	// move to the right page
	while ( it.current() )
	{
		item = it.current();
		if ( item->text( ID_IDENT ) == CC_DVD )
		{
		}
		if ( ( ct / 4 ) == currentpage )
			break;
		if ( item->text( ID_IDENT ) == CC_TITLE )
		{
			ct++;
		}
		it++;
	}
	ct = 0;
	if ( stream )
	{
		*stream << "convert -size 720x576 xc:transparent ";
		*stream << "-family \"" << font.family() << "\" ";
		*stream << "-fill " << cname << " ";
		*stream << "-stroke " << cname << " ";
		*stream << "-strokewidth 2 -colors 3 +antialias ";
	}
	while ( it.current() )
	{
		item = it.current();
		if ( item->text( ID_IDENT ) == CC_TITLE )
		{
			p.setPen( c );
			font.setPointSize(origFontSize);
			p.setFont( font );
			QFontMetrics fm( font );
			QRect titleRect = fm.boundingRect( item->text( ID_NAME ) );
			while( titleRect.width() > 280 )
			{
				font.setPointSize( font.pointSize() - 1 );
				p.setFont( font );
				fm = QFontMetrics( font );
				titleRect = fm.boundingRect( item->text( ID_NAME ) );
			}

			int x = ( tabr[ct].x() + tabr[ct].width()/2 ) - titleRect.width()/2;
			int y = tabr[ct].y()+tabr[ct].height() + font.pointSize() + 10;
			p.drawText( x, y, item->text( ID_NAME ) );

			if ( stream )
			{
				*stream << "-pointsize " << font.pointSize() << " ";
				*stream << "-draw \"text " << x << "," << y << " \\\"";
				*stream << item->text(ID_NAME) << "\\\"\" ";
			}


			QRect endrect( x, y-titleRect.height(), titleRect.width()+3, titleRect.height()+3 );
			vrects->push_back( endrect );
			ct++;
			if ( ct == 4 ) break;
		}
		it++;
	}
	
	// prev menu
	int ps = font.pointSize();
	font.setPointSize( ps - 8 );
	p.setFont( font );
	QFontMetrics fm( font );
	if ( currentpage > 0 )
	{
		QString prev( "<<Prev" );
		QRect prevRect = fm.boundingRect( prev );
		p.drawText( 30, 550, prev );

		if ( stream )
		{
			*stream << "-pointsize " << font.pointSize() << " ";
			*stream << "-draw \"text 30,550 " << " \\\"";
			*stream << prev << "\\\"\" ";
		}

		QRect endrect( 30, 550-prevRect.height(), 
				prevRect.width()+3, prevRect.height()+3 );
		vrects->push_back( endrect );
	}
	// next menu
	if ( currentpage != totalpages - 1 )
	{
		QString next( "Next>>" );
		QRect nextRect = fm.boundingRect( next );
		p.drawText( 600, 550, next );

		if ( stream )
		{
			*stream << "-pointsize " << font.pointSize() << " ";
			*stream << "-draw \"text 600,550 " << " \\\"";
			*stream << next << "\\\"\" ";
		}

		QRect endrect( 600, 550-nextRect.height(), 
				nextRect.width()+3, nextRect.height()+3 );
		vrects->push_back( endrect );
	}
	font.setPointSize( ps );
	p.end();
	if ( stream )
	{
		*stream << " -quality 100 " << fname << endl;
	}
	return fond;
}

void dvdmenuWnd::preview()
{
	if ( videoItem )
	{
		/*
		videoPreviewWnd *tpw = new videoPreviewWnd( lvDVD, this, "preview", Qt::WType_Dialog );
		tpw->setBgOnly( false );
		tpw->setPreview( true );
		QListViewItemIterator it( lvDVD );
		QListViewItem *item = 0;
		int ct = 0;
		while ( it.current() )
		{
			item = it.current();
			if ( item->text( ID_IDENT ) == CC_TITLE )
				ct++;
			it++;
		}
		if ( ct > 3 )
		{
			tpw->setTotalPages( 3 );
			tpw->setCurrentPage( 1 );	// to see the prev/next buttons
		}
		else
		{
			tpw->setTotalPages( 1 );
			tpw->setCurrentPage( 0 );
		}
		tpw->setFont( font );
		tpw->setBgColor( bgcolor );
		tpw->setBgPic( bgPic );
		setOptions( tpw );
		tpw->show();
		*/
		QListViewItemIterator it( lvDVD );
		int ct = 0;
		while ( it.current() )
		{
			if ( it.current()->text( ID_IDENT ) == CC_TITLE )
			{
				ct++;
			}
			it++;
		}
		// we show the first page
		int n = ct / 4;
		if ( ( ct % 4 ) != 0 )
		{
			n++;
		}
		QPixmap bg;
		if ( !bgPic.isEmpty() )
		{
			bg = QPixmap( bgPic );
			bg.resize( 720, 576 );
		}
		else if ( bgcolor.isValid() )
		{
			bg = QPixmap( 720, 576 );
			bg.fill( bgcolor );
		}
		else
		{
			bg = QPixmap::fromMimeSource( "black.jpg" );
		}
		std::vector<QRect> vr;
		bg = drawMenu( bg, 0, n );
		bg = drawButtons( bg, Qt::white, "white", "", 0, 0, n, &vr );

		previewWnd *pw = new previewWnd();
		pw->tlPixmap->setPixmap( bg );
		pw->show();
	}
}

void dvdmenuWnd::removeItem( )
{
	if ( videoItem )
	{
		delete videoItem;
		videoItem = 0;
	}
	else if ( titleItem )
	{
		delete titleItem;
		titleItem = 0;
	}
	else if ( dvdItem )
	{
		delete dvdItem;
		dvdItem = 0;
	}
}

void dvdmenuWnd::makeMenu( QStringList titles )
{
	if ( titles.count() > 0 )
	{
	}
}

void dvdmenuWnd::makeDvd( )
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

	/*
void dvdmenuWnd::setOptions( videoPreviewWnd *t )
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
	*/

void dvdmenuWnd::cleanAlbumNames(QStringList &titles)
{
	// Since we heavily use shell and quotes, both single and double,
	// let's clean title, just to be on the safe side
	for ( int i = 0; i < titles.count(); i++ )
	{
		QString &s = titles[i].replace( '\'', "" );
		titles[i] = s;
		s = titles[i].replace( '"', "" );
		titles[i] = s;
	}
}

void dvdmenuWnd::genMenuCommand( QStringList titles, QTextStream &stream, QString color, QString outputName, int pg, int npages )
{
	//convert -size 720x576 xc:transparent -pointsize 28 -fill red -stroke red -strokewidth 2 -colors 3 +antialias -draw "text 80,150 \"Album 1\"" -draw "text 80,200 \"Album 2\"" -quality 100 menuh.png
	int ctr;
	stream << "convert -size 720x576 xc:transparent -pointsize " << sbFontSize->value();
	stream << " -fill " << color << " -stroke " << color;
	stream << " -strokewidth 2 -colors 3 +antialias";
	for ( ctr = 0; (ctr < 8) && ( ((pg*8)+ctr) < titles.count()); ctr++ )
	{
		stream << " -draw 'text 80," << 100+ctr*50 << " \"";
		stream << titles[(pg*8)+ctr] << "\"'";
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

void dvdmenuWnd::selectBgPic( )
{
	bgPic = QFileDialog::getOpenFileName( "/usr", "Images (*.png *.xpm *.jpg)",
		this,
		"open file dialog",
		"Choose a background image for the menu" );
}

void dvdmenuWnd::selectSound( )
{
	menuSound = QFileDialog::getOpenFileName( workingDir, "*.mp3",
		this,
		"open file dialog",
		"Choose a sound for the menu" );
}

