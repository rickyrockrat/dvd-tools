#include "qt4dvdmenuwnd.h"

#include <QStringList>
#include <QFileDialog>
#include <QColorDialog>
#include <QFontDialog>
#include <QMessageBox>
#include <QtXml/QDomDocument>
#include <QPainter>

#include "qt4newtitlewnd.h"
#include "qt4previewwnd.h"
#include "qt4encodewnd.h"

qt4dvdmenuWnd::qt4dvdmenuWnd( QWidget *parent )
	: QMainWindow( parent )
{
	setupUi(this);
	lvDVD->clear();
	lvDVD->setSortingEnabled(false);
	dvdItem = 0;
	titleItem = 0;
	videoItem = 0;
	procEncode = 0;
	mpginfoProc = 0;
	bgcolor = Qt::black;
	titleCtr = 0;
	videoDir = QDir::currentPath();
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
	connect( lvDVD, SIGNAL(clicked(QTreeWidgetItem *)),
	this, SLOT(lvClicked(QTreeWidgetItem*)));

	pbOccup->setMaximum( 4400000 );
	r1 = QRect( 50, 70, 230, 180 );
	r2 = QRect( 350, 70, 230, 180 );
	r3 = QRect( 50, 320, 230, 180 );
	r4 = QRect( 350, 320, 230, 180 );
	tabr[0] = r1;
	tabr[1] = r2;
	tabr[2] = r3;
	tabr[3] = r4;
}

void qt4dvdmenuWnd::newDVD()
{
	lvDVD->clear();
	lvDVD->setColumnCount( 10 );
	QStringList heads;

	heads << "ident";
	heads << "picture/chapters";
	heads << "aspect";
	heads << "audio1";
	heads << "audio2";
	heads << "sub1";
	heads << "sub2";
	heads << "sub3";
	heads << "sub4";
	heads << "filename";
	lvDVD->setHeaderLabels( heads );
	lvDVD->setRootIsDecorated( true );
	QStringList root;
	root << "New DVD" << CC_DVD;
	dvdItem = new QTreeWidgetItem( lvDVD, root );
	//FIXME dvdItem->setRenameEnabled( 0, true );
	//FIXME dvdItem->setOpen( true );

	workingDir = QFileDialog::getExistingDirectory( this,
			"Choose the temporary directory",
			QDir::currentPath() );
}

void qt4dvdmenuWnd::addTitle()
{
	qt4newtitleWnd *nt = new qt4newtitleWnd(this);

	if ( nt->exec() == QDialog::Accepted )
	{
		QStringList a;
		a << nt->title;
		a << CC_TITLE;
		a << nt->snapshot;
		a << nt->aspect;
		a << nt->audio1;
		a << nt->audio2;
		a << nt->sub1;
		a << nt->sub2;
		a << nt->sub3;
		a << nt->sub4;

		if ( titleItem )
			titleItem = new QTreeWidgetItem( titleItem, a);
		else
			titleItem = new QTreeWidgetItem( dvdItem, a);
		/* FIXME
		titleItem->setRenameEnabled( ID_NAME, true );
		titleItem->setRenameEnabled( ID_PICTURE, true );
		titleItem->setRenameEnabled( ID_ASPECT, true );
		titleItem->setRenameEnabled( ID_AUDIO1, true );
		titleItem->setRenameEnabled( ID_AUDIO2, true );
		titleItem->setRenameEnabled( ID_SUB1, true );
		titleItem->setRenameEnabled( ID_SUB2, true );
		titleItem->setRenameEnabled( ID_SUB3, true );
		titleItem->setRenameEnabled( ID_SUB4, true );
		titleItem->setOpen( true );
		*/

	}
}

void qt4dvdmenuWnd::lvClicked( QTreeWidgetItem *item )
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

