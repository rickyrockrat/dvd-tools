#include "wndQrip.h"

#include <iostream>
#include <stdio.h>
#include <QCheckBox>
#include <QTime>
#include <QDebug>
#include <QDir>
#include <QTableWidgetItem>
#include <QTextCodec>
#include <QFileDialog>

#include "qripcddb.h"
#include <cddb/cddb.h>

#define program_name	"qrip"

#define COL_SEL			0
#define COL_COMPOSER	1
#define COL_SELCOMPOSER	2
#define COL_WORK		3
#define COL_SELWORK		4
#define COL_TRACKNO		5
#define COL_TRACKTITLE	6
#define COL_LENGTH		7
#define COL_LISTEN		8

#define STR_DEFDDATA	"/data/musique/%G/%C/%W/%A/%N - %P.%E"
#define STR_DEFDPLAY	"/data/musique/%G/ Albums/%C/%T - %A.%M"

#define STR_DEFLDDATA	"/data/musique/%G/%C/%W/%A/%W.%E"
#define STR_DEFLDPLAY	"/data/musique/%G/ Albums/%C/%T - %A.%M"

wndQrip::wndQrip( QWidget *parent )
	: QMainWindow( parent )
{
	setupUi(this);

	connect( pbGo, SIGNAL(clicked()), this, SLOT(readInfo()) );
	connect( cbPlaylist, SIGNAL(stateChanged(int)),
		this, SLOT(playlistChecked()) );
	connect( cbLive, SIGNAL(stateChanged(int)),
		this, SLOT(liveChecked()) );
	connect( pbExtract, SIGNAL(clicked()), this, SLOT(extract()) );
	connect( tbEject, SIGNAL(clicked()), this, SLOT(eject()) );
	connect( tbSelArtist, SIGNAL(clicked()), this, SLOT(selartist()) );
	connect( tbSelGenre, SIGNAL(clicked()), this, SLOT(selgenre()) );
	connect( twTracks, SIGNAL(cellClicked(int,int)),
		this, SLOT(cellClicked(int,int)) );

	leDestData->setText( STR_DEFDDATA );
	leDestPlaylist->setText( STR_DEFDPLAY );
	cbPlaylist->setChecked(true);
	lblProgress->setText("");
	proc = 0;
	QTextCodec::setCodecForCStrings( QTextCodec::codecForName("UTF-8")) ;
}

void wndQrip::selAll( )
{
	qDebug() << "sel ALL" << endl;
	QCheckBox *cb = (QCheckBox *)twTracks->cellWidget( 0, COL_SEL );
	Qt::CheckState state;
	if ( cb->isChecked() )
	{
		state = Qt::Checked;
	}
	else
	{
		state = Qt::Unchecked;
	}
	QTableWidgetItem *item;
	for ( int i = 1; i < twTracks->rowCount(); i++ )
	{
		item = twTracks->item( i, COL_SEL );
		item->setCheckState( state );
	}
}

void wndQrip::cellClicked( int row, int col)
{
	//qDebug() << "row=" << row << " col=" << col << endl;
}

void wndQrip::cellChanged( int row, int col)
{
	if ( ( col == COL_COMPOSER ) || ( col == COL_WORK ) )
	{
		if ( !cbMultiple->isChecked() )
		{
			// on met a jour les autres lignes de la table
			disconnect( twTracks, SIGNAL(cellChanged(int,int)),
				this, SLOT(cellChanged(int,int)) );
			QString val = twTracks->item( row, col )->text();
			QTableWidgetItem *item;
			for ( int i = 0; i < twTracks->rowCount(); i++ )
			{
				if ( i != row )
				{
					item = new QTableWidgetItem( val );
					twTracks->setItem( i, col, item );
				}
			}
			connect( twTracks, SIGNAL(cellChanged(int,int)),
				this, SLOT(cellChanged(int,int)) );
		}
	}
	twTracks->resizeColumnsToContents();
}

