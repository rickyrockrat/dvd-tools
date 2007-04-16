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

#include <qmessagebox.h>
#include <qfiledialog.h>
#include <iostream>

#define ITEM_EUROSPORT		0
#define ITEM_EUROSPORT2		1

void recWnd::init()
{
	cbChaine->clear();
	cbChaine->insertItem( "Eurosport" );
	cbChaine->insertItem( "Eurosport 2" );

	connect( pbSelFile, SIGNAL(clicked()),
				this, SLOT(selFile()) );
	connect( pbValider, SIGNAL(clicked()),
				this, SLOT(newRec()) );
	connect( pbAnnuler, SIGNAL(clicked()),
				this, SLOT(cancelRec()) );
	connect( pbQuit, SIGNAL(clicked()),
				this, SLOT(close()) );

	dteDebut->setDateTime( QDateTime::currentDateTime() );
	teDuree->setTime( QTime( 1, 0 ) );
}

void recWnd::selFile()
{
	QString s = QFileDialog::getSaveFileName();
	leFile->setText( s );
}

void recWnd::newRec()
{
	if ( leFile->text().isEmpty() )
	{
		QMessageBox::information( this, "Enregistreur", 
						"Entrez un nom de fichier !" );
		return;
	}
	QString url;
	if ( cbChaine->currentItem() == ITEM_EUROSPORT )
	{
		url = "http://player.eurosport.fr/playlist.aspx?mode=live&id=6";
	}
	else if ( cbChaine->currentItem() == ITEM_EUROSPORT2 )
	{
		url = "http://player.eurosport.fr/playlist.aspx?mode=live&id=7";
	}
	else
	{
		return;
	}
	QFile tmpf( "/tmp/rectmp" );
	tmpf.open( IO_WriteOnly );
	QTextStream stream( &tmpf );
	stream.setEncoding( QTextStream::UnicodeUTF8 );
	stream << "mplayer -vf harddup -playlist \"";
	stream << url << "\" -cache 1024 -cache-min 90 -o ";
	stream << leFile->text() << " & " << endl;
	QTime t = teDuree->time();
	int secs  = t.hour() * 3600 + t.minute() * 60 + t.second();
	stream << "sleep " << secs << endl;
	stream << "kill $!" << endl;

	tmpf.close();

	QProcess p;
	p.addArgument( "at" );
	p.addArgument( "-f" );
	p.addArgument( "/tmp/rectmp" );
	std::cout << dteDebut->dateTime().toString().ascii() << std::endl;
	QTime dt = dteDebut->dateTime().time();
	QDate dd = dteDebut->dateTime().date();
	QString du;
	du.sprintf( "%02d:%02d %02d/%02d/%02d", dt.hour(), dt.minute(), dd.month(), dd.day(), dd.year() );
	p.addArgument( du);

	p.start();

	while ( p.isRunning() )
	{
		sleep( 2 );
	}
	if ( !p.normalExit() )
	{
		QMessageBox::information( this, "Problem",
								"error running at command " );
	}
	refreshProgs();
}

void recWnd::refreshProgs()
{
	lvPrevus->clear();

	pAtq = new QProcess;
	pAtq->addArgument( "atq" );
	connect( pAtq, SIGNAL(processExited()),
				this, SLOT(readAtq()));
	pAtq->start();
}

void recWnd::readAtq()
{
	QString line = pAtq->readLineStdout();
	while ( !line.isNull() )
	{
		item = new QListViewItem( lvPrevus, item, line );
		line = pAtq->readLineStdout();
	}
	delete pAtq;
	pAtq = 0;
}

void recWnd::itemSel()
{
	if ( lvPrevus->currentItem() )
	{
		pbAnnuler->setEnabled( true );
	}
	else
	{
		pbAnnuler->setEnabled( false );
	}
}

void recWnd::cancelRec()
{
	QListViewItem * i = lvPrevus->currentItem();
	if ( i )
	{
		QString id = i->text(0).section( " ", 0, 0 );
		QProcess p;
		p.addArgument( "atrm" );
		p.addArgument( id );
		p.start();
		refreshProgs();
	}
}
