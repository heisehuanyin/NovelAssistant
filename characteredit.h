#ifndef CHARACTEREDIT_H
#define CHARACTEREDIT_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QSqlQueryModel>
#include <QTableView>
#include <QTextEdit>
#include "storydisplay.h"


namespace UIComp {
    class CharacterEdit:public QDialog
    {
        Q_OBJECT
    public:
        explicit CharacterEdit(QWidget * const parent=nullptr);
        virtual ~CharacterEdit();

    private:
        QLineEdit *const input;
        QTableView *const table;
        QSqlQueryModel *const tableModel;
        QPushButton *const addItem;
        QPushButton *const removeItem;
        QPushButton *const apply;
        QLineEdit *const birthDay;
        QLabel *const birthDesc;
        QLineEdit *const deathDay;
        QLabel *const deathDesc;
        QListWidget *const nicknames;
        QPushButton *const addNick;
        QPushButton *const removeNick;
        UIComp::StoryDisplay *const storyDisplay;
        QSqlQueryModel *const storyListModel;
        QPushButton *const editStory;
        QTextEdit *const comment;

    private slots:
        void slot_queryCharacter(const QString &text);
        void slot_addItem();
        void slot_responseItemSelection(const QItemSelection &, const QItemSelection &);
    };
}

#endif // CHARACTEREDIT_H