void wndQrip::readInfo()
{
	disconnect( twTracks, SIGNAL(cellChanged(int,int)),
		this, SLOT(cellChanged(int,int)) );

	std::string s =  leDevice->text().toStdString();
	track_t i_first_track;
	track_t i_tracks;
	lba_t leadout;
	const char *psz_drive = NULL;
	cddb_disc_t *disc = NULL;

	CdIo_t *p_cdio = cdio_open(s.c_str(), DRIVER_DEVICE);
	if ( !p_cdio )
	{
		cdio_destroy(p_cdio);
		printf("Couldn't find CD\n");
		return;
	}
	else
	{
		i_first_track = cdio_get_first_track_num(p_cdio);
		i_tracks      = cdio_get_num_tracks(p_cdio);

		printf( "i_tracks=%i\n", i_tracks );
		printf( "i_first_track=%i\n", i_first_track );
	}

	disc = cddb_disc_new();
	if (disc == NULL)
	{
		fprintf(stderr, "out of memory, unable to create disc");
		return;
	}
	leadout = cdio_get_track_lba(p_cdio, CDIO_CDROM_LEADOUT_TRACK);
	cddb_disc_set_length(disc, leadout/CDIO_CD_FRAMES_PER_SEC );

	printf("  #:  LBA\n");
	int i = i_first_track;	
	cddb_track_t *track = NULL;
	for (int j = 0; j < i_tracks; i++, j++)
	{
		lba_t lba = cdio_get_track_lba(p_cdio, i);
		if (CDIO_INVALID_LBA != lba)
			printf("LBA: %3d: %06d\n", (int) i, lba);

		track = cddb_track_new();
		if (track == NULL)
		{
			fprintf(stderr, "out of memory, unable to create track");
			return;
		}
		cddb_track_set_frame_offset(track, lba);
		cddb_disc_add_track(disc, track);
	}

	cdio_destroy(p_cdio);
	cddb_conn_t * conn = NULL;
	get_cddb_conn( &conn );
	get_cddb_info( conn, disc, i_tracks, i_first_track );

	connect( twTracks, SIGNAL(cellChanged(int,int)),
		this, SLOT(cellChanged(int,int)) );

}

void wndQrip::liveChecked()
{
	if ( cbLive->isChecked() )
	{
		leDestData->setText( STR_DEFLDDATA );
	}
	else
	{
		leDestData->setText( STR_DEFDDATA );
	}
}

void wndQrip::playlistChecked()
{
	leDestPlaylist->setEnabled( cbPlaylist->isChecked() );
	cbFormatPlaylist->setEnabled( cbPlaylist->isChecked() );
}

void wndQrip::listenTrack()
{
	QProcess *p = new QProcess( this );
	int row = twTracks->currentRow();
	if ( row )
	{
		QString val = twTracks->item( row, COL_TRACKNO )->text();
		QStringList args;

		args << "-cdrom-device" << leDevice->text();
		args << "cdda://" + val;

		p->start("mplayer", args );
	}
}

void wndQrip::selWork()
{
	QString s = QFileDialog::getExistingDirectory(this, tr("Select Work"),
		QDir::currentPath() );
	if ( s != NULL )
	{
		int i = twTracks->currentRow();
		QTableWidgetItem *item = new QTableWidgetItem( s.section("/", -1) );
		twTracks->setItem( i, COL_WORK, item );
	}
}

void wndQrip::selComposer()
{
	QString s = QFileDialog::getExistingDirectory(this, tr("Select composer"),
		QDir::currentPath() );
	if ( s != NULL )
	{
		int i = twTracks->currentRow();
		QTableWidgetItem *item = new QTableWidgetItem( s.section("/", -1) );
		twTracks->setItem( i, COL_COMPOSER, item );
	}
}

void wndQrip::selartist()
{
	QString s = QFileDialog::getExistingDirectory(this, tr("Select Artist"),
		QDir::currentPath() );
	if ( s != NULL )
	{
		leAlbumArtist->setText( s.section( "/", -1 ) );
	}
}

void wndQrip::selgenre()
{
	QString s = QFileDialog::getExistingDirectory(this, tr("Select Genre"),
		QDir::currentPath() );
	if ( s != NULL )
	{
		leAlbumGenre->setText( s.section( "/", -1 ) );
	}
}

void wndQrip::eject()
{
	QProcess *p = new QProcess( this );
	QStringList args;
	args << leDevice->text();
	p->start("eject", args );
}

