

#include <QtNetwork/QHttp>
#include <QUrl>
#include <QMessageBox>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QProcess>
#include <QDateTime>
#include <QDateTimeEdit>
#include <QTimer>
#include <QSettings>
#include <unistd.h>
#include <QtDebug>
#include "detailwnd.h"
#include "prefswnd.h"
#include "recwnd.h"

#include <iostream>

recwnd::recwnd( QWidget *parent )
	: QMainWindow( parent )
{
	setupUi(this);

	req = new QHttp(this);
	imReq = new QHttp(this);

	connect ( pbGet, SIGNAL( clicked() ),
		this, SLOT( get() ) );
	connect ( pbAddProgram, SIGNAL( clicked() ),
		this, SLOT( addProgram() ) );
	connect ( pbDelProgram, SIGNAL( clicked() ),
		this, SLOT( delProgram() ) );
	connect ( pbDestFile, SIGNAL( clicked() ),
		this, SLOT( selDestFile() ) );
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

	connect( aPrefs, SIGNAL(triggered()),
			this, SLOT(showPrefs()));
	connect( aExit, SIGNAL(triggered()),
			this, SLOT(ok()));
	pbCancelGet->setEnabled( false );
	progbarGet->setVisible( false );
	cancel = false;
	prog = false;
	retries = 0;
	numProgs = 0;
	progTimer = 0;
	readPrefs();

}

void recwnd::readPrefs()
{
	QSettings prefs( "dvdtools","recorder");
	proxyEnabled = prefs.value("proxy/enabled").toBool();
	if ( proxyEnabled )
	{
		proxyHost = prefs.value("proxy/host").toString();
		proxyPort = prefs.value("proxy/port").toInt();
		proxyUser = prefs.value("proxy/user").toString();
		proxyPassword = prefs.value("proxy/password").toString();
	}
	vidLinkRE = QRegExp( prefs.value("misc/vidlinkre").toString() );
	vidListRE = QRegExp( prefs.value("misc/vidlistre").toString() );
	defaultUrl = prefs.value("misc/defaulturl").toString();
	retries = prefs.value("misc/maxretries").toInt();
	minSize = prefs.value("misc/minsize").toInt();
}

