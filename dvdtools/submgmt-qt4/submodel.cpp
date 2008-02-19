
#include <vector>
#include "subtitle.h"
#include "submodel.h"

SubModel::SubModel( const std::vector<Subtitle *>subs, QObject *parent)
						: QAbstractListModel(parent)
{
	_subs = subs;
}

int SubModel::rowCount(const QModelIndex &parent) const
{
	return _subs.size();
}

QVariant SubModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (index.row() >= _subs.size())
		return QVariant();

	if (role == Qt::DisplayRole)
		return qVariantFromValue( _subs[index.row()] );
	else
		return QVariant();
}

Qt::ItemFlags SubModel::flags(const QModelIndex &index) const
{
	if (!index.isValid())
		return Qt::ItemIsEnabled;

	return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

bool SubModel::setData(const QModelIndex &index,
                               const QVariant &value, int role)
{
	if (index.isValid() && role == Qt::EditRole)
	{
		Subtitle *s = qVariantValue<Subtitle *>(value);
		_subs[ index.row() ] = s;
		emit dataChanged(index, index);
		return true;
	}
	return false;
}
