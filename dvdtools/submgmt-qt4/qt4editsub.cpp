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
//
#include <vector>
#include <iostream>
#include <QLineEdit>
#include <QFont>
#include <QTime>
#include <QListWidget>
#include <QHeaderView>
#include <QDialog>

#include "subtitle.h"

#include "qt4editsub.h"

qt4editsub::qt4editsub( QWidget *parent ) : QDialog( parent  )
{
	setupUi( this );
	connect( pbOk, SIGNAL(clicked()),
				this, SLOT(valid()));
	connect( pbCancel, SIGNAL(clicked()),
				this, SLOT(reject()));
	connect( pbBold, SIGNAL(clicked()),
				this, SLOT(boldClicked( )));
	connect( pbItalic, SIGNAL(clicked()),
				this, SLOT(italicClicked( )));
	connect( pbUnderline, SIGNAL(clicked()),
				this, SLOT(underlineClicked( )));
	connect( pbStrikeout, SIGNAL(clicked()),
				this, SLOT(strikeoutClicked( )));
	connect( pbInsertLine, SIGNAL(clicked()),
				this, SLOT(insertLineClicked( )));
	connect( pbDeleteLine, SIGNAL(clicked()),
				this, SLOT(deleteLineClicked( )));
	//tabLines->setColumnStretchable( 0, true );
	subtitle = 0;
	std::cout << "create editsub = " << this << std::endl;
}


void qt4editsub::deleteLineClicked( )
{
	tabLines->removeRow(tabLines->currentRow());
}

void qt4editsub::insertLineClicked( )
{
	tabLines->insertRow(tabLines->rowCount());
	QLineEdit *le = new QLineEdit( tabLines );
	le->setFrame( false );
	tabLines->setCellWidget( tabLines->rowCount() - 1, 0, le );
}

void qt4editsub::boldClicked( )
{
	QWidget *w = this->focusWidget();
	if ( w->inherits("QLineEdit" ) )
	{
		QLineEdit *e = (QLineEdit *)w;
		QFont f = e->font();
		f.setBold( !f.bold() );
		e->setFont(f);
	}
}

void qt4editsub::italicClicked( )
{
	QWidget *w = this->focusWidget();
	if ( w->inherits("QLineEdit" ) )
	{
		QLineEdit *e = (QLineEdit *)w;
		QFont f = e->font();
		f.setItalic( !f.italic() );
		e->setFont(f);
	}
}

void qt4editsub::underlineClicked( )
{
	QWidget *w = this->focusWidget();
	if ( w->inherits("QLineEdit" ) )
	{
		QLineEdit *e = (QLineEdit *)w;
		QFont f = e->font();
		f.setUnderline( !f.underline() );
		e->setFont(f);
	}
}

void qt4editsub::strikeoutClicked( )
{
	QWidget *w = this->focusWidget();
	if ( w->inherits("QLineEdit" ) )
	{
		QLineEdit *e = (QLineEdit *)w;
		QFont f = e->font();
		f.setStrikeOut( !f.strikeOut() );
		e->setFont(f);
	}
}

void qt4editsub::valid( )
{
	if ( subtitle->getFrameBased() )
	{
		subtitle->setFbegin( sbStart->value() );
		subtitle->setFend( sbStop->value() );
	}
	else
	{
		subtitle->setBegin( teStart->time() );
		subtitle->setEnd( teStop->time() );
	}
	QVector<Subline> v;
	for ( int i = 0; i < tabLines->rowCount(); i++ )
	{
		QLineEdit *le = (QLineEdit *)tabLines->cellWidget( i, 0 );
		QString *s = new QString( le->text() );
		Subline *l = new Subline( *s );
		QFont f = le->font();
		Subline::FMT fmt;
		if ( f.italic() )
		{
			fmt = Subline::Italic;
		}
		if ( f.bold() )
		{
			fmt = Subline::Bold;
		}
		if ( f.underline() )
		{
			fmt = Subline::Underline;
		}
		if ( f.strikeOut() )
		{
			fmt = Subline::Stroke;
		}
		l->setFmt( fmt );
		v.push_back( *l );
	}
	subtitle->setSubs( v );
	accept();
}

Subtitle &qt4editsub::getSub( )
{
	std::cout << "getsub ( " << this << " ) = " << subtitle << std::endl;
		return *subtitle;
}

void qt4editsub::setSub( Subtitle sub )
{
	std::cout << "set sub=" << &sub << std::endl;
	subtitle = &sub;
	setup();
}

void qt4editsub::setup( )
{
	if ( subtitle->getFrameBased() )
	{
		gbTime->hide();
		gbFrame->show();
		sbStart->setValue( subtitle->getFbegin() );
		sbStop->setValue( subtitle->getFend() );
		if ( subtitle->getProblem() )
		{
			QPalette pal;
			pal.setColor( QPalette::Background, Qt::red );
			sbStart->setPalette( pal );
			sbStop->setPalette( pal );
		}
	}
	else
	{
		gbTime->show();
		gbFrame->hide();
		teStart->setDisplayFormat( "HH:MM:SS.zzz" );
		teStart->setTime( subtitle->getBegin() );
		teStop->setTime( subtitle->getEnd() );
		if ( subtitle->getProblem() )
		{
			QPalette pal;
			pal.setColor( QPalette::Background, Qt::red );
			gbTime->setPalette( pal );
		}
	}
	const QVector<Subline> &v = subtitle->getSubs();
	QVector<Subline>::const_iterator it;
	tabLines->insertColumn( 0 );
	QStringList headers;
	headers << "Text";
	tabLines->setHorizontalHeaderLabels( headers );
	int ctr = 0;
	for ( it = v.begin(); it != v.end(); ctr++, it++ )
	{
		tabLines->insertRow( ctr );
		QLineEdit *le = new QLineEdit( tabLines );
		le->setFrame( false );
		tabLines->setCellWidget( ctr, 0, le );
		QFont f = le->font();
		if ( it->getFmt() == Subline::Italic )
		{
			f.setItalic(true);
		}
		if ( it->getFmt() == Subline::Bold )
		{
			f.setBold(true);
		}
		if ( it->getFmt() == Subline::Underline )
		{
			f.setUnderline(true);
		}
		if ( it->getFmt() == Subline::Stroke )
		{
			f.setStrikeOut(true);
		}
		le->setFont( f );
		le->setText( it->getLine() );
	}
	tabLines->horizontalHeader()->resizeSections( QHeaderView::Stretch );
}
