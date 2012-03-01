#ifndef _WNDQRIP_H	// {
#define _WNDQRIP_H

#include <QProcess>
#include <QTextStream>
#include <cdio++/cdio.hpp>
#include <cddb/cddb.h>

#include "ui_wndQrip.h"

class wndQrip : public QMainWindow, public Ui::wndQrip
{
Q_OBJECT

public:
	wndQrip( QWidget *parent = 0 );
public slots :
	void readInfo();
	void cellChanged(int, int);
	void cellClicked(int, int);
	void playlistChecked();
	void liveChecked();
	void selAll();
	void extract();
	void eject();
	void listenTrack();
	void selWork();
	void selComposer();
	void selartist();
	void selgenre();
	void readEncodage();
	void encodageFini(int, QProcess::ExitStatus);
private :
	void liveTracks( QTextStream &);
	void normalTracks( QTextStream &);
	void get_cddb_conn(cddb_conn_t ** );
	void get_cddb_info(cddb_conn_t *, cddb_disc_t *, track_t, track_t );
	void defaultTracks( int );
	QString cleanString( QString );
	QProcess *proc;
};

#endif	//} _WNDQRIP_H
// vim: nows nosm ts=4 sw=4
