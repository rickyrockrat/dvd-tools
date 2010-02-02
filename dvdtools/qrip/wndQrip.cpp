#include "wndQrip.h"

#include <iostream>
#include <stdio.h>
#include <QCheckBox>
#include <QTime>
#include <QDebug>
#include <QDir>
#include <QTableWidgetItem>

#include "qripcddb.h"

#define program_name	"qrip"

#define COL_SEL			0
#define COL_COMPOSER	1
#define COL_WORK		2
#define COL_TRACKNO		3
#define COL_TRACKTITLE	4
#define COL_LENGTH		5

#define STR_DEFDDATA	"/data/musique/%G/%C/%W/%N - %P.%E"
#define STR_DEFDPLAY	"/data/musique/Albums/%C/%T - %A.%M"

wndQrip::wndQrip( QWidget *parent )
	: QMainWindow( parent )
{
	setupUi(this);

	connect( pbGo, SIGNAL(clicked()), this, SLOT(readInfo()) );
	connect( cbMultiple, SIGNAL(stateChanged(int)), this, SLOT(multipleChecked()) );
	connect( cbPlaylist, SIGNAL(stateChanged(int)), this, SLOT(playlistChecked()) );
	connect( pbExtract, SIGNAL(clicked()), this, SLOT(extract()) );

	leDestData->setText( STR_DEFDDATA );
	leDestPlaylist->setText( STR_DEFDPLAY );
	cbPlaylist->setChecked(true);
	lblProgress->setText("");
	proc = 0;
}

void wndQrip::readInfo()
{
	std::string s =  leDevice->text().toStdString();
	track_t i_first_track;
	track_t i_tracks;
	const char *psz_drive = NULL;

	//CdIo_t *p_cdio = cdio_open(s.c_str(), DRIVER_DEVICE);
	CdIo_t *p_cdio = cdio_open("/dev/cdrom", DRIVER_DEVICE);
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

	get_cddb_info( p_cdio, i_tracks, i_first_track );

}

void wndQrip::multipleChecked()
{
}

