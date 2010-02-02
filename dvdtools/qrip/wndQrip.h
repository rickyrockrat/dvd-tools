#ifndef _WNDQRIP_H	// {
#define _WNDQRIP_H

#include <QProcess>
#include <cdio++/cdio.hpp>

#include "ui_wndQrip.h"

class wndQrip : public QMainWindow, public Ui::wndQrip
{
Q_OBJECT

public:
	wndQrip( QWidget *parent = 0 );
public slots :
	void readInfo();
	void multipleChecked();
	void playlistChecked();
	void extract();
	void readEncodage();
	void encodageFini(int, QProcess::ExitStatus);
private :
	void get_cddb_info(CdIo_t *, track_t, track_t );
	QProcess *proc;
};

#endif	//} _WNDQRIP_H
// vim: nows nosm ts=4 sw=4
