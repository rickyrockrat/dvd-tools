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
			workingDir = dvdname.value();
			dvdname = docElem.attributeNode( "MENUMUSIC" );
			menuSound = dvdname.value();
			dvdname = docElem.attributeNode( "MENUBG" );
			bgPic = dvdname.value();

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
						if ( titleItem )
							titleItem = new QListViewItem( dvdItem, titleItem, titlename.value(), CC_TITLE );
						else
							titleItem = new QListViewItem( dvdItem, titlename.value(), CC_TITLE );
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
						titleItem->setText( ID_PICTURE, att.value() );
						att = e.attributeNode( "ID_ASPECT" );
						titleItem->setText( ID_ASPECT, att.value() );
						att = e.attributeNode( "ID_AUDIO1" );
						titleItem->setText( ID_AUDIO1, att.value() );
						att = e.attributeNode( "ID_AUDIO2" );
						titleItem->setText( ID_AUDIO2, att.value());
						att = e.attributeNode( "ID_SUB1" );
						titleItem->setText( ID_SUB1, att.value());
						att = e.attributeNode( "ID_SUB2" );
						titleItem->setText( ID_SUB2, att.value() );
						att = e.attributeNode( "ID_SUB3" );
						titleItem->setText( ID_SUB3, att.value() );
						att = e.attributeNode( "ID_SUB4" );
						titleItem->setText( ID_SUB4, att.value());

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
										videoItem = new QListViewItem( titleItem, videoItem, att.value() );
									else
										videoItem = new QListViewItem( titleItem, att.value() );
									videoItem->setText( ID_IDENT, CC_VIDEO ); 

									videoItem->setRenameEnabled( ID_NAME, true );
									att = e1.attributeNode( "ID_CHAPTERS" );
									videoItem->setText( ID_PICTURE, att.value());
									videoItem->setRenameEnabled( ID_PICTURE, true );
									att = e1.attributeNode( "ID_FILENAME" );
									videoItem->setText( ID_FILENAME, att.value());
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
		rootElem.setAttribute( "NAME", dvdItem->text(ID_NAME) );
		rootElem.setAttribute( "TMPDIR", workingDir );
		rootElem.setAttribute( "MENUMUSIC", menuSound );
		rootElem.setAttribute( "MENUBG", bgPic );
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
				titleEl.setAttribute( "NAME", item->text( ID_NAME ) );
				titleEl.setAttribute( "ID_PICTURE", item->text( ID_PICTURE ) );
				titleEl.setAttribute( "ID_ASPECT", item->text( ID_ASPECT ) );
				titleEl.setAttribute( "ID_AUDIO1", item->text( ID_AUDIO1 ) );
				titleEl.setAttribute( "ID_AUDIO2", item->text( ID_AUDIO2 ) );
				titleEl.setAttribute( "ID_SUB1", item->text( ID_SUB1 ) );
				titleEl.setAttribute( "ID_SUB2", item->text( ID_SUB2 ) );
				titleEl.setAttribute( "ID_SUB3", item->text( ID_SUB3 ) );
				titleEl.setAttribute( "ID_SUB4", item->text( ID_SUB4 ) );
				rootElem.appendChild( titleEl );
			}
			else if ( item->text( ID_IDENT ) == CC_VIDEO )
			{
				el = doc.createElement( "VIDEO" );
				el.setAttribute( "ID_NAME", item->text( ID_NAME ) );
				el.setAttribute( "ID_CHAPTERS", item->text( ID_PICTURE ) );
				el.setAttribute( "ID_FILENAME", item->text(ID_FILENAME) );
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
	videoPreviewWnd *tpw;
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
					tpw = new videoPreviewWnd( lvDVD, this, "preview", Qt::WType_Dialog );
					tpw->setTotalPages( npages );
					tpw->setCurrentPage( pg );
					tpw->setBgPic( bgPic );
					tpw->setFont( font );
					tpw->setBgColor( bgcolor );
					setOptions( tpw );
					tpw->setBgOnly( true );

					name.sprintf( "bg%04d.ppm", pg );
					QPixmap pic0 = QPixmap::grabWidget( tpw );
					QImage img0 = pic0.convertToImage();
					img0.convertDepth( 8 );
					img0.save( name, "PPM" );

					tpw->setBgOnly( false );

					name.sprintf( "picN%04d.png", pg );
					tpw->setFgColor( Qt::white );
					QPixmap pic1 = QPixmap::grabWidget( tpw );
					QImage img1 = pic1.convertToImage();
					img1.convertDepth( 8 );
					img1.save( name, "PNG" );
					// convert buttons to 3 colors
					stream << "convert " << name << " -colors 3 " << name << endl;

					name.sprintf( "picH%04d.png", pg );
					tpw->setFgColor( Qt::yellow );
					QPixmap pic2 = QPixmap::grabWidget( tpw );
					QImage img2 = pic2.convertToImage();
					img2.convertDepth( 8 );
					img2.save( name, "PNG" );
					stream << "convert " << name << " -colors 3 " << name << endl;

					name.sprintf( "picS%04d.png", pg );
					tpw->setFgColor( Qt::red );
					QPixmap pic3 = QPixmap::grabWidget( tpw );
					QImage img3 = pic3.convertToImage();
					img3.convertDepth( 8 );
					img3.save( name, "PNG" );
					stream << "convert " << name << " -colors 3 " << name << endl;

					delete tpw;
					QString outputName, outputsName, outputhName, outputXml, outputMenu;

							
					// encoding bg to mpeg2
					outputMpeg.sprintf( "menu_base.m2v" );
					bgName.sprintf( "bg%04d.ppm", pg );
					stream << "ppmtoy4m -n 1 -F25:1 -A 59:54 ";
					stream << bgName << " -S 420mpeg2 ";
					stream << " | mpeg2enc -f 8 -o " << outputMpeg << endl;
					stream << "# rm -f " << bgName << endl;

					if ( pg > 0 )
					{
						stXml << "<pgc pause=\"0\" >" << endl;
					}
					stream << "echo Generating menu " << pg << " of " << npages << endl;
					outputName.sprintf( "picN%04d.png", pg );
					outputhName.sprintf( "picH%04d.png", pg );
					outputsName.sprintf( "picS%04d.png", pg );
		
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
					stream << "transparent=\"000000\"" << endl;
					stream << "autooutline=\"infer\"" << endl;
					stream << "outlinewidth=\"30\"" << endl;
					stream << "autoorder=\"rows\">" << endl;
					stream << "</spu>" << endl;
					stream << "</stream>" << endl;
					stream << "</subpictures>' >" << outputXml << endl;
					stream << "mplex -f 8 -o /dev/stdout " << outputMpeg;
					stream << " menusnd.mp2 | ";
					stream << "spumux " << outputXml;
					stream << " > " << outputMenu << endl;
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

void dvdmenuWnd::preview()
{
	if ( videoItem )
	{
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

void dvdmenuWnd::setOptions( videoPreviewWnd *t )
{
	/*
	t->setDrawGenre( cbGenre->isChecked() );
	t->setDrawArtist( cbArtist->isChecked() );
	t->setDrawAlbum( cbAlbum->isChecked() );
	t->setDrawYear( cbYear->isChecked() );
	t->setDrawTitle( cbTitle->isChecked() );
	t->setDrawTrack( cbTrack->isChecked() );
	t->setDrawDuration( cbDuration->isChecked() );
	t->setDrawBitrate( cbBitrate->isChecked() );
	*/
}

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

