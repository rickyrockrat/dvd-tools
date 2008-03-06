
#include "qt4encodewnd.h"

void qt4encodeWnd::readAuthor()
{
	if ( procEncode )
	{
		//teAuthor->append( procEncode->readStderr() );
		teAuthor->append( procEncode->readAllStandardError() );
	}
}

void qt4encodeWnd::readEncode()
{
	if ( procEncode )
	{
		QString in;
		const QString sp(" ");
		int frames;
		//in = QString( procEncode->readStdout() );
		in = QString( procEncode->readAllStandardOutput() );
		//teEncode->append( in );
		QStringList l = in.split( "Pos:" );
		QStringList pos = l.join("").split( sp );
		pbEncode->setValue(pos[1].section( "f", 0, 0 ).toInt() );
	}
}

void qt4encodeWnd::demarrer(QString fn, int tf )
{
	cbKeep->setChecked( false);
	cbKeep->hide();
	pbClose->hide();
	setWindowTitle( "Encoding to vob" );
	setModal( true );
	teAuthor->clear();
	/*FIXME
	teAuthor->setTextFormat( Qt::LogText );
	teAuthor->setMaxLogLines(1000);
	*/
	teAuthor->append( fn );
	pbEncode->setMaximum( tf );
	connect( procEncode, SIGNAL(readyReadStdout()),
						this, SLOT(readEncode()));
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