void wndQrip::extract()
{
	int i;

	QString stmpdir;

	char *tmpdir = (char *)getenv( "TMPDIR" );
	if ( tmpdir == NULL )
		stmpdir = QString( "/tmp" );
	else
		stmpdir = QString( tmpdir );

	QTableWidgetItem *item;
	int ctr = 0;
	for ( i = 1; i < twTracks->rowCount(); i++ )
	{
		item = twTracks->item( i, COL_SEL );
		if ( item->checkState() == Qt::Checked )
		{
			ctr++;
		}
	}
	if ( !ctr ) return;
	QString shName("/tmp/rip.sh");
	QFile fRip( shName );
	if ( !fRip.open( QIODevice::WriteOnly ) )
	{
		qDebug() << "erreur creation /tmp/rip.sh";
		// TODO message
		return;
	}
	QTextStream stream( &fRip );
	stream << "#!/bin/bash" << endl;
	if ( cbLive->isChecked() )
	{
		this->liveTracks( stream );
	}
	else
	{
		this->normalTracks( stream );
	}
	if ( cbEject->isChecked() )
	{
		stream << "eject " << leDevice->text() << endl;
	}
	fRip.close();
	proc = new QProcess( this );
	connect( proc,
		SIGNAL(readyReadStandardOutput()),
		this,
		SLOT(readEncodage()));
	connect( proc,
		SIGNAL(finished(int,QProcess::ExitStatus)),
		this,
		SLOT(encodageFini(int, QProcess::ExitStatus)));

	QStringList args;
	args << "/tmp/rip.sh";
	proc->start("sh", args );
}

