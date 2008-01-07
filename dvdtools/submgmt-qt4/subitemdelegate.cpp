
#include <QVector>
#include <QApplication>
#include <iostream>
#include "subtitle.h"

#include "subitemdelegate.h"
#include "subeditor.h"

SubItemDelegate::SubItemDelegate( QObject * parent ) : QItemDelegate( parent )
{
	setClipping(false);
}

QWidget *SubItemDelegate::createEditor(	QWidget *parent,
			const QStyleOptionViewItem &option,
			const QModelIndex &index
			) const
{
	return new SubEditor( index, option, parent );
}

void SubItemDelegate::setEditorData( QWidget *editor,
			const QModelIndex &index
			) const
{
	SubEditor *edit = static_cast<SubEditor *>(editor);
	QVariant v = index.model()->data(index, Qt::DisplayRole);
	edit->setSub( v.value<Subtitle *>() );
}

void SubItemDelegate::setModelData( QWidget *editor,
			QAbstractItemModel *model,
			const QModelIndex &index
			) const
{
	SubEditor *edit = static_cast<SubEditor *>(editor);
	QVariant value = QVariant::fromValue( edit->getSub() );

	model->setData(index, value, Qt::EditRole);
}

void SubItemDelegate::updateEditorGeometry( QWidget *editor,
			const QStyleOptionViewItem &option,
			const QModelIndex &index
			) const
{
	editor->setGeometry(option.rect);
	return;
}
QSize SubItemDelegate::sizeHint( const QStyleOptionViewItem & option,
				const QModelIndex & index ) const  
{
	QVariant var = index.model()->data(index, Qt::DisplayRole);
	Subtitle *_subtitle = var.value<Subtitle *>();
	QFontMetrics fm( QApplication::font() );
	int h =	(_subtitle->getSubs().size() + 1) * fm.lineSpacing();
	int w = fm.width( _subtitle->getTiming());
	QVector<Subline> v = _subtitle->getSubs();
	QVector<Subline>::const_iterator it;
	for ( it = v.begin(); it != v.end(); it++ )
	{
		if ( fm.width( it->getLine()) > w )
			w = fm.width( it->getLine());
	}
	return QSize( w+3, h+3 );
}

void SubItemDelegate::paint( QPainter * p,
				const QStyleOptionViewItem & option,
				const QModelIndex & index ) const  
{
	p->save();
	if (option.state & QStyle::State_Selected)
	{
		p->fillRect(option.rect, option.palette.highlight());
		p->setPen( Qt::white );
	}
	QVariant var = index.model()->data(index, Qt::DisplayRole);
	Subtitle *_subtitle = var.value<Subtitle *>();
	QFontMetrics fm( QApplication::font() );
	p->setFont( QApplication::font() );
	int begin =  option.rect.y() + fm.lineSpacing();
	int xbeg = option.rect.x() + 3;
	if ( _subtitle->getProblem() )
	{
		p->setPen( Qt::red );
	}
	p->drawText( xbeg, begin, _subtitle->getTiming() );
	
	QVector<Subline> v = _subtitle->getSubs();
	QVector<Subline>::const_iterator it;
	for ( it = v.begin(); it != v.end(); it++ )
	{
		begin += fm.lineSpacing();
		QFont f( QApplication::font() );
		if ( it->getFmt() == Subline::Italic )
		{
			f.setItalic( true );

		}
		else if ( it->getFmt() == Subline::Bold )
		{
			f.setBold( true );
		}
		p->setFont( f );
		p->drawText( xbeg, begin, it->getLine() );
		p->setFont( QApplication::font() );
	}
	p->restore();
}
