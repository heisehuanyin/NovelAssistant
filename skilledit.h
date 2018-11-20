#ifndef SKILLEDIT_H
#define SKILLEDIT_H

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

    class SkillEdit:public QDialog
    {
        Q_OBJECT
    public:
        explicit SkillEdit(QWidget *parent = nullptr);
        virtual ~SkillEdit();

    private:
        QComboBox *const typeLimit;
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
        void slot_querySkills(const QString &text);
        void slot_addItem();
        void slot_clearStatus();
        void slot_responseItemSelection(const QItemSelection &, const QItemSelection &);
        void slot_responseApply();
        void slot_statusChanged();
        void slot_levelEdit();
    };
}


#endif // SKILLEDIT_H
