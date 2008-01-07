
#ifndef _SUBITEMDELEGATE_H
#define _SUBITEMDELEGATE_H

#include "subtitle.h"
#include <QItemDelegate>
#include <QPainter>

#define QMAX(a, b)    ((b) < (a) ? (a) : (b))

class SubItemDelegate : public QItemDelegate
{
public :
	SubItemDelegate( QObject * );
	QWidget *createEditor(	QWidget *parent,
				const QStyleOptionViewItem &option,
				const QModelIndex &index
				) const;

	void setEditorData( QWidget *editor,
				const QModelIndex &index
				) const;
	void setModelData( QWidget *editor,
				QAbstractItemModel *model,
				const QModelIndex &index
				) const;
	void updateEditorGeometry( QWidget *editor,
				const QStyleOptionViewItem &option,
				const QModelIndex &index
				) const;
protected:
	void paint( QPainter * painter,
			const QStyleOptionViewItem & option,
			const QModelIndex & index 
			) const;
	QSize sizeHint ( const QStyleOptionViewItem & option,
				const QModelIndex & index 
				) const;
};

#endif /* _SUBITEMDELEGATE_H */
