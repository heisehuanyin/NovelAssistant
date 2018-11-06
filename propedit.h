#ifndef PROPEDIT_H
#define PROPEDIT_H

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

    private:
        QLineEdit *const input;
        QTableView *const table;
        QSqlQueryModel *const tableModel;
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
        void slot_responseItemSelection(const QItemSelection &, const QItemSelection &);
        void slot_responseApply();
        void slot_statusChanged();
        void slot_levelEdit();
    };
}

#endif // PROPEDIT_H
