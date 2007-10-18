
#include <qapplication.h>
#include <qlistbox.h>
#include <qtextedit.h>
#include <qpainter.h>
#include <qdatetime.h>
#include <vector>
#include "subtitle.h"

#include "sublistboxitem.h"

SubListBoxItem::SubListBoxItem( Subtitle &s, QListBox * listbox ) :QListBoxItem(listbox)
{
	_subtitle = &s;
}

Subtitle &SubListBoxItem::getSubtitle()
{
	return *_subtitle;
}

void SubListBoxItem::paint(QPainter *p)
{
	QFontMetrics fm = p->fontMetrics();
	int begin =  fm.ascent();
	if ( _subtitle->getProblem() )
	{
		p->setPen( Qt::red );
	}
	p->drawText( 3, begin, _subtitle->getTiming() );
	
	std::vector<Subline> v = _subtitle->getSubs();
	std::vector<Subline>::const_iterator it;
	QFont f0 = p->font();
	for ( it = v.begin(); it != v.end(); it++ )
	{
		begin += fm.lineSpacing();
		if ( it->getFmt() == Subline::Italic )
		{
			QFont f = p->font();
			f.setItalic( true );
			p->setFont( f );

		}
		else if ( it->getFmt() == Subline::Bold )
		{
			QFont f = p->font();
			f.setBold( true );
			p->setFont( f );
		}
		p->drawText( 3, begin, it->getLine() );
		p->setFont( f0 );
	}
}

int SubListBoxItem::height ( const QListBox * lb ) const 
{
	int h = lb ? lb->fontMetrics().lineSpacing() + 2 : 0;
	int th = h + _subtitle->getSubs().size() * h;
	return QMAX( th, QApplication::globalStrut().height() );

}

int SubListBoxItem::width ( const QListBox * lb ) const 
{
	int w, w1;
	if ( lb )
	{
		w = lb->fontMetrics().width( _subtitle->getTiming() ) + 6;
		const std::vector<Subline> v = _subtitle->getSubs();
		std::vector<Subline>::const_iterator it;
		for ( it = v.begin(); it != v.end(); it++ )
		{
			w1 = lb->fontMetrics().width( it->getLine() ) + 6;
			if ( w1 > w ) w = w1;
		}
	}
	return QMAX( w, QApplication::globalStrut().width() );
}