void qt4dvdmenuWnd::addVideo()
{
	QFileDialog fd( this, "Add tracks", videoDir, "*.vob ; *.mpg" );
	fd.setFileMode( QFileDialog::ExistingFiles );
	if ( fd.exec() == QDialog::Accepted )
	{
		QStringList l = fd.selectedFiles();
		QStringList::Iterator it;
		for ( it = l.begin(); it != l.end(); it++ )
		{
			QStringList tra;
			tra << *it;
			tra << CC_VIDEO;
			tra << *it;
			if ( videoItem )
				videoItem = new QTreeWidgetItem( videoItem, tra );
			else
				videoItem = new QTreeWidgetItem( titleItem, tra );

			QFileInfo fi( *it );
			pbOccup->setValue( pbOccup->value()+int(fi.size()*1.2/1024) );
			videoDir = fi.absolutePath();
		}
	}
}

void qt4dvdmenuWnd::selectBgColor()
{
	bgcolor = QColorDialog::getColor();
}

void qt4dvdmenuWnd::selectFont()
{
	bool ok;
	font = QFontDialog::getFont(&ok, font );
}

void qt4dvdmenuWnd::newProject()
{
}

void qt4dvdmenuWnd::openProject()
{

	QString s = QFileDialog::getOpenFileName(
                    this,
                    "Choose a file to open",
                    workingDir,
                    "Xml (*.xml)"
		    );
 	if ( !s.isEmpty() )
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
			QDomAttr dvdname = docElem.attributeNode( "TMPDIR" );
			workingDir = QString/*::fromUtf8*/( dvdname.value() );
			dvdname = docElem.attributeNode( "MENUMUSIC" );
			menuSound = QString/*::fromUtf8*/( dvdname.value() );
			dvdname = docElem.attributeNode( "MENUBG" );
			bgPic = QString/*::fromUtf8*/( dvdname.value() );

			dvdname = docElem.attributeNode( "NAME" );
			//std::cout << "dvd=" << dvdname.value() << std::endl;
			lvDVD->setColumnCount( 10 );
			QStringList heads;

			heads << "ident";
			heads << "picture/chapters";
			heads << "aspect";
			heads << "audio1";
			heads << "audio2";
			heads << "sub1";
			heads << "sub2";
			heads << "sub3";
			heads << "sub4";
			heads << "filename";
			lvDVD->setHeaderLabels( heads );
			lvDVD->setRootIsDecorated( true );
			QStringList root;
			root << dvdname.value();
			root << CC_DVD;
			dvdItem = new QTreeWidgetItem( lvDVD, root );
			/*FIXME dvdItem->setRenameEnabled( ID_NAME, true );
			dvdItem->setOpen(true);
			*/

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
						QStringList t;
						t <<  titlename.value(); 
						t << CC_TITLE;
						if ( titleItem )
							titleItem = new QTreeWidgetItem( titleItem, t );
						else
							titleItem = new QTreeWidgetItem( dvdItem, t );
						/*
						titleItem->setRenameEnabled( ID_NAME, true );
						titleItem->setRenameEnabled( ID_PICTURE, true );
						titleItem->setRenameEnabled( ID_ASPECT, true );
						titleItem->setRenameEnabled( ID_AUDIO1, true );
						titleItem->setRenameEnabled( ID_AUDIO2, true );
						titleItem->setRenameEnabled( ID_SUB1, true );
						titleItem->setRenameEnabled( ID_SUB2, true );
						titleItem->setRenameEnabled( ID_SUB3, true );
						titleItem->setRenameEnabled( ID_SUB4, true );
						*/

						att = e.attributeNode( "ID_PICTURE" );
						titleItem->setText( ID_PICTURE, QString/*::fromUtf8*/( att.value()) );
						att = e.attributeNode( "ID_ASPECT" );
						titleItem->setText( ID_ASPECT, QString/*::fromUtf8*/( att.value()) );
						att = e.attributeNode( "ID_AUDIO1" );
						titleItem->setText( ID_AUDIO1, QString/*::fromUtf8*/( att.value()) );
						att = e.attributeNode( "ID_AUDIO2" );
						titleItem->setText( ID_AUDIO2, QString/*::fromUtf8*/( att.value()));
						att = e.attributeNode( "ID_SUB1" );
						titleItem->setText( ID_SUB1, QString/*::fromUtf8*/( att.value()));
						att = e.attributeNode( "ID_SUB2" );
						titleItem->setText( ID_SUB2, QString/*::fromUtf8*/( att.value() ));
						att = e.attributeNode( "ID_SUB3" );
						titleItem->setText( ID_SUB3, QString/*::fromUtf8*/( att.value() ));
						att = e.attributeNode( "ID_SUB4" );
						titleItem->setText( ID_SUB4, QString/*::fromUtf8*/( att.value()));

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
										videoItem = new QTreeWidgetItem( videoItem, QStringList/*::fromUtf8*/( att.value() ) );
									else
										videoItem = new QTreeWidgetItem( titleItem, QStringList/*::fromUtf8*/( att.value() ) );
									videoItem->setText( ID_IDENT, CC_VIDEO ); 

									//videoItem->setRenameEnabled( ID_NAME, true );
									att = e1.attributeNode( "ID_CHAPTERS" );
									videoItem->setText( ID_PICTURE, QString/*::fromUtf8*/( att.value()));
									//videoItem->setRenameEnabled( ID_PICTURE, true );
									att = e1.attributeNode( "ID_FILENAME" );
									videoItem->setText( ID_FILENAME, QString/*::fromUtf8*/( att.value()) );
									//titleItem->setOpen( true );
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

void qt4dvdmenuWnd::saveProject()
{
	if ( dvdItem )
	{
		QDomDocument doc( "DvD" );
		QDomElement rootElem = doc.createElement( "DVD" );
		rootElem.setAttribute( "NAME", dvdItem->text(ID_NAME)/*.utf8()*/ );
		rootElem.setAttribute( "TMPDIR", workingDir/*.utf8()*/ );
		rootElem.setAttribute( "MENUMUSIC", menuSound/*.utf8()*/ );
		rootElem.setAttribute( "MENUBG", bgPic/*.utf8()*/ );
		doc.appendChild( rootElem );

		QTreeWidgetItemIterator it( lvDVD );
		QTreeWidgetItem *item;
		QDomElement el;
		QDomElement titleEl;
		int ctr = 0;

		while ( *it )
		{
			item = *it;
			if ( item->text( ID_IDENT ) == CC_TITLE )
			{

				titleEl = doc.createElement( "TITLE" );
				titleEl.setAttribute( "NAME", item->text( ID_NAME )/*.utf8()*/ );
				titleEl.setAttribute( "ID_PICTURE", item->text( ID_PICTURE )/*.utf8()*/ );
				titleEl.setAttribute( "ID_ASPECT", item->text( ID_ASPECT )/*.utf8()*/ );
				titleEl.setAttribute( "ID_AUDIO1", item->text( ID_AUDIO1 )/*.utf8()*/ );
				titleEl.setAttribute( "ID_AUDIO2", item->text( ID_AUDIO2 )/*.utf8()*/ );
				titleEl.setAttribute( "ID_SUB1", item->text( ID_SUB1 )/*.utf8()*/ );
				titleEl.setAttribute( "ID_SUB2", item->text( ID_SUB2 )/*.utf8()*/ );
				titleEl.setAttribute( "ID_SUB3", item->text( ID_SUB3 )/*.utf8()*/ );
				titleEl.setAttribute( "ID_SUB4", item->text( ID_SUB4 )/*.utf8()*/ );
				rootElem.appendChild( titleEl );
			}
			else if ( item->text( ID_IDENT ) == CC_VIDEO )
			{
				el = doc.createElement( "VIDEO" );
				el.setAttribute( "ID_NAME", item->text( ID_NAME )/*.utf8()*/ );
				el.setAttribute( "ID_CHAPTERS", item->text( ID_PICTURE )/*.utf8()*/ );
				el.setAttribute( "ID_FILENAME", item->text(ID_FILENAME)/*.utf8()*/ );
				titleEl.appendChild( el );
			}
			it++;
		}
		QString s = QFileDialog::getSaveFileName(
				this,
				"Choose a filename to save under",
				workingDir,
				"Xml (*.xml)" );
		QFile f( s );
		if ( !f.open(QIODevice::WriteOnly) )
		{
			QMessageBox::information( this, "error", "Cannot open file" );
		}
		QTextStream st( &f );
		st << doc.toString()/*.utf8()*/;
		f.close();
	}
}

void qt4dvdmenuWnd::encode()
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
	QString bgName;
	QString outputName;
	QString outputMp2;
	QString pal( " -F 25:1 -A 59:54 " );
	QString npal( "25" );
	//videoPreviewWnd *tpw;
	QStringList titles;
	while ( (*it1) )
	{
		item = (*it1);
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
	if ( ( fEnc.open( QIODevice::WriteOnly ) )
		&& ( fXml.open( QIODevice::WriteOnly ) ) )
	{
		QTextStream stream( &fEnc );
		QTextStream stXml( &fXml );
		while ( (*it) )
		{
			item = *it;
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
						bg = QPixmap( bgPic ).scaled( QSize( 720, 576 ) );
					}
					else if ( bgcolor.isValid() )
					{
						bg = QPixmap( 720, 576 );
						bg.fill( bgcolor );
					}
					else
					{
						bg = QPixmap( ":/black.jpg" );
					}
					name.sprintf( "bg%04d.ppm", pg );
					QPixmap pic0 = drawMenu( bg, pg, npages );
					QImage img0 = pic0.toImage();
					img0.convertToFormat( QImage::Format_Indexed8 ).save( name, "PPM" );

					QString outputName, outputsName, outputhName, outputXml, outputMenu;
					outputName.sprintf( "picN%04d.png", pg );
					outputhName.sprintf( "picH%04d.png", pg );
					outputsName.sprintf( "picS%04d.png", pg );
		
					QPixmap bp1 = QPixmap( ":/black.jpg" );
					std::vector<QRect> vr;

					QRgb pal[ 4 ];
					pal[ 0 ] = QColor( Qt::black ).rgb();
					pal[ 1 ] = QColor( Qt::white ).rgb();
					pal[ 2 ] = QColor( Qt::yellow ).rgb();
					pal[ 3 ] = QColor( Qt::blue ).rgb();
					QPixmap pic1 = drawButtons( bp1, Qt::white, pg, npages,&vr );
					//QImage img1 = pic1.convertToImage().convertDepth( 8 );
					QImage img1 = pic1.toImage();
					clipColors( img1, Qt::white );
					img1.save( outputName , "PNG" );

					int nc = 2;

					QPixmap bp2 = QPixmap( ":/black.jpg" );
					QPixmap pic2 = drawButtons( bp2, Qt::yellow, pg, npages,&vr );
					QImage img2 = pic2.toImage();
					clipColors( img2, Qt::yellow );
					img2.save( outputhName , "PNG" );

					QPixmap bp3 = QPixmap( ":/black.jpg" );
					QPixmap pic3 = drawButtons( bp3, Qt::blue, pg, npages,&vr );
					QImage img3 = pic3.toImage();
					clipColors( img3, Qt::blue );
					img3.save( outputsName , "PNG" );

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
					stream << "transparent=\"000000\" " << endl;
					/*
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
				if ( !(*it)->text(ID_ASPECT).isEmpty() )
				{
					stXml << "<video aspect=\"";
					stXml << (*it)->text(ID_ASPECT);
					stXml << "\"/>" << endl;
				}
				if ( !(*it)->text(ID_AUDIO1).isEmpty() )
				{
					stXml << "<audio lang=\"";
					stXml << (*it)->text(ID_AUDIO1);
					stXml << "\"/>" << endl;
				}
				else
				{
					stXml << "<audio lang=\"en\"/>" << endl;
				}
				if ( !(*it)->text(ID_AUDIO2).isEmpty() )
				{
					stXml << "<audio lang=\"";
					stXml << (*it)->text(ID_AUDIO2);
					stXml << "\"/>" << endl;
				}
				if ( !(*it)->text(ID_SUB1).isEmpty() )
				{
					stXml << "<subpicture lang=\"";
					stXml << (*it)->text(ID_SUB1);
					stXml << "\"/>" << endl;
				}
				if ( !(*it)->text(ID_SUB2).isEmpty() )
				{
					stXml << "<subpicture lang=\"";
					stXml << (*it)->text(ID_SUB2);
					stXml << "\"/>" << endl;
				}
				if ( !(*it)->text(ID_SUB3).isEmpty() )
				{
					stXml << "<subpicture lang=\"";
					stXml << (*it)->text(ID_SUB3);
					stXml << "\"/>" << endl;
				}
				if ( !(*it)->text(ID_SUB4).isEmpty() )
				{
					stXml << "<subpicture lang=\"";
					stXml << (*it)->text(ID_SUB4);
					stXml << "\"/>" << endl;
				}
				stXml << "<pgc>" << endl;
				it1 = it;
				it1++;
				while ( ( *it1 ) &&
						( (*it1)->text(ID_IDENT) == CC_VIDEO ) )
				{
					stXml << "<vob file=\"" << (*it1)->text(ID_FILENAME);
					if ( !(*it1)->text(ID_PICTURE).isEmpty())
					{
						stXml << "\" chapters=\"";
						stXml << (*it1)->text(ID_PICTURE).isEmpty();
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


QString qt4dvdmenuWnd::stringListeEnd( QStringList &liste, int depart )
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

QPixmap qt4dvdmenuWnd::drawMenu( QPixmap &fond, int currentpage, int totalpages )
{
	QPainter p;

	p.begin( &fond );
	p.setPen( QPen( Qt::white, 5 ) );
	p.setFont( font );

	QTreeWidgetItemIterator it( lvDVD );
	QTreeWidgetItem *item;
	int ct = 0;
	while ( (*it) )
	{
		item = (*it);
		if ( item->text( ID_IDENT ) == CC_DVD )
		{
			p.setPen( Qt::white );
			QFontMetrics fm( font );
			QRect titleRect = fm.boundingRect( item->text( ID_NAME ) );
			int x = ( 720-titleRect.width() ) / 2;
			p.drawText( x, 30+titleRect.height(), item->text( ID_NAME ) );
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
	while ( (*it) )
	{
		item = (*it);
		if ( item->text( ID_IDENT ) == CC_TITLE )
		{
			QPixmap pmt( item->text( ID_PICTURE ) );
			double aspect = (double)pmt.width() / (double)pmt.height();
			QRect newr;
			if ( aspect > 1 )
			{
				newr = QRect( tabr[ ct ].x(),
								tabr[ ct ].y(),
								tabr[ ct ].width() / aspect,
								tabr[ ct ].height() );
			}
			else
			{
				newr = QRect( tabr[ ct ].x(),
								tabr[ ct ].y(),
								tabr[ ct ].width() * aspect,
								tabr[ ct ].height() );
			}
			p.drawPixmap( newr, pmt );
			ct++;
		}
		if ( ct == 4 ) break;
		it++;
	}


	p.end();
	return fond;
}

QPixmap &qt4dvdmenuWnd::drawButtons( QPixmap &fond, QColor c, 
	int currentpage, int totalpages, std::vector<QRect> *vrects )
{
	QPainter p;

	p.begin( &fond );
	p.setPen( QPen( c, 8 ) );
	p.setBrush( QBrush( c, Qt::SolidPattern ) );
	p.setFont( font );
	vrects->clear();

	QTreeWidgetItemIterator it( lvDVD );
	QTreeWidgetItem *item;
	int ct = 0;
	int origFontSize = font.pointSize();
	// move to the right page
	while ( (*it) )
	{
		item = (*it);
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
	while ( (*it) )
	{
		item = (*it);
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


		QRect endrect( 600, 550-nextRect.height(), 
				nextRect.width()+3, nextRect.height()+3 );
		vrects->push_back( endrect );
	}
	font.setPointSize( ps );
	p.end();
	return fond;
}

void qt4dvdmenuWnd::preview()
{
	if ( videoItem )
	{
		/*
		videoPreviewWnd *tpw = new videoPreviewWnd( lvDVD, this, "preview", Qt::WType_Dialog );
		tpw->setBgOnly( false );
		tpw->setPreview( true );
		QTreeWidgetItemIterator it( lvDVD );
		QTreeWidgetItem *item = 0;
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
		QTreeWidgetItemIterator it( lvDVD );
		int ct = 0;
		while ( (*it) )
		{
			if ( (*it)->text( ID_IDENT ) == CC_TITLE )
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
			bg = QPixmap( bgPic ).scaled( QSize( 720, 576 ) );
		}
		else if ( bgcolor.isValid() )
		{
			bg = QPixmap( 720, 576 );
			bg.fill( bgcolor );
		}
		else
		{
			bg = QPixmap( ":/black.jpg" );
		}
		std::vector<QRect> vr;
		bg = drawMenu( bg, 0, n );
		bg = drawButtons( bg, Qt::white, 0, n, &vr );

		qt4previewWnd *pw = new qt4previewWnd();
		pw->tlPixmap->setPixmap( bg );
		pw->show();
	}
}

void qt4dvdmenuWnd::removeItem( )
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

void qt4dvdmenuWnd::makeMenu( QStringList titles )
{
	if ( titles.count() > 0 )
	{
	}
}

void qt4dvdmenuWnd::makeDvd( )
{
	QProcess *dvdauthorProc = new QProcess( this );
	QStringList args( "menu.sh" );
	dvdauthorProc->start( "bash", args);
	if ( !dvdauthorProc->waitForStarted( ) )
	{
		QMessageBox::information( this, "DvdAuthor",
				"Problem authoring DVD" );
	}
	qt4encodeWnd *encoding = new qt4encodeWnd( this );
	encoding->procEncode = dvdauthorProc;
	encoding->demarrerA( );
	if ( dvdauthorProc->exitStatus() != QProcess::NormalExit )
	{
		QMessageBox::information( this, "Problem",
			"authoring error for file " );
	}
	delete dvdauthorProc;
}

	/*
void qt4dvdmenuWnd::setOptions( videoPreviewWnd *t )
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

void qt4dvdmenuWnd::cleanAlbumNames(QStringList &titles)
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

void qt4dvdmenuWnd::genMenuCommand( QStringList titles, QTextStream &stream, QString color, QString outputName, int pg, int npages )
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

void qt4dvdmenuWnd::selectBgPic( )
{
	bgPic = QFileDialog::getOpenFileName(
		this,
		"Choose a background image for the menu",
		"/usr",
		"Images (*.png *.xpm *.jpg)"
		);
}

void qt4dvdmenuWnd::selectSound( )
{
	menuSound = QFileDialog::getOpenFileName(
		this,
		"Choose a sound for the menu",
		workingDir,
		"*.mp3"
		);
}

void qt4dvdmenuWnd::clipColors( QImage &img, const QColor &destcol )
{
//return;
	QRgb destrgb = destcol.rgb();
	int clip = 0;
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
				img.setPixel( x, y, QColor( Qt::black ).rgb() );
			}
			else
				img.setPixel( x, y, destrgb );
		}
	}
	img.setNumColors( 3 );
}

// vim: nows nosm ts=4 sw=4