void recwnd::ok()
{
	clean();
	close();
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

void recwnd::record()
{
	prog = true;
	this->get();
}

void recwnd::recordStart()
{
	bool kwFound = false;
	for ( int i = 0; i < linkList.size(); i++ )
	{
		QString s = linkList[ i ];
		QString t = titleList[ i ];
		QRegExp re( progKeyword );
		if ( ( t.contains( re ) )
		|| ( s.contains( re ) ) )
		{
			kwFound = true;
			QUrl link( s );
			progLinkReq = new QHttp( this );
			if ( proxyEnabled )
			{
				progLinkReq->setProxy( proxyHost,
						proxyPort,
						proxyUser,
						proxyPassword );
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
	for ( it = l.begin(); it != l.end(); it++ )
	{
		QString s( *it );
		if ( s.contains( vidLinkRE ) )
		{
			//  var videoUrl='http://vodf.eurosport.com/2008/3/12/foot_demission_mancini_fr-67408-700-384-288.flv';
			QStringList sl = s.split('\'');
			QString link = sl[1];
			if ( !link.isEmpty() )
			{
				QString prox;
				if ( proxyEnabled )
				{
					if ( proxyUser.isEmpty() )
					{
						prox = "http_proxy://" + proxyHost + ":"
									+ QString::number( proxyPort ) +
									"/http://";
					}
					else
					{
						prox = "http_proxy://" +
							proxyUser + ":" +
							proxyPassword + "@" +
							proxyHost + ":" +
							QString::number( proxyPort ) +
							"/http://";
					}
				}
				else
				{
					prox = "http://";
				}
				if ( link.contains("mms://") )
				{
					link.replace( "mms://", prox );
				}
				else
				{
					link.replace( "http://", prox );
				}
				int retries = 0;
				QStringList arg;
				while( retries < 4 )
				{
					arg.clear();
					arg << "-cache";
					arg << "4096";
					arg << "-dumpstream";
					arg << "-dumpfile";
					arg << progDestFile;
					arg << link;
					QProcess::execute( "mplayer", arg );
					if ( retries < 4 )
					{
						// keyword found, let's check if we dumped something useful
						QFile f( progDestFile );
						if ( f.size() < minSize )
						{
							sleep( 120 );
							retries++;
						}
						else
						{
							retries = 4;	// sortie
						}
					}
				}
				arg.clear();
				arg << "/usr/share/sounds/k3b_success1.wav";
				QProcess::execute( "mplayer", arg );
			}
		}
	}
}

void recwnd::delProgram()
{
	int cr = twProgs->currentRow();
	if ( cr >= 0 )
	{
		twProgs->removeRow(cr);
		numProgs--;
	}
}

void recwnd::addProgram()
{
	twProgs->insertRow( numProgs );
	QDateTime curdt( QDateTime::currentDateTime() );

	QTableWidgetItem *tu = new QTableWidgetItem( leUrl->text() );
	twProgs->setCellWidget(  numProgs, 1, new QDateTimeEdit( curdt ) );
	twProgs->setItem( numProgs, 3, tu );
	// (re)start the timer 
	if ( !progTimer )
	{
		progTimer = new QTimer(this);
     	connect( progTimer, SIGNAL(timeout()),
				this, SLOT(checkProg()));
	}
	if ( !progTimer->isActive() ) progTimer->start(60000);	// every minute

	numProgs++;
}

void recwnd::checkProg()
{
	QDateTime curdt( QDateTime::currentDateTime() );
	int nb = twProgs->rowCount();
	for ( int i = 0; i < nb; i++ )
	{
		QDateTimeEdit *te = (QDateTimeEdit *)twProgs->cellWidget( i, 1 );
		if ( curdt >= te->dateTime() )
		{
			// launch prog
			progKeyword = twProgs->item( i, 0 )->text();
			progDestFile = twProgs->item( i, 2 )->text();
			progUrl = twProgs->item( i, 3 )->text();
			twProgs->removeRow( i );
			if ( twProgs->rowCount() == 0 )
			{
				progTimer->stop();
			}
			this->record();
		}
	}
}

void recwnd::selDestFile()
{
	int cr = twProgs->currentRow();
	if ( cr >= 0 )
	{
		QString fn = QFileDialog::getSaveFileName();
		if ( !fn.isEmpty() )
		{
			QTableWidgetItem *tf = new QTableWidgetItem( fn );
			twProgs->setItem( cr, 2, tf );
		}
	}
}

void recwnd::get()
{
	cancel = false;
	lwVideos->clear();
	clean();
	if ( proxyEnabled )
	{
		req->setProxy( proxyHost,
				proxyPort,
				proxyUser,
				proxyPassword );
	}
	QUrl url;
	if ( prog )
	{
		url = QUrl( progUrl );
	}
	else
	{
		url = QUrl( leUrl->text() );
	}
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
	imageList.clear();
	nameList.clear();
	linkList.clear();
	ctr = 0;
	int nb = 0;
	for ( it = l.begin(); it != l.end(); it++ )
	{
		QString s( *it );
		if ( s.contains( vidListRE ) )
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
			if ( ++nb > sbNumber->value() ) break;
		}
	}
	progbarGet->setVisible( true );
	int max = 0;
	if ( sbNumber->value() != 0 )
	{
		if ( sbNumber->value() > imageList.size() )
		{
			max = imageList.size();
		}
		else
			max = sbNumber->value();
	}
	else
	{
		max = imageList.size();
	}
	progbarGet->setMaximum( max );
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
		if ( proxyEnabled )
		{
			imReq->setProxy( proxyHost,
					proxyPort,
					proxyUser,
					proxyPassword );
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
		if ( proxyEnabled )
		{
			linkReq->setProxy( proxyHost,
					proxyPort,
					proxyUser,
					proxyPassword );
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
	for ( it = l.begin(); it != l.end(); it++ )
	{
		QString s( *it );
		if ( s.contains( vidLinkRE ) )
		{
			//  var videoUrl='http://vodf.eurosport.com/2008/3/12/foot_demission_mancini_fr-67408-700-384-288.flv';
			QStringList sl = s.split('\'');
			QString link = sl[1];
			if ( !link.isEmpty() )
			{
				QString prox;
				if ( proxyEnabled )
				{
					if ( proxyUser.isEmpty() )
					{
						prox = "http_proxy://" + proxyHost + ":"
									+ QString::number( proxyPort ) +
									"/http://";
					}
					else
					{
						prox = "http_proxy://" +
							proxyUser + ":" +
							proxyPassword + "@" +
							proxyHost + ":" +
							QString::number( proxyPort ) +
							"/http://";
					}
				}
				else
				{
					prox = "http://";
				}
				if ( link.contains("mms://") )
				{
					link.replace( "mms://", prox );
				}
				else
				{
					link.replace( "http://", prox );
				}

				detailwnd *dw = new detailwnd(this);
				dw->setLink(link);
				dw->exec();
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
                            tr("Video (*.asf *.asx *wsx *.wmv *.flv *.avi)"));
	if ( destFile.isEmpty() ) return;
	QStringList arg;
	arg << "-cache";
	arg << "4096";
	arg << "-dumpstream";
	arg << "-dumpfile";
	arg << destFile;
	arg << link;
	QProcess::execute( "mplayer", arg );
	arg.clear();
	arg << "/usr/share/sounds/k3b_success1.wav";
	QProcess::execute( "mplayer", arg );
}

void recwnd::showPrefs()
{
	prefswnd *pw = new prefswnd( this );
	pw->exec();
}
