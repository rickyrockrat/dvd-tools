#include <iostream>
#include "subeditor.h"

SubEditor::SubEditor( const QModelIndex &index,
						const QStyleOptionViewItem &option,
						QWidget *parent )
						: QWidget( parent )
{
	QVariant var = index.model()->data(index, Qt::DisplayRole);
	sub = qVariantValue<Subtitle *>(var);
	frBegin = new QSpinBox( this );
	frEnd = new QSpinBox( this );
	teBegin = new QTimeEdit( this );
	teBegin->setDisplayFormat( "hh:mm:ss.zzz" );
	teEnd = new QTimeEdit( this );
	teEnd->setDisplayFormat( "hh:mm:ss.zzz" );
	init(option);
}

SubEditor::~SubEditor()
{
}

void SubEditor::setSub( Subtitle *s )
{
	sub = s;
}

Subtitle *SubEditor::getSub( )
{
	std::cout << "getSub!!" << std::endl;
	sub->setFbegin( frBegin->value() );
	sub->setFend( frEnd->value() );
	sub->setBegin( teBegin->time() );
	sub->setEnd( teEnd->time() );
	QVector<QLineEdit *>::Iterator it;
	QLineEdit *le;
	sub->getSubs().clear();
	for ( it = vlEdit.begin(); it != vlEdit.end(); it++ )
	{
		le = *it;
		QString st = le->text();
		if ( !st.isEmpty() )
		{
			Subline *s  = new Subline( st );
			sub->getSubs().push_back( *s );
		}
	}
	return sub;
}

void SubEditor::init( const QStyleOptionViewItem &option )
{
	QFontMetrics fm = option.fontMetrics;
	int l = option.rect.width();

	if ( sub->getFrameBased() )
	{
		frBegin->setValue( sub->getFbegin() );
		frEnd->setValue( sub->getFend() );

		frBegin->move( 0, 0 );
		frBegin->resize( frBegin->size().width(), fm.lineSpacing() );
		frBegin->show();
		frEnd->move( frBegin->size().width(), 0 );
		frEnd->resize( frEnd->size().width(), fm.lineSpacing() );
		frEnd->show();
		teBegin->hide();
		teEnd->hide();
	}
	else
	{
		teBegin->setTime( sub->getBegin() );

		teEnd->setTime( sub->getEnd() );

		teBegin->move( 0, 0 );
		teBegin->resize( teBegin->size().width(), fm.lineSpacing() );
		teBegin->show();
		teEnd->move( teBegin->size().width(), 0);
		teEnd->resize( teEnd->size().width(), fm.lineSpacing() );
		teEnd->show();

		frBegin->hide();
		frEnd->hide();
	}
	QVector<Subline>::Iterator it;
	QVector<Subline> v = sub->getSubs();
	QLineEdit *le;
	vlEdit.clear();
	int y = fm.lineSpacing();
	for ( it = v.begin(); it != v.end(); it++ )
	{
		le = new QLineEdit( this );
		le->resize( l, fm.lineSpacing() );
		le->move( 0, y );
		le->setText( it->getLine() );
		vlEdit.push_back( le );
		y += fm.lineSpacing();
	}
}