void wndQrip::normalTracks( QTextStream &stream )
{
	QTableWidgetItem *item;
	QString destrep;
	QString destpl;
	QDir dpl;
	
	int i;
	int first = 0;
	int passage = 0;
	for ( i = 1; i < twTracks->rowCount(); i++ )
	{
		item = twTracks->item( i, COL_SEL );
		if ( item->checkState() == Qt::Checked )
		{
			first = i;
			QString destfn = leDestData->text();
			destfn.replace( QString("%T"), leAlbumTitle->text() );
			destfn.replace( QString("%A"), leAlbumArtist->text() );
			destfn.replace( QString("%G"), leAlbumGenre->text() );
			item = twTracks->item( i, COL_COMPOSER );
			destfn.replace( QString("%C"), item->text() );
			item = twTracks->item( i, COL_WORK );
			destfn.replace( QString("%W"), item->text() );
			item = twTracks->item( i, COL_TRACKNO );
			destfn.replace( QString("%N"), item->text() );
			item = twTracks->item( i, COL_TRACKTITLE );
			destfn.replace( QString("%P"), item->text() );
			item = twTracks->item( i, COL_LENGTH );
			destfn.replace( QString("%L"), item->text() );

			destfn.replace( QString("%E"), cbFormat->currentText() );

			cleanString( destfn );

			QString rep = QString( destfn);
			destrep = rep.remove( rep.lastIndexOf( "/" ), rep.length() );
			QDir().mkpath( destrep );
			qDebug() << "r=" << destrep;
			qDebug() << "d=" << destfn;
			
			// commande à passer :
			// cdda2wav -D <dev> -t <track no> <dest> (sans le .wav)
			stream << "echo 'Extracting track " << QString::number(i);
			stream << "...'" << endl;
			stream << "cdda2wav -D " << leDevice->text() << " -t ";
			stream << QString::number(i) << " '" << destfn << "'" << endl;

			if ( cbFormat->currentText() == "flac" )
			{
				stream << "echo 'Encoding track " << QString::number(i);
				stream << "...'" << endl;
				stream << "flac -8 ";
				if ( cbOverwrite->isChecked() ) 
					stream << "-f ";
				stream << "-o '" << destfn << "' '";
				QString wav = QString( destfn );
				QString inf = QString( destfn );
				wav.replace( ".flac", ".wav" );
				inf.replace( ".flac", ".inf" );
				stream << wav;
				stream << "'" << endl;
				stream << " metaflac --set-tag 'AlbumTitle=";
				QString albtit = leAlbumTitle->text();
				cleanString ( albtit );
				stream << albtit << "' '" << destfn << "'" << endl;
				stream << " metaflac --set-tag 'AlbumArtist=";
				QString albart = leAlbumArtist->text();
				cleanString ( albart );
				stream << albart << "' '" << destfn << "'" << endl;
				stream << " metaflac --set-tag 'AlbumGenre=";
				stream << leAlbumGenre->text() << "' '" << destfn << "'" << endl;
				stream << " metaflac --set-tag 'Composer=";
				QString compo = twTracks->item( i, COL_COMPOSER )->text();
				cleanString( compo );
				stream << compo << "' '" << destfn << "'" << endl;
				stream << " metaflac --set-tag 'Work=";
				QString work = twTracks->item( i, COL_WORK )->text();
				cleanString( work );
				stream << work << "' '" << destfn << "'" << endl;
				stream << " metaflac --set-tag 'TrackNo=";
				stream << twTracks->item( i, COL_TRACKNO )->text() << "' '" << destfn << "'" << endl;
				stream << " metaflac --set-tag 'TrackTitle=";
				QString tit = twTracks->item( i, COL_TRACKTITLE )->text();
				cleanString( tit );
				stream << tit << "' '" << destfn << "'" << endl;
				if ( cbRemove->isChecked() )
				{
					stream << "rm -f '" << wav << "' '";
					stream << inf << "'" << endl;
				}
			}
			if ( ( cbPlaylist->isChecked() ) && ( passage == 0 ) )
			{
				passage++;
				destpl = leDestPlaylist->text();
				destpl.replace( QString("%T"), leAlbumTitle->text() );
				destpl.replace( QString("%A"), leAlbumArtist->text() );
				destpl.replace( QString("%G"), leAlbumGenre->text() );
				// on prend les infos de la premire piste selectionnee
				item = twTracks->item( first, COL_COMPOSER );
				destpl.replace( QString("%C"), item->text() );
				item = twTracks->item( first, COL_WORK );
				destpl.replace( QString("%W"), item->text() );
				item = twTracks->item( first, COL_TRACKNO );
				destpl.replace( QString("%N"), item->text() );
				item = twTracks->item( first, COL_TRACKTITLE );
				destpl.replace( QString("%P"), item->text() );
				item = twTracks->item( first, COL_LENGTH );
				destpl.replace( QString("%L"), item->text() );

				destpl.replace( QString("%E"), cbFormat->currentText() );
				destpl.replace( QString("%M"), cbFormatPlaylist->currentText() );

				cleanString( destpl );

				QString reppl = QString( destpl);
				QString destreppl = reppl.remove( reppl.lastIndexOf( "/" ), reppl.length() );
				QDir().mkpath( destreppl );
				dpl= QDir( destreppl );
				qDebug() << "pl r=" << destreppl;
				qDebug() << "pl d=" << destpl;

			}
			stream << "echo '" << dpl.relativeFilePath( destfn );
			stream << "' >> '" << destpl << "'" << endl;
		}
	}
}

