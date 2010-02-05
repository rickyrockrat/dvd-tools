#include "wndQrip.h"

#include <iostream>
#include <stdio.h>
#include <QCheckBox>
#include <QTime>
#include <QDebug>
#include <QDir>
#include <QTableWidgetItem>
#include <QTextCodec>

#include "qripcddb.h"

#define program_name	"qrip"

#define COL_SEL			0
#define COL_COMPOSER	1
#define COL_WORK		2
#define COL_TRACKNO		3
#define COL_TRACKTITLE	4
#define COL_LENGTH		5

#define STR_DEFDDATA	"/data/musique/%G/%C/%W/%N - %P.%E"
#define STR_DEFDPLAY	"/data/musique/%G/Albums/%C/%T - %A.%M"

wndQrip::wndQrip( QWidget *parent )
	: QMainWindow( parent )
{
	setupUi(this);

	connect( pbGo, SIGNAL(clicked()), this, SLOT(readInfo()) );
	connect( cbPlaylist, SIGNAL(stateChanged(int)),
		this, SLOT(playlistChecked()) );
	connect( pbExtract, SIGNAL(clicked()), this, SLOT(extract()) );
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
			twTracks->resizeColumnsToContents();
			connect( twTracks, SIGNAL(cellChanged(int,int)),
				this, SLOT(cellChanged(int,int)) );
		}
	}
}

void wndQrip::readInfo()
{
	disconnect( twTracks, SIGNAL(cellChanged(int,int)),
		this, SLOT(cellChanged(int,int)) );

	std::string s =  leDevice->text().toStdString();
	track_t i_first_track;
	track_t i_tracks;
	const char *psz_drive = NULL;

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

	get_cddb_info( p_cdio, i_tracks, i_first_track );
	connect( twTracks, SIGNAL(cellChanged(int,int)),
		this, SLOT(cellChanged(int,int)) );

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
	int first = 0;
	QStringList tracks;
	QTextStream stream( &fRip );
	QString destrep;
	QString destpl;
	QDir dpl;
	stream << "#!/bin/bash" << endl;
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
				stream << leAlbumTitle->text() << "' '" << destfn << "'" << endl;
				stream << " metaflac --set-tag 'AlbumArtist=";
				stream << leAlbumArtist->text() << "' '" << destfn << "'" << endl;
				stream << " metaflac --set-tag 'AlbumGenre=";
				stream << leAlbumGenre->text() << "' '" << destfn << "'" << endl;
				stream << " metaflac --set-tag 'Composer=";
				stream << twTracks->item( i, COL_COMPOSER )->text() << "' '" << destfn << "'" << endl;
				stream << " metaflac --set-tag 'Work=";
				stream << twTracks->item( i, COL_WORK )->text() << "' '" << destfn << "'" << endl;
				stream << " metaflac --set-tag 'TrackNo=";
				stream << twTracks->item( i, COL_TRACKNO )->text() << "' '" << destfn << "'" << endl;
				stream << " metaflac --set-tag 'TrackTitle=";
				stream << twTracks->item( i, COL_TRACKTITLE )->text() << "' '" << destfn << "'" << endl;
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
		else if ( p_cddb_disc )
		{
			printf("%s: Found %d matches in CDDB\n", program_name, i_cddb_matches);
			/* using the first match for simplicity
			if ( i_cddb_matches > 1 )
			{
				// TODO
				cddb_disc_print(p_cddb_disc);
				cddb_query_next(p_conn, p_cddb_disc);
				if (i != i_cddb_matches) cddb_read(p_conn, p_cddb_disc);
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
					pb = new QPushButton( "Work (%W)" );
					twTracks->setCellWidget( 0, COL_WORK, pb );
					pb = new QPushButton( "Track no (%N)" );
					twTracks->setCellWidget( 0, COL_TRACKNO, pb );
					pb = new QPushButton( "Track title (%P)" );
					twTracks->setCellWidget( 0, COL_TRACKTITLE, pb );
					pb = new QPushButton( "Lenght (%L)" );
					twTracks->setCellWidget( 0, COL_LENGTH, pb );
				}

				cddb_track_t *piste;
				piste = cddb_disc_get_track_first(p_cddb_disc);
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

					item = new QTableWidgetItem( cleanString(QString( title) ) );
					twTracks->setItem( i, COL_WORK, item );

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

					piste = cddb_disc_get_track_next(p_cddb_disc);
				}
				twTracks->resizeColumnsToContents();
			/*
			}
			*/
		}

		cddb_disc_destroy(p_cddb_disc);
		cddb_destroy(p_conn);
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
	return s;
}
