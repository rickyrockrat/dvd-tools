
#ifndef _TABLE_BUTTON_H	// {
#define _TABLE_BUTTON_H

#include <qpushbutton.h>

class TableButton : public QPushButton
{
	Q_OBJECT
	public:
		TableButton( QWidget *parent, const char *name = 0 );
		TableButton( const QString &text,QWidget *parent,const char *name = 0 );
		TableButton( int row, int col, QWidget *parent, const char *name = 0 );
		TableButton( int row, int col, const QString &text,
				QWidget *parent, const char *name = 0 );
		~TableButton();
		void setRow( int );
		void setCol( int );
		int row();
		int col();
	signals:
		void clic( int, int );
	public slots:
		void recClic();
	private:
		int _row;
		int _col;
};

#endif	// } _TABLE_BUTTON_H
