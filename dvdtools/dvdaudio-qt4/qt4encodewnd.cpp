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
#include "qt4encodewnd.h"
#include <iostream>

qt4encodeWnd::qt4encodeWnd( QWidget *parent )
	: QDialog( parent )
{
	setupUi(this);
}

void qt4encodeWnd::readAuthor()
{
	if ( procEncode )
	{
		teAuthor->append( procEncode->readAll() );
	}
}

void qt4encodeWnd::readEncMpeg()
{
	if ( procEncode )
	{
		QByteArray in;
		const QString sp(" ");
		int frames;
		in = procEncode->readAll();
		QList<QByteArray> l1 = in.split( '\n' );
		QList<QByteArray>::Iterator it;
		for (it = l1.begin(); it !=l1.end(); it++ )
		{
			QList<QByteArray> pos = it->split(' ');
			pbEncode->setValue(pos[1].toInt() );
		}
	}
}

void qt4encodeWnd::readEncode()
{
	if ( procEncode )
	{
		QByteArray in;
		const QString sp(" ");
		int frames;
		in = procEncode->readAll();
		QList<QByteArray> l = in.split( '\n' );
		QList<QByteArray>::Iterator it;
		for (it = l.begin(); it != l.end(); it++ )
		{
			QList<QByteArray> pos = it->split( ':');
			QString s(pos[1]);
			pbEncode->setValue(s.section( "f", 0, 0 ).toInt() );
		}
	}
}
//INFO: [mpeg2enc] Frame 16399 16399 P q=112.00 sum act=477462.15818

void qt4encodeWnd::demarrer(QString fn, int tf )
{
	cbKeep->setChecked( false);
	cbKeep->hide();
	pbClose->hide();
	setWindowTitle( "Encoding to vob" );
	setModal( true );
	//FIXME teAuthor->setMaxLogLines(1000);
	teAuthor->clear();
	teAuthor->append( fn );
	pbEncode->setMaximum( tf );
	connect( procEncode, SIGNAL(readyReadStdout()),
						this, SLOT(readEncode()));
	connect( procEncode, SIGNAL(readyReadStderr()),
						this, SLOT(readEncMpeg()));
	connect( procEncode, SIGNAL(processExited()),
						this, SLOT(accept()));
	exec();
}

void qt4encodeWnd::demarrerA( )
{
	cbKeep->setChecked( true );
	cbKeep->show();
	pbClose->show();
	setWindowTitle( "Authoring " );
	setModal( true );
	teAuthor->clear();
	connect( procEncode, SIGNAL(readyReadStderr()),
						this, SLOT(readAuthor()));
	connect( procEncode, SIGNAL(processExited()),
						this, SLOT(endAuthor()));
	exec();
}

void qt4encodeWnd::endAuthor()
{
	if ( procEncode->exitStatus() == QProcess::NormalExit )
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

void qt4encodeWnd::cancelJob()
{
	procEncode->terminate();
}
