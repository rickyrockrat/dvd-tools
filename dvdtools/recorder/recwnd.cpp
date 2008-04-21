

#include <QtNetwork/QHttp>
#include <QUrl>
#include <QMessageBox>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QProcess>
#include <QDateTime>
#include <QTimer>
#include <QtDebug>
#include "recwnd.h"

#include <iostream>

recwnd::recwnd( QWidget *parent )
	: QDialog( parent )
{
	setupUi(this);

	req = new QHttp(this);
	imReq = new QHttp(this);
	dteStart->setDateTime( QDateTime::currentDateTime());

	connect( ckEnableProxy, SIGNAL(toggled(bool)),
		this, SLOT(toggledProxy(bool)) );
	connect ( pbGet, SIGNAL( clicked() ),
		this, SLOT( get() ) );
	connect ( pbSelDestFile, SIGNAL( clicked() ),
		this, SLOT( selectDestFile() ) );
	connect ( pbProgram, SIGNAL( clicked() ),
		this, SLOT( program() ) );
	connect ( pbCancelGet, SIGNAL( clicked() ),
		this, SLOT( cancelGet() ) );
	connect ( pbQuit, SIGNAL( clicked() ),
		this, SLOT( ok() ) );
	connect( req, SIGNAL(done(bool)),
		this, SLOT(readResponse(bool)) );
	connect( imReq, SIGNAL( requestFinished( int, bool )),
		this, SLOT(imReqFinished(int, bool)) );
	connect( lwVideos, SIGNAL(itemDoubleClicked ( QListWidgetItem *)),
		this, SLOT(vidDoubleClicked ( QListWidgetItem * )) ); 
	connect( leDestFile, SIGNAL(textChanged (QString)),
		this, SLOT(canProgram()) ); 
	connect( leKeyword, SIGNAL(textChanged (QString)),
		this, SLOT(canProgram()) ); 
	connect( dteStart, SIGNAL(dateTimeChanged ( const QDateTime & )),
		this, SLOT(canProgram()) );
	pbProgram->setEnabled( false );
	pbCancelGet->setEnabled( false );
	progbarGet->setVisible( false );
	lwProgs->setVisible( false );
	cancel = false;
	prog = false;
	retries = 0;
}

void recwnd::ok()
{
	clean();
	accept();
}

void recwnd::clean()
{
	std::vector<QFile *>::iterator it;
	for ( it = toRemove.begin(); it != toRemove.end(); it++ )
	{
		(*it)->remove();
	}
	toRemove.clear();
}

void recwnd::cancelGet()
{
	cancel = true;
	if ( imReq ) imReq->abort();
	progbarGet->setVisible( false );
	pbCancelGet->setEnabled(false);
}

void recwnd::selectDestFile()
{
	QString fn = QFileDialog::getSaveFileName( this, "Save to" );
	if ( !fn.isEmpty() )
		leDestFile->setText( fn );
}

void recwnd::canProgram()
{
	if ( ( !leDestFile->text().isEmpty() )
	&& ( !leKeyword->text().isEmpty() )
	&& ( dteStart->dateTime() >  QDateTime::currentDateTime() ) )
	{
		pbProgram->setEnabled( true );
	}
	else
	{
		pbProgram->setEnabled( false );
	}
}

void recwnd::toggledProxy(bool b)
{
	gbProxy->setEnabled(b);
}

void recwnd::record()
{
	prog = true;
	this->get();
}

void recwnd::recordStart()
{
	/*
	QStringList arg;
	arg << leKeyword->text();
	arg << leDestFile->text();
	arg << leUrl->text();
	QProcess::execute( "/home/david/dvdtools/recorder/prog.sh", arg );
	*/
	bool kwFound = false;
	for ( int i = 0; i < linkList.size(); i++ )
	{
		QString s = linkList[ i ];
		QString t = titleList[ i ];
		if ( t.contains( leKeyword->text() ) )
		{
			kwFound = true;
			QUrl link( s );
			progLinkReq = new QHttp( this );
			if ( ckEnableProxy->isChecked() )
			{
				progLinkReq->setProxy( leProxyHost->text(),
						sbProxyPort->value(),
						leProxyUser->text(),
						leProxyPassword->text() );
			}
			progLinkReq->setHost( link.host() );
			connect( progLinkReq, SIGNAL(done(bool)),
				this, SLOT(progLinkReadResponse(bool)));
			progLinkReq->get(link.path());
			break;
		}
	}
	if ( ( !kwFound ) && ( retries < 4 ) )
	{
		// retry two mminute later
		retries++;
		QTimer::singleShot(120000, this, SLOT(record()));
	}
}

