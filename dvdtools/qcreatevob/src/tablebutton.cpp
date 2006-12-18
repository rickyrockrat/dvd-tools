
#include "tablebutton.h"

TableButton::TableButton( QWidget *parent, const char *name )
												: QPushButton( parent, name )
{
	connect ( this, SIGNAL( clicked() ),
			this, SLOT(recClic() ) );
}

TableButton::TableButton( const QString &text,
							QWidget *parent,
							const char *name )
							: QPushButton( text, parent, name )
{
	connect ( this, SIGNAL( clicked() ),
			this, SLOT(recClic() ) );
}

TableButton::TableButton( int row, int col,
							QWidget *parent, const char *name )
							: QPushButton( parent, name )
{
	connect ( this, SIGNAL( clicked() ),
			this, SLOT(recClic() ) );
	_row = row;
	_col = col;
}

TableButton::TableButton( int row, int col, const QString &text,
				QWidget *parent, const char *name )
							: QPushButton( text, parent, name )
{
	connect ( this, SIGNAL( clicked() ),
			this, SLOT(recClic() ) );
	_row = row;
	_col = col;
}

TableButton::~TableButton()
{
}

void TableButton::setRow( int r )
{
	_row = r;
}

void TableButton::setCol( int c )
{
	_col = c;
}

int TableButton::row()
{
	return _row;
}

int TableButton::col()
{
	return _col;
}

void TableButton::recClic()
{
	emit clic( _row, _col );
}

