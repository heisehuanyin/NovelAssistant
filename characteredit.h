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
#include "hiddedidmodel.h"
#include "storydisplay.h"
#include "superdatetool.h"


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
        Support::HiddenIdModel *const tableModel;
        QPushButton *const addItem;
        QPushButton *const removeItem;
        QPushButton *const apply;
        QPushButton *const birthDay;
        Support::SuperDateTool *const birthEdit;
        QPushButton *const deathDay;
        Support::SuperDateTool *const deathEdit;
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
        void slot_removeItem();
        void slot_4apply();
        void slot_responseItemSelection(const QItemSelection &, const QItemSelection &);
        void slot_birthEdit();
        void slot_deathEdit();
        void slot_addNickName();
        void slot_removeNickName();
        void slot_editCharcterStory();
    };
}

#endif // CHARACTEREDIT_H