void recwnd::progLinkReadResponse(bool err )
{
	if ( err )
	{
		return;
	}
	QList<QByteArray> l = progLinkReq->readAll().split('\n');
	QList<QByteArray>::Iterator it;
	QRegExp vidregexp( "var videoUrl=" );
	for ( it = l.begin(); it != l.end(); it++ )
	{
		QString s( *it );
		if ( s.contains( vidregexp ) )
		{
			//  var videoUrl='http://vodf.eurosport.com/2008/3/12/foot_demission_mancini_fr-67408-700-384-288.flv';
			QStringList sl = s.split('\'');
			QString link = sl[1];
			if ( !link.isEmpty() )
			{
				QStringList arg;
				arg << "-dumpstream";
				arg << "-dumpfile";
				arg << leDestFile->text();
				arg << link;
				QProcess::execute( "mplayer", arg );
				arg.clear();
				arg << "/usr/share/sounds/k3b_success1.wav";
				QProcess::execute( "mplayer", arg );
			}
		}
	}
}
void recwnd::program()
{
	QDateTime progdt = dteStart->dateTime();
	QDateTime curdt( QDateTime::currentDateTime() );
	if ( progdt > curdt )
	{
		uint delay = progdt.toTime_t() - curdt.toTime_t();
		QTimer::singleShot(delay * 1000, this, SLOT(record()));
		lwProgs->setVisible( true );
		QString s;
		s = leKeyword->text() + " at " + dteStart->dateTime().toString() + " to " + leDestFile->text() + " from " + leUrl->text();
		new QListWidgetItem( s, lwProgs );
		retries = 0;
	}
}

void recwnd::get()
{
	cancel = false;
	lwVideos->clear();
	clean();
	if ( ckEnableProxy->isChecked() )
	{
		req->setProxy( leProxyHost->text(),
				sbProxyPort->value(),
				leProxyUser->text(),
				leProxyPassword->text() );
	}
	QUrl url( leUrl->text() );
	req->setHost( url.host() );
	req->get( url.path() );
	pbCancelGet->setEnabled( true );
}

void recwnd::readResponse(bool err)
{
	if ( err )
	{
		QMessageBox::information( this, "status", req->errorString() );
		return;
	}
	QList<QByteArray> l = req->readAll().split('\n');
	QList<QByteArray>::Iterator it;
	QRegExp vidregexp( "VideoListUne.*new VideoListItem.*jpg.*video_vid[0-9]*.shtml");
	imageList.clear();
	nameList.clear();
	linkList.clear();
	ctr = 0;
	for ( it = l.begin(); it != l.end(); it++ )
	{
		QString s( *it );
		if ( s.contains( vidregexp ) )
		{
			// VideoListUne[1]=new VideoListItem('Mancini va partir', '', 'http://i.video.eurosport.fr/2008/03/12/424449-2776674-81-61.jpg', 'http://video.eurosport.fr/football/ligue-des-champions/2007-2008/video_vid67408.shtml', '2 257', 'Mer. 12/03/2008', '10:54','Football', '1', '0', '', '');
			QStringList sl = s.split(',');
			QStringList sl0 = sl[0].split('\'');
			QString titre = sl0[1];
			qDebug() << titre;
			QString fn = sl[2].remove("'").remove(' ') ;
			QString vidname = sl[0].section('\'', 1 ).remove("'").remove(' ') ;
			QString link = sl[3].remove("'").remove(' ');
			if ( !fn.isEmpty() )
			{
				titleList << titre;
				imageList << fn;
				nameList << vidname;
				linkList << link;
			}
		}
	}
	progbarGet->setVisible( true );
	progbarGet->setMaximum( imageList.size() );
	progbarGet->setValue( 0 );
	if ( imageList.size() > 0 )
	{
		ctr = 0;
		getOneImage();
	}
}

void recwnd::getOneImage()
{
	if ( ctr >= imageList.size() )
	{
		progbarGet->setVisible( false );
		pbCancelGet->setEnabled( false );
		if ( prog )
		{
			this->recordStart();
		}
		return;
	}
	QByteArray bau = imageList[ctr].toAscii();
	QUrl imUrl( imageList[ctr] );
	QFileInfo imInfo( imUrl.path() );
	qDebug( "Downloading %s with id=%d", imUrl.path().toAscii().constData(), imGetId );
	QString imFname( imInfo.fileName() );
	imFile = new QFile( imFname );
	imName = nameList[ ctr ];
	if (!imFile->open(QIODevice::WriteOnly)) {
		QMessageBox::information( this, "error",
		"error creating file" );
	}
	else
	{
		imReq = new QHttp( this );
		if ( ckEnableProxy->isChecked() )
		{
			imReq->setProxy( leProxyHost->text(),
					sbProxyPort->value(),
					leProxyUser->text(),
					leProxyPassword->text() );
		}
		imReq->setHost( imUrl.host() );
		connect( imReq,
			SIGNAL(responseHeaderReceived(const QHttpResponseHeader &)),
			this,
			SLOT(imReadResponseHeader(const QHttpResponseHeader&)));
		connect( imReq, SIGNAL(requestFinished(int,bool)),
			this, SLOT(imReqFinished(int,bool)));
		connect( imReq, SIGNAL(done(bool)),
			this, SLOT(imReqDone(bool)));
		imGetId = imReq->get( imUrl.path(), imFile );
	}
}

