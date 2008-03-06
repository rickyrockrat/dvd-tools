#ifndef _QT4ENCODEWND_H	//{
#define _QT4ENCODEWND_H

#include "ui_qt4encodewnd.h"
#include <QDialog>
#include <QProcess>

class qt4encodeWnd : public QDialog, public Ui::qt4encodeWnd
{
Q_OBJECT

public:
	qt4encodeWnd( QWidget *parent = 0 ) : QDialog( parent )
	{
		setupUi( this );
	}

	QProcess *procEncode;
public slots:
	void readAuthor();
	void readEncode();
	void demarrer(QString fn, int tf );
	void demarrerA( );
	void endAuthor();
	void cancelJob();
};
#endif	// _QT4ENCODEWND_H }
