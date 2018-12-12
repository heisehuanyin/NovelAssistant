#ifndef STORYBOARD_H
#define STORYBOARD_H

#include "storydisplay.h"

#include <QComboBox>
#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QTabWidget>
#include <QTableView>
#include <QTextEdit>


namespace UIComp {

/**
 * @brief 编辑角色的故事线的主要面板
 */
class StoryBoard: public QDialog
{
public:
    /**
     * @brief 新建编辑单元
     * @param id 数据库中角色条目的绑定id
     * @param parent 编辑器的父组件
     */
    explicit StoryBoard(qlonglong id, QWidget * parent = nullptr);
    virtual ~StoryBoard() override;

private:
    qlonglong characterID;
    QLabel *const charName;
    QLabel *const nodeName;
    UIComp::StoryDisplay *const time_Story;
    QTabWidget *const tabCon;

    QPushButton *const addEvent,
                *const removeEvent;
    QComboBox *const locationSelect;
    QTextEdit *const evNode2Char_Desc;

    QPushButton *const addProp,
                *const removeProp;
    QTableView  *const propTable;

    QPushButton *const addSkill,
                *const removeSkill;
    QTableView  *const skillTable;

    QPushButton *const addRelation,
                *const removeRelation;
    QTableView  *const relationTable;

private slots:
    void slot_Response4selection(qlonglong id);
    void slot_Response4AddEventNode();
};

}
#endif // STORYBOARD_H
