

#include "detailwnd.h"
#include <QProcess>
#include <QFileDialog>
#include <QtDebug>
#include <iostream>

detailwnd::detailwnd( QWidget *parent )
	: QDialog( parent )
{
	setupUi(this);

	connect ( pbRegarder, SIGNAL( clicked() ),
		this, SLOT( regarder() ) );
	connect ( pbEnregistrer, SIGNAL( clicked() ),
		this, SLOT( enregistrer() ) );
	connect ( pbRetour, SIGNAL( clicked() ),
		this, SLOT( accept() ) );
}

void detailwnd::setLink( QString link )
{
	leAdresse->setText( link );
}

void detailwnd::regarder( )
{
	QStringList arg;
	/* for eurosport
	arg << "-cache";
	arg << "4096";
	arg << leAdresse->text();
	QProcess::execute( "mplayer", arg );
	*/
	/* for atdhe */
	arg << leAdresse->text();
	QProcess::execute( "firefox", arg );
}

void detailwnd::enregistrer( )
{
	QString destFile = QFileDialog::getSaveFileName(this, tr("Sauver la video"),
                            "/home/dige7306/"+leAdresse->text(),
                            tr("Video (*.asf *.asx *.wsx *.wmv *.flv *.avi)"));
	if ( destFile.isEmpty() ) return;
	QStringList arg;
	/* for eurosport
	arg << "-cache";
	arg << "4096";
	arg << "-dumpstream";
	arg << "-dumpfile";
	arg << destFile;
	arg << leAdresse->text();
	QProcess::execute( "mplayer", arg );
	arg.clear();
	arg << "/usr/share/sounds/k3b_success1.wav";
	QProcess::execute( "mplayer", arg );
	*/
	/* for atdhe.net */
	arg << leAdresse->text();
	QProcess::execute( "firefox", arg );
	sleep( 5 );	/* wait for plugin to start */
	QStringList f;
	f << "Flash*";
	QStringList files = QDir::temp().entryList( f );
	qDebug() << files[0];
	arg.clear();
	arg << "-n" << "100000" << "--follow=name" << QDir::tempPath() + "/" + files[0];
	QProcess *tail = new QProcess( this );
	tail->setStandardOutputFile( destFile, QIODevice::Truncate );
	tail->setStandardErrorFile( "/tmp/tail.err", QIODevice::Truncate );
	tail->start( "tail", arg );
	if ( tail->waitForStarted() )
	{
		int msec = teDuree->time().hour() * 60 * 60 + teDuree->time().minute() * 60;
		msec *= 1000;
		if ( msec == 0 ) msec = -1;
		if ( !tail->waitForFinished(msec) )
		{
			tail->terminate();
		}
		qDebug() << " tail : " << tail->exitStatus() << " " << tail->exitCode() << endl;
	}
	else
	{
		qDebug() << " tail non demarre" << endl;
	}
}

