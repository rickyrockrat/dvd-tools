#ifndef _DETAILWND_H	// {
#define _DETAILWND_H

#include "ui_detailwnd.h"
#include <QtNetwork/QHttp>
#include <QFile>

class detailwnd : public QDialog, public Ui::detailWnd
{
Q_OBJECT

public:
	detailwnd( QWidget *parent = 0);
	void setLink( QString );
public slots:
	void regarder( );
	void enregistrer( );
};
#endif	// _DETAILWND_H }
