

#include "detailwnd.h"
#include <QProcess>
#include <QFileDialog>
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
	arg << "-cache";
	arg << "4096";
	arg << leAdresse->text();
	QProcess::execute( "mplayer", arg );
}

void detailwnd::enregistrer( )
{
	QString destFile = QFileDialog::getSaveFileName(this, tr("Sauver la video"),
                            "/home/dige7306/"+leAdresse->text(),
                            tr("Video (*.asf *.asx *.wsx *.wmv *.flv *.avi)"));
	if ( destFile.isEmpty() ) return;
	QStringList arg;
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
}

