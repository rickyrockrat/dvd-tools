

#include <QtNetwork/QHttp>
#include <QUrl>
#include <QMessageBox>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QProcess>
#include "recwnd.h"

#include <iostream>

recwnd::recwnd( QWidget *parent )
	: QDialog( parent )
{
	setupUi(this);

	req = new QHttp(this);
	imReq = new QHttp(this);

	connect( ckEnableProxy, SIGNAL(toggled(bool)),
		this, SLOT(toggledProxy(bool)) );
	connect ( pbGet, SIGNAL( clicked() ),
		this, SLOT( get() ) );
	connect( req, SIGNAL(done(bool)),
		this, SLOT(readResponse(bool)) );
	connect( imReq, SIGNAL( requestFinished( int, bool )),
		this, SLOT(imReqFinished(int, bool)) );
	connect( lwVideos, SIGNAL(itemDoubleClicked ( QListWidgetItem *)),
		this, SLOT(vidDoubleClicked ( QListWidgetItem * )) ); 
}

void recwnd::toggledProxy(bool b)
{
	gbProxy->setEnabled(b);
}

void recwnd::get()
{
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
			QString fn = sl[2].remove("'").remove(' ') ;
			QString vidname = sl[0].section('\'', 1 ).remove("'").remove(' ') ;
			QString link = sl[3].remove("'").remove(' ');
			if ( !fn.isEmpty() )
			{
				imageList << fn;
				nameList << vidname;
				linkList << link;
			}
		}
	}
	if ( imageList.size() > 0 )
	{
		ctr = 0;
		getOneImage();
	}
}

void recwnd::getOneImage()
{
	if ( ctr >= imageList.size() ) return;
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
	if ( error ) QMessageBox::information( this, "err", imReq->errorString() );
	new QListWidgetItem( QIcon( imFile->fileName() ), imName, lwVideos, ctr );
	imFile->close();
}

void recwnd::imReqDone(bool error)
{
	if ( error ) QMessageBox::information( this, "err", imReq->errorString() );
	ctr++;
	delete imFile;
	imFile = 0;
	delete imReq;
	getOneImage();
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
}

