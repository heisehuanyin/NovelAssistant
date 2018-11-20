#include "hiddedidmodel.h"

#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>
#include <QSqlRecord>

using namespace Support;

HiddenIdModel::HiddenIdModel(bool checkable, QObject *parent):
    QAbstractTableModel (parent),
    checkable(checkable)
{

}

HiddenIdModel::~HiddenIdModel()
{

}

void HiddenIdModel::setQuery(int valueCount, QString sqlStatement, int indexOfId)
{
    this->beginResetModel();
    QSqlQuery q;
    if(!q.exec(sqlStatement)){
        qDebug() << q.lastError() << endl;
        qDebug() << sqlStatement ;
        return;
    }
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

void HiddenIdModel::setHorizontalHeader(int index, QString header)
{
    if(index >= this->headers.size())
        return;
    this->beginResetModel();
    this->headers.replace(index, header);
    this->endResetModel();
}

void HiddenIdModel::setColumnModified(int index, QString sqlStatement, QString holder4DisplayRole)
{
    this->modifySymbo.insert(index, QPair<QString,QString>(sqlStatement, holder4DisplayRole));
}

void HiddenIdModel::setSelectedDetermine(QString sqlStatement)
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


    this->endResetModel();
}

QList<QVariant> HiddenIdModel::selectedRecordIDs()
{
    QList<QVariant> rtn;
    QPair<QPair<QVariant,bool>,QList<QVariant>> row;
    foreach (row, this->contents) {
        auto prefix = row.first;
        if(prefix.second)
            rtn << prefix.first;
    }
    return rtn;
}

QVariant HiddenIdModel::oppositeID(QModelIndex &index)
{
    if(!index.isValid())
        return QVariant();
    if(index.row() < this->contents.size()){
        return this->contents.at(index.row()).first.first;
    }
    return QVariant();
}

int HiddenIdModel::rowCount(const QModelIndex &) const
{
    return this->contents.length();
}

int HiddenIdModel::columnCount(const QModelIndex &) const
{
    if(this->contents.size()==0)
        return 0;
    return this->contents.at(0).second.length();
}

Qt::ItemFlags HiddenIdModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags ret = Qt::ItemIsEnabled;

    if(!index.isValid())
        return ret;

    if(this->checkable && index.column() == 0)
        ret |= Qt::ItemIsUserCheckable;
    if(this->modifySymbo.contains(index.column()))
        ret |= Qt::ItemIsEditable;

    return ret | QAbstractItemModel::flags(index);
}

QVariant HiddenIdModel::data(const QModelIndex &index, int role) const
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

QVariant HiddenIdModel::headerData(int section, Qt::Orientation orientation, int role) const
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

bool HiddenIdModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if((!index.isValid()) || (index.row() > this->contents.size()))
        return false;
    if(this->checkable && role == Qt::CheckStateRole && index.column() == 0){
        auto row = this->contents.at(index.row());
        auto val(QPair<QVariant, bool>(row.first.first,value.toBool()));
        qSwap(row.first, val);
        emit this->dataChanged(index, index);
        if(value.toBool()){
            emit this->signal_recordHasBeenSelected(row.first.first.toLongLong());
        }else{
            emit this->signal_recordHasBeenDeselected(row.first.first.toLongLong());
        }
    }
    if(role == Qt::DisplayRole && this->modifySymbo.contains(index.column())){
        auto row = this->contents.at(index.row());
        auto newRow = row.second;
        newRow.replace(index.column(), value);
        qSwap(row.second, newRow);
        emit this->dataChanged(index,index);
        auto modifyStatement = this->modifySymbo.value(index.column());
        QSqlQuery q;
        q.prepare(modifyStatement.first);
        q.bindValue(modifyStatement.second, value);
        if(!q.exec()){
            qDebug() << q.lastError();
        }
    }
    return true;
}
