void wndQrip::liveTracks( QTextStream &stream)
{
	QTableWidgetItem *item;
	QString destrep;
	QString destpl;
	QDir dpl;
	
	int i, j;
	int first = 0;
	int passage = 0;
	for ( i = 1; i < twTracks->rowCount(); i++ )
	{
		item = twTracks->item( i, COL_SEL );
		if ( item->checkState() == Qt::Checked )
		{
			first = i;
			QString destfn = leDestData->text();
			destfn.replace( QString("%T"), leAlbumTitle->text() );
			destfn.replace( QString("%A"), leAlbumArtist->text() );
			destfn.replace( QString("%G"), leAlbumGenre->text() );
			item = twTracks->item( i, COL_COMPOSER );
			destfn.replace( QString("%C"), item->text() );
			item = twTracks->item( i, COL_WORK );
			destfn.replace( QString("%W"), item->text() );
			item = twTracks->item( i, COL_TRACKNO );
			destfn.replace( QString("%N"), item->text() );
			item = twTracks->item( i, COL_TRACKTITLE );
			destfn.replace( QString("%P"), item->text() );
			item = twTracks->item( i, COL_LENGTH );
			destfn.replace( QString("%L"), item->text() );

			destfn.replace( QString("%E"), cbFormat->currentText() );

			cleanString( destfn );

			QString rep = QString( destfn);
			destrep = rep.remove( rep.lastIndexOf( "/" ), rep.length() );
			QDir().mkpath( destrep );
			qDebug() << "r=" << destrep;
			qDebug() << "d=" << destfn;
			
			// commande à passer :
			// cdda2wav -D <dev> -t <track depart>+<track fin> <dest> (sans le .wav)
			// dans le cas live, <dest> correspond par défaut a %W.%E
			QString work = QString( twTracks->item( i, COL_WORK )->text() );
			// recherche de la piste de fin
			int lastTrack = i;
			QStringList seekList;
			for ( j = i+1; j < twTracks->rowCount(); j++ )
			{
				if ( twTracks->item( j, COL_WORK )->text() != work )
				{
					lastTrack = j - 1;
					break;
				}
				seekList << twTracks->item(j, COL_LENGTH )->text();
			}
			if ( j == twTracks->rowCount() )
			{
				lastTrack = twTracks->rowCount() - 1;
			}
			cleanString( work );
			stream << "echo 'Extracting live work " << work;
			stream << "...'" << endl;
			stream << "cdda2wav -D " << leDevice->text() << " -t ";
			stream << QString::number(i);
			if ( i != lastTrack )
			{
				stream << "+" << QString::number(lastTrack);
			}
			stream << " '" << destfn << "'" << endl;

			if ( cbFormat->currentText() == "flac" )
			{
				stream << "echo 'Encoding live work " << work;
				stream << "...'" << endl;
				stream << "flac -8 ";
				if ( cbOverwrite->isChecked() ) 
					stream << "-f ";
				stream << "-o '" << destfn << "' ";

				// ajout des seek points
				for ( j = 0; j < seekList.size(); j++ )
				{
					stream << " -S " << seekList[j];
				}
				stream << "'";
				QString wav = QString( destfn );
				QString inf = QString( destfn );
				wav.replace( ".flac", ".wav" );
				inf.replace( ".flac", ".inf" );
				stream << wav;
				stream << "'" << endl;
				stream << " metaflac --set-tag 'AlbumTitle=";
				QString albtit = leAlbumTitle->text();
				cleanString ( albtit );
				stream << albtit << "' '" << destfn << "'" << endl;
				stream << " metaflac --set-tag 'AlbumArtist=";
				QString albart = leAlbumArtist->text();
				cleanString ( albart );
				stream << albart << "' '" << destfn << "'" << endl;
				stream << " metaflac --set-tag 'AlbumGenre=";
				stream << leAlbumGenre->text() << "' '" << destfn << "'" << endl;
				stream << " metaflac --set-tag 'Composer=";
				QString compo = twTracks->item( i, COL_COMPOSER )->text();
				cleanString( compo );
				stream << compo << "' '" << destfn << "'" << endl;
				stream << " metaflac --set-tag 'Work=";
				stream << work << "' '" << destfn << "'" << endl;
				if ( cbRemove->isChecked() )
				{
					stream << "rm -f '" << wav << "' '";
					stream << inf << "'" << endl;
				}
			}
			if ( ( cbPlaylist->isChecked() ) && ( passage == 0 ) )
			{
				passage++;
				destpl = leDestPlaylist->text();
				destpl.replace( QString("%T"), leAlbumTitle->text() );
				destpl.replace( QString("%A"), leAlbumArtist->text() );
				destpl.replace( QString("%G"), leAlbumGenre->text() );
				// on prend les infos de la premire piste selectionnee
				item = twTracks->item( first, COL_COMPOSER );
				destpl.replace( QString("%C"), item->text() );
				item = twTracks->item( first, COL_WORK );
				destpl.replace( QString("%W"), item->text() );
				item = twTracks->item( first, COL_TRACKNO );
				destpl.replace( QString("%N"), item->text() );
				item = twTracks->item( first, COL_TRACKTITLE );
				destpl.replace( QString("%P"), item->text() );
				item = twTracks->item( first, COL_LENGTH );
				destpl.replace( QString("%L"), item->text() );

				destpl.replace( QString("%E"), cbFormat->currentText() );
				destpl.replace( QString("%M"), cbFormatPlaylist->currentText() );

				cleanString( destpl );

				QString reppl = QString( destpl);
				QString destreppl = reppl.remove( reppl.lastIndexOf( "/" ), reppl.length() );
				QDir().mkpath( destreppl );
				dpl= QDir( destreppl );
				qDebug() << "pl r=" << destreppl;
				qDebug() << "pl d=" << destpl;

			}
			stream << "echo '" << dpl.relativeFilePath( destfn );
			stream << "' >> '" << destpl << "'" << endl;
			i = lastTrack;
		}
	}
}

