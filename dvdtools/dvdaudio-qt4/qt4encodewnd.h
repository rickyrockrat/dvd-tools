#ifndef _QT4ENCODEWND_H	// {
#define _QT4ENCODEWND_H

#include "ui_qt4encodewnd.h"
#include "common.h"
#include <iostream>
#include <QProcess>

class qt4encodeWnd : public QDialog, public Ui::encodeWnd
{
Q_OBJECT

public:
	qt4encodeWnd( QWidget *parent = 0 );
	QProcess *procEncode;
public slots:
	void readAuthor();
	void readEncMpeg();
	void readEncode();
	void demarrer(QString fn, int tf );
	void demarrerA( );
	void endAuthor();
	void cancelJob();
};
#endif // } QT4ENCODEWND_H
