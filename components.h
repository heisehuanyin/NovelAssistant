#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <QComboBox>
#include <QDialog>
#include <QListView>
#include <QPushButton>
#include <QStandardItemModel>
#include <QTableView>


namespace UIComp {
    class GTME :public QDialog
    {
        Q_OBJECT
    public:
        explicit GTME(QWidget *parent=nullptr);
        virtual ~GTME() override;


    private:
        QComboBox *const groups;
        QComboBox *const types;
        QTableView *const items;
        QStandardItemModel *const itemsmodel;
        QPushButton *const up;
        QPushButton *const down;
        QPushButton *const append;
        QPushButton *const remove;
        QPushButton *const apply;
        QPushButton *const insertType;
        QList<QVariant> list;

    private slots:
        void slot_groupsSelected(const QString &text);
        void slot_typesSelected(const QString &text);
        void slot_insertNewType();
        void slot_itemUp();
        void slot_itemDown();
        void slot_itemAppend();
        void slot_itemRemove();
        void slot_itemApply();
        void slot_btnEnableRelyOnSelect(const QItemSelection &selected, const QItemSelection &);
        void slot_btnApplyRespond(QStandardItem *);
    };
}

#endif // COMPONENTS_H
