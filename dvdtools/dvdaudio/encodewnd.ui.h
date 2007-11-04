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
#include <iostream>

void encodeWnd::readAuthor()
{
	if ( procEncode )
	{
		teAuthor->append( procEncode->readStderr() );
	}
}

void encodeWnd::readEncMpeg()
{
	if ( procEncode )
	{
		QString in;
		const QString sp(" ");
		int frames;
		in = QString( procEncode->readStderr() );
		QStringList l1 = QStringList::split( "INFO: [mpeg2enc]", in );
		QStringList pos = QStringList::split( sp, l1.join("") );
		pbEncode->setProgress(pos[1].toInt() );
	}
}

void encodeWnd::readEncode()
{
	if ( procEncode )
	{
		QString in;
		const QString sp(" ");
		int frames;
		in = QString( procEncode->readStdout() );
		QStringList l = QStringList::split( "Pos:", in );
		QStringList pos = QStringList::split( sp, l.join("") );
		pbEncode->setProgress(pos[1].section( "f", 0, 0 ).toInt() );
	}
}
//INFO: [mpeg2enc] Frame 16399 16399 P q=112.00 sum act=477462.15818

void encodeWnd::demarrer(QString fn, int tf )
{
	cbKeep->setChecked( false);
	cbKeep->hide();
	pbClose->hide();
	setCaption( "Encoding to vob" );
	setModal( true );
	teAuthor->setMaxLogLines(1000);
	teAuthor->clear();
	teAuthor->append( fn );
	pbEncode->setTotalSteps( tf );
	connect( procEncode, SIGNAL(readyReadStdout()),
						this, SLOT(readEncode()));
	connect( procEncode, SIGNAL(readyReadStderr()),
						this, SLOT(readEncMpeg()));
	connect( procEncode, SIGNAL(processExited()),
						this, SLOT(accept()));
	exec();
}

void encodeWnd::demarrerA( )
{
	cbKeep->setChecked( true );
	cbKeep->show();
	pbClose->show();
	setCaption( "Authoring " );
	setModal( true );
	teAuthor->clear();
	connect( procEncode, SIGNAL(readyReadStderr()),
						this, SLOT(readAuthor()));
	connect( procEncode, SIGNAL(processExited()),
						this, SLOT(endAuthor()));
	exec();
}

void encodeWnd::endAuthor()
{
	if ( procEncode->normalExit() )
	{
		teAuthor->append( "Authoring successful\n" );
	}
	else
	{
		QString s;
		s.sprintf( "authoring failed with code : %d\n",
					procEncode->exitStatus() );
		teAuthor->append( s );
	}
	if ( !cbKeep->isChecked() ) accept();
}

void encodeWnd::cancelJob()
{
	procEncode->tryTerminate();
}
