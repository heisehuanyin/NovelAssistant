#ifndef LOCATIONEDIT_H
#define LOCATIONEDIT_H

#include "hideidmodel.h"

#include <QDialog>
#include <QGroupBox>
#include <QLineEdit>
#include <QListView>
#include <QListWidget>
#include <QPushButton>
#include <QSpinBox>
#include <QSqlQueryModel>
#include <QStandardItemModel>
#include <QTableWidget>
#include <QTextEdit>

namespace Editor {

    class Location:public QDialog
    {
        Q_OBJECT
    public:
        explicit Location(QWidget* parent=nullptr);
        virtual ~Location();

        static QList<QVariant> getSelectedItemsID();

    private:
        QLineEdit *const locName;
        QPushButton *const locAdd;
        QPushButton *const locRemove;
        QTableView *const locTable;
        Support::HideIdModel * const locQueryModel;
        QLineEdit *const suffixInput;
        QSpinBox *const xposInput;
        QSpinBox *const yposInput;
        QSpinBox *const zposInput;
        QTextEdit *const storyInput;
        QListWidget *const nickNames;
        QPushButton *const nickAdd;
        QPushButton *const nickRemove;
        QPushButton *const apply;

    private slots:
        void slot_queryLocation(const QString &text);
        void slot_addLocation();
        void slot_removeLocation();
        void slot_statusChanged();
        void slot_changeApply();
        void slot_responseItemSelection(const QItemSelection &selected, const QItemSelection &);
        void slot_addNick();
        void slot_removeNick();
    };

}

#endif // LOCATIONEDIT_H
