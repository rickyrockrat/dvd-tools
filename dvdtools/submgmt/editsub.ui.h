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

void EditSub::init()
{
	connect( pbOk, SIGNAL(clicked()),
				this, SLOT(accept()));
	connect( pbCancel, SIGNAL(clicked()),
				this, SLOT(reject()));
}

void EditSub::setSub( SubListBoxItem * item )
{
	Subtitle *sub = item->getSubtitle();
	if ( sub->getFrameBased() )
	{
		gbTime->hide();
		gbFrame->show();
		sbStart->setValue( sub->getFbegin() );
		sbStop->setValue( sub->getFend() );
	}
	else
	{
		gbTime->show();
		gbFrame->hide();
		teStart->setTime( sub->getBegin() );
		teStop->setTime( sub->getEnd() );
	}
	const std::vector<Subline> v = sub->getSubs();
	std::vector<Subline>::const_iterator it;
	for ( it = v.begin(); it != v.end(); it++ )
	{
		if ( it->getFmt() == Subline::Italic )
		{
			teSubs->append( "<i>" + it->getLine() + "</i>" );
		}
		else if ( it->getFmt() == Subline::Bold )
		{
			teSubs->append( "<b>" + it->getLine() + "</b>" );
		}
		else
		{
			teSubs->append( it->getLine() );
		}
	}
	

}