void wndQrip::readEncodage()
{
	if ( proc )
	{
		QString in = proc->readAllStandardOutput();
		lblProgress->setText( in );
	}
}

void wndQrip::encodageFini(int exitCode, QProcess::ExitStatus status )
{
	if ( exitCode == 0 )
	{
		lblProgress->setText( "Encodage fini" );
	}
	else
	{
		lblProgress->setText( "Erreur d'encodage" );
	}
}

void wndQrip::get_cddb_info( cddb_conn_t *conn,
	cddb_disc_t *p_cddb_disc,
	track_t i_tracks,
	track_t i_first_track)
{
	int i, matches = 0;
	matches = cddb_query(conn, p_cddb_disc);
	if (matches == -1)
	{
		/* something went wrong, print error */
		cddb_error_print(cddb_errno(conn));
		//exit(-1);
	}
	printf( "%d matches\n", matches );
	int success = cddb_read(conn, p_cddb_disc);
	if (!success)
	{
		/* something went wrong, print error */
		cddb_error_print(cddb_errno(conn));
		//exit(-1);
	}


	if (-1 == matches)
	{
		printf("%s: %s\n", program_name, cddb_error_str(cddb_errno(conn)));
		// Well let's try anyway...
	}
	else if ( p_cddb_disc )
	{
		printf("%s: Found %d matches in CDDB\n", program_name, matches);
		/* using the first match for simplicity
		if ( matches > 1 )
		{
			// TODO
			cddb_disc_print(p_cddb_disc);
			cddb_query_next(p_conn, p_cddb_disc);
			if (i != matches) cddb_read(p_conn, p_cddb_disc);
		}
		else
		{
		*/
			unsigned int discid = cddb_disc_get_discid(p_cddb_disc);
			const char *catstr = cddb_disc_get_category_str(p_cddb_disc);
			leAlbumGenre->setText( cleanString( QString(catstr).toLower() ) );
			unsigned int lenght = cddb_disc_get_length(p_cddb_disc);
			int trackcount = cddb_disc_get_track_count(p_cddb_disc);
			const char * title = cddb_disc_get_title(p_cddb_disc);
			leAlbumTitle->setText( cleanString(title) );
			const char * artist = cddb_disc_get_artist(p_cddb_disc);
			leAlbumArtist->setText( cleanString(artist) );
		
			QTableWidgetItem *item;
			if ( trackcount > 0 )
			{
				twTracks->clear();
				twTracks->setRowCount( trackcount + 1 );
				QCheckBox *cb = new QCheckBox( "All" );
				cb->setChecked(true);
				connect( cb, SIGNAL(stateChanged(int)),
						this, SLOT(selAll()) );
				twTracks->setCellWidget( 0, COL_SEL, cb );

				QPushButton *pb = new QPushButton( "Composer (%C)" );
				twTracks->setCellWidget( 0, COL_COMPOSER, pb );
				item = new QTableWidgetItem( "SelComp" );
				twTracks->setItem( 0, COL_SELCOMPOSER, item );
				pb = new QPushButton( "Work (%W)" );
				twTracks->setCellWidget( 0, COL_WORK, pb );
				item = new QTableWidgetItem( "SelWork" );
				twTracks->setItem( 0, COL_SELWORK, item );
				pb = new QPushButton( "Track no (%N)" );
				twTracks->setCellWidget( 0, COL_TRACKNO, pb );
				pb = new QPushButton( "Track title (%P)" );
				twTracks->setCellWidget( 0, COL_TRACKTITLE, pb );
				pb = new QPushButton( "Lenght (%L)" );
				twTracks->setCellWidget( 0, COL_LENGTH, pb );
				item = new QTableWidgetItem( "Listen" );
				twTracks->setItem( 0, COL_LISTEN, item );
			}

			cddb_track_t *piste;
			piste = cddb_disc_get_track_first(p_cddb_disc);
			QToolButton *tb = 0;
			while ( piste != NULL )
			{
				int i = cddb_track_get_number(piste);
				int length = cddb_track_get_length(piste);	// en secondes
				const char *tracktitle = cddb_track_get_title(piste);
				const char *trackartist = cddb_track_get_artist(piste);
				const char *extdata = cddb_track_get_ext_data(piste);

				item = new QTableWidgetItem( "" );
				item->setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsEnabled);
				item->setCheckState(Qt::Checked);
				twTracks->setItem( i, COL_SEL, item );

				item = new QTableWidgetItem( cleanString(QString( trackartist) ) );
				twTracks->setItem( i, COL_COMPOSER, item );

				tb = new QToolButton( twTracks );
				tb->setText("...");
				connect( tb, SIGNAL(clicked()),
						this, SLOT(selComposer()) );
				twTracks->setCellWidget( i, COL_SELCOMPOSER, tb );

				item = new QTableWidgetItem( cleanString(QString( title) ) );
				twTracks->setItem( i, COL_WORK, item );

				tb = new QToolButton( twTracks );
				tb->setText("...");
				connect( tb, SIGNAL(clicked()),
						this, SLOT(selWork()) );
				twTracks->setCellWidget( i, COL_SELWORK, tb );

				if ( i < 10 )
					item = new QTableWidgetItem( QString( "0%1" ).arg( i ) );
				else
					item = new QTableWidgetItem( QString( "%1" ).arg( i ) );
				twTracks->setItem( i, COL_TRACKNO, item );

				item = new QTableWidgetItem( cleanString(QString( tracktitle) ) );
				twTracks->setItem( i, COL_TRACKTITLE, item );

				QTime t = QTime( 0, 0, 0, 0);
				QTime t1 = t.addSecs( length );
				item=new QTableWidgetItem( t1.toString("mm:ss" ) );
				twTracks->setItem( i, COL_LENGTH, item );

				tb = new QToolButton( twTracks );
				tb->setText("|>");
				connect( tb, SIGNAL(clicked()),
						this, SLOT(listenTrack()) );
				twTracks->setCellWidget( i, COL_LISTEN, tb );

				piste = cddb_disc_get_track_next(p_cddb_disc);
			}
			twTracks->resizeColumnsToContents();
		/*
		}
		*/

		cddb_disc_destroy(p_cddb_disc);
		cddb_destroy(conn);
		libcddb_shutdown();
	}
}

