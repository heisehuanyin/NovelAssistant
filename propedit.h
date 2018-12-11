#ifndef PROPEDIT_H
#define PROPEDIT_H

#include "hiddedidmodel.h"

#include <QComboBox>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QObject>
#include <QPushButton>
#include <QSqlQueryModel>
#include <QTableView>
#include <QTextEdit>
namespace UIComp {

    class PropEdit:public QDialog
    {
        Q_OBJECT
    public:
        explicit PropEdit(QWidget *parent = nullptr);
        virtual ~PropEdit();

        /**
         * @brief 静态方法返回在本界面中选中的条目
         * @return 返回选中的条目的id
         */
        static QList<QVariant> getSelectedItems();

    private:
        QComboBox *const typeLimit;
        QLineEdit *const input;
        QTableView *const table;
        Support::HiddenIdModel *const tableModel;
        QPushButton *const addItem;
        QPushButton *const removeItem;
        QPushButton *const apply;
        QComboBox *const level;
        QPushButton *const levelEdit;
        QLabel *const descLine;
        QLineEdit *const value;
        QTextEdit *const descBlock;


    private slots:
        void slot_queryProps(const QString &text);
        void slot_addItem();
        void slot_clearStatus();
        void slot_responseItemSelection(const QItemSelection &, const QItemSelection &);
        void slot_responseApply();
        void slot_statusChanged();
        void slot_levelEdit();
    };
}

#endif // PROPEDIT_H
