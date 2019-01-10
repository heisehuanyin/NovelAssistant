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
#include "hideidmodel.h"
#include "storydisplay.h"
#include "superdatetool.h"

/**
 * @brief Editor命名空间用于放置各种专用编辑器单元，设计用来编辑项目数据
 */
namespace Editor {

/**
 * @brief 角色基本信息编辑器，用于角色的创建、销毁和属性修改
 */
class Character:public QDialog
{
    Q_OBJECT
public:
    explicit Character(QWidget * const parent=nullptr);
    virtual ~Character();

    /**
         * @brief 获取选中条目id集合
         * @return ID集合
         */
    static QList<QVariant> getSelectedItems();

private:
    QLineEdit *const input;
    QTableView *const table;
    Support::HideIdModel *const tableModel;
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
    Component::StoryDisplay *const storyDisplay;
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
    void slot_statusChanged();
};
}


#endif // CHARACTEREDIT_H
