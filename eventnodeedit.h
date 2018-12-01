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

    private:
        QLineEdit   *const input;
        QPushButton *const addItem,
                    *const removeItem,
                    *const apply;

        QTableView  *const eventTable;
        Support::HiddenIdModel *const eventModel;

        QPushButton *const birthDay,
                    *const deathDay;
        Support::SuperDateTool *const birthStatus,
                               *const deathStatus;
        QLineEdit   *const evNameInput;
        QTextEdit   *const evNodeDesc;

        QGroupBox   *const effect;
        QLineEdit   *const localInput;
        QTableView  *const localTable;
        QTableView  *const localProps;
        QLabel      *const pre_socialDesc;
        QTextEdit   *const socialDesc;
        QLabel      *const pre_sufaceDesc;
        QTextEdit   *const sufaceDesc;

    private slots:
        void slot_queryEventNode(const QString &text);
        void slot_respond2Additem();
        void slot_targetItemChanged(const QItemSelection &, const QItemSelection &);
        void slot_editBeginTime();
        void slot_editEndTime();
    };

}
#endif // EVENTNODEEDIT_H
