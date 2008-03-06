#ifndef _QT4PREVIEWWND_H	// {
#define _QT4PREVIEWWND_H

#include "ui_qt4previewwnd.h"
#include <QDialog>

class qt4previewWnd : public QDialog, public Ui::previewWnd
{
Q_OBJECT

public:
	qt4previewWnd( QWidget *parent = 0 ) : QDialog( parent )
	{
		setupUi( this );
	}

};

#endif	// _QT4PREVIEWWND_H }
