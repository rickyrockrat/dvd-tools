/****************************************************************************
** h extension file, included from the uic-generated form implementation.
**
** If you want to add, delete, or rename functions or slots, use
** Qt Designer to update this file, preserving your code.
**
** You should not define a constructor or destructor in this file.
** Instead, write your code in functions called init() and destroy().
** These will automatically be called by the form's constructor and
** destructor.
*****************************************************************************/
#include <qfontdialog.h>
#include <qcolordialog.h>

#include "qt4genpngwnd.h"

// constructor
qt4genpngwnd::qt4genpngwnd( QWidget *parent ) : QDialog( parent )
{
	setupUi( this );
	connect( pbNormalFont, SIGNAL(clicked()), this, SLOT(selNormalFont()));
	connect( pbItalicFont, SIGNAL(clicked()), this, SLOT(selItalicFont()));
	connect( pbBoldFont, SIGNAL(clicked()), this, SLOT(selBoldFont()));
	connect( pbSubColor, SIGNAL(clicked()), this, SLOT(selSubColor()));
	connect( pbOk, SIGNAL(clicked()), this, SLOT(accept()));
	connect( pbCancel, SIGNAL(clicked()), this, SLOT(reject()));
	QPalette pal;
	pal.setColor( QPalette::Button, Qt::white );
	pbSubColor->setPalette( pal );
}

void qt4genpngwnd::selNormalFont()
{
	pbNormalFont->setFont( QFontDialog::getFont( 0, pbNormalFont->font()) );
}

void qt4genpngwnd::selItalicFont()
{
	pbItalicFont->setFont( QFontDialog::getFont( 0, pbItalicFont->font()) );
}

void qt4genpngwnd::selBoldFont()
{
	pbBoldFont->setFont( QFontDialog::getFont( 0, pbBoldFont->font()) );
}

void qt4genpngwnd::selSubColor()
{
	QPalette pal;
	pal.setColor( QPalette::Button, QColorDialog::getColor() );
	pbSubColor->setPalette( pal );
}