void recwnd::imReadResponseHeader(const QHttpResponseHeader &responseHeader)
{
	if (responseHeader.statusCode() != 200)
	{
         QMessageBox::information(this, tr("HTTP"),
                                  tr("Download failed: %1.")
                                  .arg(responseHeader.reasonPhrase()));
	}
}

void recwnd::imReqFinished(int curId, bool error )
{
	if ( curId != imGetId ) return;
	if ( error )
	{
		QMessageBox::information( this, "err", imReq->errorString() );
		return;
	}
	if ( !cancel )
	{
		QIcon *ic = new QIcon( imFile->fileName() );
		new QListWidgetItem( *ic, titleList[ctr], lwVideos, ctr );
		imFile->close();
		progbarGet->setValue( progbarGet->value() + 1 );
		toRemove.push_back(imFile);
		ctr++;
		imReq = 0;
		delete imReq;
		getOneImage();
	}
}

void recwnd::imReqDone(bool /*error*/)
{
	/*
	if ( error )
	{
		QMessageBox::information( this, "err", imReq->errorString() );
		delete imReq;
		imReq = 0;
		return;
	}
	*/
}

void recwnd::vidDoubleClicked ( QListWidgetItem * item ) 
{
	if ( item )
	{
		QUrl link( linkList[ item->type() ] );
		linkReq = new QHttp( this );
		if ( ckEnableProxy->isChecked() )
		{
			linkReq->setProxy( leProxyHost->text(),
					sbProxyPort->value(),
					leProxyUser->text(),
					leProxyPassword->text() );
		}
		linkReq->setHost( link.host() );
		connect( linkReq, SIGNAL(done(bool)),
			this, SLOT(linkReadResponse(bool)));
		linkReq->get(link.path());
	}
}

void recwnd::linkReadResponse(bool err)
{
	if ( err )
	{
		QMessageBox::information( this, "status", linkReq->errorString() );
		return;
	}
	QList<QByteArray> l = linkReq->readAll().split('\n');
	QList<QByteArray>::Iterator it;
	QRegExp vidregexp( "var videoUrl=" );
	for ( it = l.begin(); it != l.end(); it++ )
	{
		QString s( *it );
		if ( s.contains( vidregexp ) )
		{
			//  var videoUrl='http://vodf.eurosport.com/2008/3/12/foot_demission_mancini_fr-67408-700-384-288.flv';
			QStringList sl = s.split('\'');
			QString link = sl[1];
			if ( !link.isEmpty() )
			{
				link.replace( "mms", "http" );
				QMessageBox msgBox;
				msgBox.setWindowTitle( "Question" );
				msgBox.setText("Voulez-vous regarder cette video ou l'enregistrer ?" );
				QPushButton *pbRegarder = msgBox.addButton(tr("Regarder"), QMessageBox::ActionRole);
				QPushButton *pbEnregistrer = msgBox.addButton(tr("Enregistrer"), QMessageBox::ActionRole); 
				QPushButton *pbAnnuler = msgBox.addButton("Annuler", QMessageBox::ActionRole); 
				msgBox.exec();

				if (msgBox.clickedButton() == pbRegarder)
				{
					regarder( link );
 				}
				else if (msgBox.clickedButton() == pbEnregistrer)
				{
					enregistrer( link );
				}
				else if ( msgBox.clickedButton() == pbAnnuler )
				{
				}
			}
		}
	}
}

void recwnd::regarder( QString link )
{
	QStringList arg;
	arg << link;
	QProcess::execute( "mplayer", arg );
}

void recwnd::enregistrer( QString link )
{
	QString destFile = QFileDialog::getSaveFileName(this, tr("Sauver la video"),
                            "/home/dige7306/"+link,
                            tr("Video (*.asf *.asx *.wmv *.flv *.avi)"));
	if ( destFile.isEmpty() ) return;
	QStringList arg;
	arg << "-dumpstream";
	arg << "-dumpfile";
	arg << destFile;
	arg << link;
	QProcess::execute( "mplayer", arg );
	arg.clear();
	arg << "/usr/share/sounds/k3b_success1.wav";
	QProcess::execute( "mplayer", arg );
}

