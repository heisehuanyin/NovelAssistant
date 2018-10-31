#include "projectmanager.h"

ProjectManager::ProjectManager(QString filePath)
{

}

ProjectManager::~ProjectManager()
{

}

QModelIndex ProjectManager::index(int row, int column, const QModelIndex &parent) const
{
    return parent.child(row, column);
}

QModelIndex ProjectManager::parent(const QModelIndex &child) const
{
    return child.parent();
}

int ProjectManager::rowCount(const QModelIndex &parent) const
{

}

int ProjectManager::columnCount(const QModelIndex &parent) const
{
    return 1;
}

QVariant ProjectManager::data(const QModelIndex &index, int role) const
{

}

bool ProjectManager::setData(const QModelIndex &index, const QVariant &value, int role)
{

}

bool ProjectManager::insertRows(int row, int count, const QModelIndex &parent)
{

}

bool ProjectManager::removeRows(int row, int count, const QModelIndex &parent)
{

}

bool ProjectManager::moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild)
{

}

Qt::ItemFlags ProjectManager::flags(const QModelIndex &index) const
{

}
