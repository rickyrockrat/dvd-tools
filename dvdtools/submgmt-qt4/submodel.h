
#ifndef _SUBMODEL_H
#define _SUBMODEL_H

#include "subtitle.h"
#include <QAbstractListModel>

class SubModel : public QAbstractListModel
{
Q_OBJECT

public:
SubModel(const std::vector<Subtitle *>, QObject *parent = 0);

int		rowCount	(const QModelIndex &parent = QModelIndex()
				) const;
QVariant	data		(const QModelIndex &index, int role
				) const;
Qt::ItemFlags	flags		(const QModelIndex &index
				) const;
bool		setData		(const QModelIndex &index,
				const QVariant &value,
				int role = Qt::EditRole
				);


private:
std::vector<Subtitle *> _subs;
};

#endif
