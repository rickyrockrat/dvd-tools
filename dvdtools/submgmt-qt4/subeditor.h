
#ifndef _SUBEDITOR_H
#define _SUBEDITOR_H

#include <QWidget>
#include <QVector>
#include <QModelIndex>
#include <QLineEdit>
#include <QSpinBox>
#include <QTimeEdit>
#include <QPushButton>
#include <QStyleOptionViewItem>
#include "subtitle.h"

class SubEditor : public QWidget
{
public:
	SubEditor( const QModelIndex &index,
						const QStyleOptionViewItem &option,
						QWidget *parent = 0 );
	~SubEditor();
	void setSub( Subtitle * );
	Subtitle *getSub( );
	void init( const QStyleOptionViewItem &option );

private:
	QVector<QLineEdit *> vlEdit;
	QTimeEdit *teBegin;
	QTimeEdit *teEnd;
	QSpinBox *frBegin;
	QSpinBox *frEnd;
	Subtitle *sub;
};

#endif
