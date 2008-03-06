#ifndef _QT4NEWTITLEWND_H	// {
#define _QT4NEWTITLEWND_H

#include <QDialog>
#include "ui_qt4newtitlewnd.h"

class qt4newtitleWnd : public QDialog, public Ui::newTitle
{
Q_OBJECT

public:
	qt4newtitleWnd( QWidget *parent = 0 );
	QString title;
	QString snapshot;
	QString aspect;
	QString audio1;
	QString audio2;
	QString sub1;
	QString sub2;
	QString sub3;
	QString sub4;
public slots:
	void acceptDialog();
	void openPic();
};

#endif	// _QT4NEWTITLEWND_H }
