#ifndef EVENTNODEEDIT_H
#define EVENTNODEEDIT_H

#include <QDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QSqlQueryModel>
#include <QStandardItemModel>
#include <QTableView>
#include <QTextEdit>
#include "hideidmodel.h"
#include "superdatetool.h"

namespace Editor {

class EventNodes : public QDialog
{
    Q_OBJECT
public:
    explicit EventNodes(QWidget *parent=nullptr);
    virtual ~EventNodes();

    /**
         * @brief 获取选中条目id集合
         * @return ID集合
         */
    static QList<QVariant> getSelectedItems();

private:
    QLineEdit   *const input;
    QPushButton *const addItem,
    *const removeItem,
    *const apply;

    QTableView  *const eventTable;
    Support::HideIdModel *const eventModel;

    QTabWidget  *const tabCon;

    QLineEdit   *const evNameInput;
    QPushButton *const birthDay,
    *const deathDay;
    Support::SuperDateTool *const beginStatus,
    *const endStatus;
    QTextEdit   *const evNodeDesc;
    QTextEdit   *const evNodeComment;

    QTableView  *const localTable;
    QStandardItemModel * const locals_Model;
    QList<QVariant> local_List_ids;
    QPushButton *const addLocation;
    QPushButton *const removeLocation;
    QTextEdit   *const socialDesc;
    QTextEdit   *const physicalDesc;

private slots:
    void slot_queryEventNode(const QString &text);
    void slot_respond2Additem();
    void slot_respond2Removeitem();
    void slot_targetItemChanged(const QItemSelection &, const QItemSelection &);
    void slot_editBeginTime();
    void slot_editEndTime();
    void slot_stateChanged();
    void slot_4Apply();

    void slot_Respond2LocationAdd();
    void slot_Respond2LocationRemove();
    void slot_TargetLocationChanged(const QModelIndex&, const QModelIndex&);
};

}

#endif // EVENTNODEEDIT_H
