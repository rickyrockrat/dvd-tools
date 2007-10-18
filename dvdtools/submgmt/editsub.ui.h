/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
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
#include <qobjectlist.h>
#include <qlineedit.h>
#include <qfont.h>

void EditSub::init()
{
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
	tabLines->setColumnStretchable( 0, true );
}


void EditSub::deleteLineClicked( )
{
	tabLines->removeRow(tabLines->currentRow());
}

void EditSub::insertLineClicked( )
{
	tabLines->insertRows(tabLines->numRows());
	QLineEdit *le = new QLineEdit( tabLines, "edit" );
	le->setFrame( false );
	tabLines->setCellWidget( tabLines->numRows() - 1, 0, le );
}

void EditSub::boldClicked( )
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

void EditSub::italicClicked( )
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

void EditSub::underlineClicked( )
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

void EditSub::strikeoutClicked( )
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

void EditSub::valid( )
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
	std::vector<Subline> v;
	for ( int i = 0; i < tabLines->numRows(); i++ )
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

void EditSub::setSub( SubListBoxItem * item )
{
	subtitle = &item->getSubtitle();
	setup();
}

void EditSub::setup( )
{
	if ( subtitle->getFrameBased() )
	{
		gbTime->hide();
		gbFrame->show();
		sbStart->setValue( subtitle->getFbegin() );
		sbStop->setValue( subtitle->getFend() );
		if ( subtitle->getProblem() )
		{
			sbStart->setPaletteBackgroundColor( Qt::red );
			sbStop->setPaletteBackgroundColor( Qt::red );
		}
	}
	else
	{
		gbTime->show();
		gbFrame->hide();
		teStart->setTime( subtitle->getBegin() );
		teStop->setTime( subtitle->getEnd() );
		if ( subtitle->getProblem() )
		{
			gbTime->setPaletteBackgroundColor( Qt::red );
		}
	}
	const std::vector<Subline> v = subtitle->getSubs();
	std::vector<Subline>::const_iterator it;
	for ( it = v.begin(); it != v.end(); it++ )
	{
		tabLines->insertRows( tabLines->numRows() );
		QLineEdit *le = new QLineEdit( tabLines, "edit" );
		le->setFrame( false );
		tabLines->setCellWidget( tabLines->numRows() - 1, 0, le );
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
}
