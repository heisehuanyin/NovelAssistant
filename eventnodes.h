#ifndef EVENTNODEEDIT_H
#define EVENTNODEEDIT_H

#include <QDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
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
    QPushButton *const addLocation;
    QPushButton *const removeLocation;
    QTableView  *const localProps;
    QLabel      *const pre_socialDesc;
    QTextEdit   *const socialDesc;
    QLabel      *const pre_physicalDesc;
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
};

}

#endif // EVENTNODEEDIT_H
