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
    explicit StoryBoard(qlonglong id,QWidget * parent = nullptr);
    virtual ~StoryBoard() override;

private:
    qlonglong characterID;
    qlonglong focuseEvent;
    QList<qlonglong> eventNodeList;
    QLabel *const charName;
    QLabel *const nodeName;
    UIComp::StoryDisplay *const time_Story;
    QTabWidget *const tabCon;

    QPushButton *const addEvent,
                *const removeEvent,
                *const apply;
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

    /**
     * @brief 通过本函数显示指定角色的人生轨迹
     * @param id 数据库中角色条目的绑定id
     */
    void displayCharacterLifetracker(qlonglong id);

    /**
     * @brief 通过本函数显示指定角色的指定事件节点的详细变更情况
     * @param eventId 事件节点id，默认值-1，表示选择为空，清空状态信息
     */
    void displayEventDetial(qlonglong eventId = -1);

private slots:
    /**
     * @brief 在列表中选中目标事件节点的时候，本函数将会被自动调用
     * @param id 选中事件节点id
     */
    void slot_Response4Focuse(qlonglong id);
    void slot_Response4Defocuse();

    /**
     * @brief 调用本函数添加一个事件节点到角色的人生经历中
     */
    void slot_Response4AddEventNode();
};

}
#endif // STORYBOARD_H
