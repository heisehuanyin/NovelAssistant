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
#include "hiddedidmodel.h"
#include "superdatetool.h"


namespace UIComp {
    class EventnodeEdit:public QDialog
    {
        Q_OBJECT
    public:
        explicit EventnodeEdit(QWidget *parent=nullptr);
        virtual ~EventnodeEdit();

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
        Support::HiddenIdModel *const eventModel;

        QTabWidget  *const tabCon;

        QLineEdit   *const evNameInput;
        QPushButton *const birthDay,
                    *const deathDay;
        Support::SuperDateTool *const birthStatus,
                               *const deathStatus;
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
        void slot_targetItemChanged(const QItemSelection &, const QItemSelection &);
        void slot_editBeginTime();
        void slot_editEndTime();
    };

}
#endif // EVENTNODEEDIT_H