QString wndQrip::cleanString( QString s )
{
	s.replace( QString("à"), QString( "a" ) );
	s.replace( QString("â"), QString( "a" ) );
	s.replace( QString("ä"), QString( "a" ) );
	s.replace( QString("é"), QString( "e" ) );
	s.replace( QString("è"), QString( "e" ) );
	s.replace( QString("ê"), QString( "e" ) );
	s.replace( QString("ë"), QString( "e" ) );
	s.replace( QString("ù"), QString( "u" ) );
	s.replace( QString("û"), QString( "u" ) );
	s.replace( QString("ü"), QString( "u" ) );
	s.replace( QString("î"), QString( "i" ) );
	s.replace( QString("ï"), QString( "i" ) );
	s.replace( QString("ô"), QString( "o" ) );
	s.replace( QString("ö"), QString( "o" ) );
	s.replace( QString("ç"), QString( "c" ) );
	s.replace( QString("°"), QString( "o" ) );
	
	s.replace( QString("'"), QString( "" ) );
	s.replace( QString(";"), QString( "" ) );
	s.replace( QString(":"), QString( "" ) );
	s.replace( QString("\""), QString( "" ) );
	s.replace( QString("?"), QString( "" ) );
	return s;
}


void wndQrip::defaultTracks( int trackcount )
{
	QTableWidgetItem *item;
	if ( trackcount > 0 )
	{
		twTracks->clear();
		twTracks->setRowCount( trackcount + 1 );
		QCheckBox *cb = new QCheckBox( "All" );
		cb->setChecked(true);
		connect( cb, SIGNAL(stateChanged(int)),
				this, SLOT(selAll()) );
		twTracks->setCellWidget( 0, COL_SEL, cb );

		QPushButton *pb = new QPushButton( "Composer (%C)" );
		twTracks->setCellWidget( 0, COL_COMPOSER, pb );
		item = new QTableWidgetItem( "SelComp" );
		twTracks->setItem( 0, COL_SELCOMPOSER, item );
		pb = new QPushButton( "Work (%W)" );
		twTracks->setCellWidget( 0, COL_WORK, pb );
		item = new QTableWidgetItem( "SelWork" );
		twTracks->setItem( 0, COL_SELWORK, item );
		pb = new QPushButton( "Track no (%N)" );
		twTracks->setCellWidget( 0, COL_TRACKNO, pb );
		pb = new QPushButton( "Track title (%P)" );
		twTracks->setCellWidget( 0, COL_TRACKTITLE, pb );
		pb = new QPushButton( "Lenght (%L)" );
		twTracks->setCellWidget( 0, COL_LENGTH, pb );
		item = new QTableWidgetItem( "Listen" );
		twTracks->setItem( 0, COL_LISTEN, item );
	}

	QToolButton *tb = 0;
	for ( int i = 1; i <= trackcount; i++ )
	{
		int length = 0.0;
		char *tracktitle;
		sprintf( tracktitle, "piste %d", i );
		char *trackartist = "unknown artist";
		char *extdata = "";

		item = new QTableWidgetItem( "" );
		item->setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsEnabled);
		item->setCheckState(Qt::Checked);
		twTracks->setItem( i, COL_SEL, item );

		item = new QTableWidgetItem( cleanString(QString( trackartist) ) );
		twTracks->setItem( i, COL_COMPOSER, item );

		tb = new QToolButton( twTracks );
		tb->setText("...");
		connect( tb, SIGNAL(clicked()),
				this, SLOT(selComposer()) );
		twTracks->setCellWidget( i, COL_SELCOMPOSER, tb );

		item = new QTableWidgetItem( cleanString(QString("unknown title") ) );
		twTracks->setItem( i, COL_WORK, item );

		tb = new QToolButton( twTracks );
		tb->setText("...");
		connect( tb, SIGNAL(clicked()),
				this, SLOT(selWork()) );
		twTracks->setCellWidget( i, COL_SELWORK, tb );

		if ( i < 10 )
			item = new QTableWidgetItem( QString( "0%1" ).arg( i ) );
		else
			item = new QTableWidgetItem( QString( "%1" ).arg( i ) );
		twTracks->setItem( i, COL_TRACKNO, item );

		item = new QTableWidgetItem( cleanString(QString( tracktitle) ) );
		twTracks->setItem( i, COL_TRACKTITLE, item );

		QTime t = QTime( 0, 0, 0, 0);
		QTime t1 = t.addSecs( length );
		item=new QTableWidgetItem( t1.toString("mm:ss" ) );
		twTracks->setItem( i, COL_LENGTH, item );

		tb = new QToolButton( twTracks );
		tb->setText("|>");
		connect( tb, SIGNAL(clicked()),
				this, SLOT(listenTrack()) );
		twTracks->setCellWidget( i, COL_LISTEN, tb );

	}
	twTracks->resizeColumnsToContents();
}

void wndQrip::get_cddb_conn( cddb_conn_t **cnx )
{
	cddb_conn_t * conn;
	conn = cddb_new();
	if (conn == NULL)
	{
		fprintf(stderr, "out of memory, unable to create connection structure");
	}
	/* HTTP proxy settings */
	cddb_http_proxy_enable(conn);                          /* REQ */
	cddb_set_http_proxy_server_name(conn, "p-niceway"); /* REQ */
	cddb_set_http_proxy_server_port(conn, 3128);           /* REQ */
	cddb_set_server_port(conn, 80);                        /* REQ */
	cddb_set_server_name(conn, "freedb.org");
	cddb_set_http_path_query(conn, "/~cddb/cddb.cgi");
	cddb_set_http_path_submit(conn, "/~cddb/submit.cgi");
	cddb_set_http_proxy_username(conn, "");
	cddb_set_http_proxy_password(conn, "");

	*cnx = conn;
}
