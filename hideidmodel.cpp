#include "hideidmodel.h"

#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>
#include <QSqlRecord>

using namespace Support;

HideIdModel::HideIdModel(QObject *parent):
    QAbstractTableModel (parent),
    checkable(false)
{

}

HideIdModel::~HideIdModel()
{

}

void HideIdModel::changeCheckable(bool value)
{
    this->checkable = value;
}

void HideIdModel::setQuery(int valueCount, QString sqlStatement, int indexOfId)
{
    QSqlQuery q;
    if(!q.exec(sqlStatement)){
        qDebug() << q.lastError() << endl;
        qDebug() << sqlStatement ;
        return;
    }
    this->setQuery(valueCount, q, indexOfId);
}

void HideIdModel::setQuery(int valueCount, QSqlQuery &q, int indexOfId)
{

    this->beginResetModel();
    auto rec = q.record();
    this->headers.clear();
    for(int i=0; i<valueCount ; ++i){
        if(i == indexOfId)
            continue;
        this->headers << rec.fieldName(i);
    }

    QPair<QVariant, bool> rowPrefix;
    QList<QVariant> valus;

    this->contents.clear();
    while (q.next()) {

        valus.clear();
        auto id = q.value(indexOfId);
        rowPrefix = QPair<QVariant, bool>(id, this->selectedIDs.contains(id));

        for(int i=0; i<valueCount ; ++i){
            if(i == indexOfId)
                continue;
            valus.append(q.value(i));
        }
        this->contents.append(QPair<QPair<QVariant, bool>,QList<QVariant>>(rowPrefix,valus));
    }
    this->endResetModel();
}

void HideIdModel::clear()
{
    this->beginResetModel();
    this->contents.clear();

    this->endResetModel();
}

void HideIdModel::setHorizontalHeader(int index, QString header)
{
    if(index >= this->headers.size())
        return;
    this->beginResetModel();
    this->headers.replace(index, header);
    this->endResetModel();
}

void HideIdModel::setSelectedDetermine(QString sqlStatement)
{
    this->beginResetModel();

    QSqlQuery q;
    if(!q.exec(sqlStatement)){
        qDebug() << q.lastError();
        return;
    }
    this->selectedIDs.clear();
    while (q.next()) {
        this->selectedIDs << q.value(0);
    }
    for(int i=0; i<this->contents.size(); ++i){
        auto row = this->contents.at(i);
        auto id = row.first.first;
        QPair<QVariant, bool> val;
        if(this->selectedIDs.contains(id)){
            val = QPair<QVariant, bool>(row.first.first,true);
        }else{
            val = QPair<QVariant, bool>(row.first.first,false);
        }
        this->contents.replace(i, QPair<QPair<QVariant,bool>,QList<QVariant>>(val,row.second));
    }

    this->endResetModel();
}

QList<QVariant> HideIdModel::selectedRecordIDs()
{
    return this->selectedIDs;
}

QVariant HideIdModel::oppositeID(QModelIndex &index)
{
    if(!index.isValid())
        return QVariant();
    if(index.row() < this->contents.size()){
        return this->contents.at(index.row()).first.first;
    }
    return QVariant();
}

int HideIdModel::rowCount(const QModelIndex &) const
{
    return this->contents.length();
}

int HideIdModel::columnCount(const QModelIndex &) const
{
    if(this->contents.size()==0)
        return 0;
    return this->contents.at(0).second.length();
}

Qt::ItemFlags HideIdModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags ret = Qt::ItemIsEnabled;

    if(!index.isValid())
        return ret;

    if(this->checkable && index.column() == 0)
        ret |= Qt::ItemIsUserCheckable;

    return ret | QAbstractItemModel::flags(index);
}

QVariant HideIdModel::data(const QModelIndex &index, int role) const
{
    if((!index.isValid()) || (index.row() > this->contents.size()))
        return QVariant();

    if(this->checkable && role == Qt::CheckStateRole && index.column() == 0){
            auto check =  this->contents.at(index.row()).first.second;
            if(check)
                return Qt::Checked;
            else
                return Qt::Unchecked;
        }

    if(role != Qt::DisplayRole)
        return QVariant();

    return this->contents.at(index.row()).second.at(index.column());
}

QVariant HideIdModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role != Qt::DisplayRole)
        return QVariant();

    if(orientation == Qt::Horizontal)
        if(section < this->headers.size())
            return this->headers.at(section);
        else
            return QVariant();
    else
        return QString("%1").arg(section);
}

bool HideIdModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if((!index.isValid()) || (index.row() > this->contents.size()))
        return false;
    if(this->checkable && role == Qt::CheckStateRole && index.column() == 0){
        auto row = this->contents.at(index.row());
        auto val(QPair<QVariant, bool>(row.first.first,value.toBool()));
        this->contents.replace(index.row(), QPair<QPair<QVariant,bool>,QList<QVariant>>(val,row.second));
        emit this->dataChanged(index, index);
        if(value.toBool()){
            emit this->signal_recordHasBeenSelected(row.first.first.toLongLong());
            this->selectedIDs.append(row.first.first);
        }else{
            emit this->signal_recordHasBeenDeselected(row.first.first.toLongLong());
            this->selectedIDs.removeOne(row.first.first);
        }
    }
    return true;
}
