void wndQrip::playlistChecked()
{
	leDestPlaylist->setEnabled( cbPlaylist->isChecked() );
	cbFormatPlaylist->setEnabled( cbPlaylist->isChecked() );
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

	QCheckBox *cb;
	QTableWidgetItem *item;
	int ctr = 0;
	for ( i = 0; i < twTracks->rowCount(); i++ )
	{
		cb = (QCheckBox *)twTracks->cellWidget( i, COL_SEL );
		if ( cb->isChecked() )
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
	int first = 0;
	QStringList tracks;
	QTextStream stream( &fRip );
	QString destrep;
	QString destpl;
	QDir dpl;
	stream << "#!/bin/bash" << endl;
	int passage = 0;
	for ( i = 0; i < twTracks->rowCount(); i++ )
	{
		cb = (QCheckBox *)twTracks->cellWidget( i, COL_SEL );
		if ( cb->isChecked() )
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

			destfn.replace( QString("à"), QString( "a" ) );
			destfn.replace( QString("â"), QString( "a" ) );
			destfn.replace( QString("ä"), QString( "a" ) );
			destfn.replace( QString("é"), QString( "e" ) );
			destfn.replace( QString("è"), QString( "e" ) );
			destfn.replace( QString("ê"), QString( "e" ) );
			destfn.replace( QString("ë"), QString( "e" ) );
			destfn.replace( QString("ù"), QString( "u" ) );
			destfn.replace( QString("û"), QString( "u" ) );
			destfn.replace( QString("ü"), QString( "u" ) );
			destfn.replace( QString("î"), QString( "i" ) );
			destfn.replace( QString("ï"), QString( "i" ) );
			destfn.replace( QString("ô"), QString( "o" ) );
			destfn.replace( QString("ö"), QString( "o" ) );
			destfn.replace( QString("ç"), QString( "c" ) );
			destfn.replace( QString("°"), QString( "o" ) );
			
			QString rep = QString( destfn);
			destrep = rep.remove( rep.lastIndexOf( "/" ), rep.length() );
			QDir().mkpath( destrep );
			qDebug() << "r=" << destrep;
			qDebug() << "d=" << destfn;
			
			// commande à passer :
			// cdda2wav -D <dev> -t <track no> <dest> (sans le .wav)
			stream << "echo 'Extracting track " << QString::number(i+1);
			stream << "...'" << endl;
			stream << "cdda2wav -D " << leDevice->text() << " -t ";
			stream << QString::number(i+1) << " '" << destfn << "'" << endl;

			if ( cbFormat->currentText() == "flac" )
			{
				stream << "echo 'Encoding track " << QString::number(i+1);
				stream << "...'" << endl;
				stream << "flac -8 -o '" << destfn << "' '";
				stream << " metaflac --set-tag 'AlbumTitle=";
				stream << leAlbumTitle->text() << "'" << endl;
				stream << " metaflac --set-tag 'AlbumArtist=";
				stream << leAlbumArtist->text() << "'" << endl;
				stream << " metaflac --set-tag 'AlbumGenre=";
				stream << leAlbumGenre->text() << "'" << endl;
				stream << " metaflac --set-tag 'Composer=";
				stream << twTracks->item( i, COL_COMPOSER )->text() << "'" << endl;
				stream << " metaflac --set-tag 'Work=";
				stream << twTracks->item( i, COL_WORK )->text() << "'" << endl;
				stream << " metaflac --set-tag 'Work=";
				stream << twTracks->item( i, COL_WORK )->text() << "'" << endl;
				stream << " metaflac --set-tag 'TrackNo=";
				stream << twTracks->item( i, COL_TRACKNO )->text() << "'" << endl;
				stream << " metaflac --set-tag 'TrackTitle=";
				stream << twTracks->item( i, COL_TRACKTITLE )->text() << "'" << endl;
				QString wav = QString( destfn );
				QString inf = QString( destfn );
				wav.replace( ".flac", ".wav" );
				inf.replace( ".flac", ".inf" );
				stream << wav;
				stream << "'" << endl;
				// TODO : demander rm ?
				stream << "rm -f '" << wav << "' '";
				stream << inf << "'" << endl;
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

				destpl.replace( QString("à"), QString( "a" ) );
				destpl.replace( QString("â"), QString( "a" ) );
				destpl.replace( QString("ä"), QString( "a" ) );
				destpl.replace( QString("é"), QString( "e" ) );
				destpl.replace( QString("è"), QString( "e" ) );
				destpl.replace( QString("ê"), QString( "e" ) );
				destpl.replace( QString("ë"), QString( "e" ) );
				destpl.replace( QString("ù"), QString( "u" ) );
				destpl.replace( QString("û"), QString( "u" ) );
				destpl.replace( QString("ü"), QString( "u" ) );
				destpl.replace( QString("î"), QString( "i" ) );
				destpl.replace( QString("ï"), QString( "i" ) );
				destpl.replace( QString("ô"), QString( "o" ) );
				destpl.replace( QString("ö"), QString( "o" ) );
				destpl.replace( QString("ç"), QString( "c" ) );
				destpl.replace( QString("°"), QString( "o" ) );

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
		// TODO : ejecter
	}
	else
	{
		lblProgress->setText( "Erreur d'encodage" );
	}
}

void wndQrip::get_cddb_info(CdIo_t *p_cdio,
track_t i_tracks,
	track_t i_first_track)
{
	int i, i_cddb_matches = 0;

	cddb_conn_t *p_conn = NULL;
	cddb_disc_t *p_cddb_disc = NULL;

	if ( init_cddb(
			p_cdio, 
			&p_conn, 
			&p_cddb_disc, 
			cddb_errmsg,
			i_first_track,
			i_tracks,
			&i_cddb_matches
			) );
	{

		if (-1 == i_cddb_matches)
			printf("%s: %s\n", program_name, cddb_error_str(cddb_errno(p_conn)));
		else
		{
			printf("%s: Found %d matches in CDDB\n", program_name, i_cddb_matches);
			if ( i_cddb_matches > 1 )
			{
			// TODO
				cddb_disc_print(p_cddb_disc);
				cddb_query_next(p_conn, p_cddb_disc);
				if (i != i_cddb_matches) cddb_read(p_conn, p_cddb_disc);
			}
			else
			{
				unsigned int discid = cddb_disc_get_discid(p_cddb_disc);
				const char *catstr = cddb_disc_get_category_str(p_cddb_disc);
				unsigned int lenght = cddb_disc_get_length(p_cddb_disc);
				int trackcount = cddb_disc_get_track_count(p_cddb_disc);
				const char * title = cddb_disc_get_title(p_cddb_disc);
				leAlbumTitle->setText( title );
				const char * artist = cddb_disc_get_artist(p_cddb_disc);
				leAlbumArtist->setText( artist );
			
				//twTracks.clear();
				twTracks->setRowCount( trackcount );
				QCheckBox *cb;
				QTableWidgetItem *item;

				cddb_track_t *piste;
				piste = cddb_disc_get_track_first(p_cddb_disc);
				while ( piste != NULL )
				{
					int i = cddb_track_get_number(piste) - 1;
					int length = cddb_track_get_length(piste);	// en secondes
					const char *tracktitle = cddb_track_get_title(piste);
					const char *trackartist = cddb_track_get_artist(piste);
					const char *extdata = cddb_track_get_ext_data(piste);

					cb = new QCheckBox( twTracks );
					cb->setChecked( true );
					twTracks->setCellWidget( i, COL_SEL, cb );

					//item = new QTableWidgetItem( QString( artist ) );
					item = new QTableWidgetItem( QString( trackartist ) );
					twTracks->setItem( i, COL_COMPOSER, item );

					item = new QTableWidgetItem( QString( title ) );
					twTracks->setItem( i, COL_WORK, item );

					if ( i < 10 )
						item = new QTableWidgetItem( QString( "0%1" ).arg( i+1 ) );
					else
						item = new QTableWidgetItem( QString( "%1" ).arg( i+1 ) );
					twTracks->setItem( i, COL_TRACKNO, item );

					item = new QTableWidgetItem( QString( tracktitle ) );
					twTracks->setItem( i, COL_TRACKTITLE, item );

					QTime t = QTime( 0, 0, 0, 0);
					QTime t1 = t.addSecs( length );
					item=new QTableWidgetItem( t1.toString("mm:ss" ) );
					twTracks->setItem( i, COL_LENGTH, item );

					piste = cddb_disc_get_track_next(p_cddb_disc);
				}
			}
		}

		cddb_disc_destroy(p_cddb_disc);
		cddb_destroy(p_conn);
		libcddb_shutdown();
	
	}
}
