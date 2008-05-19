#ifndef _PREFSWND_H	// {
#define _PREFSWND_H

#include "ui_prefswnd.h"
#include <QtNetwork/QHttp>
#include <QFile>
#include <QDialog>

class prefswnd : public QDialog, public Ui::prefsWnd
{
Q_OBJECT

public:
	prefswnd( QWidget *parent = 0);
	void readPrefs();
public slots:
	void saveExit();
	void toggleProxy();
};
#endif	// _PREFSWND_H }
