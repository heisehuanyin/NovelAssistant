#ifndef PROJECTMANAGER_H
#define PROJECTMANAGER_H

#include <QAbstractItemModel>
#include <QDomDocument>
#include <QString>



class ProjectManager:public QAbstractItemModel
{
    Q_OBJECT
public:
    ProjectManager(QString filePath);
    ~ProjectManager();


private:
    QDomDocument * doc;

    // QAbstractItemModel interface
public:
    virtual QModelIndex index(int row, int column, const QModelIndex &parent) const override;
    virtual QModelIndex parent(const QModelIndex &child) const override;
    virtual int rowCount(const QModelIndex &parent) const override;
    virtual int columnCount(const QModelIndex &parent) const override;
    virtual QVariant data(const QModelIndex &index, int role) const override;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    virtual bool insertRows(int row, int count, const QModelIndex &parent) override;
    virtual bool removeRows(int row, int count, const QModelIndex &parent) override;
    virtual bool moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild) override;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const override;
};

#endif // PROJECTMANAGER_H
