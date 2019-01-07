#ifndef STORYBOARD_H
#define STORYBOARD_H

#include "storydisplay.h"

#include <QComboBox>
#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QStandardItemModel>
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
    QPushButton *const apply;
    UIComp::StoryDisplay *const time_Story;
    QTabWidget *const tabCon;

    QPushButton *const addENode,
                *const removeEvent;
    QComboBox *const locationSelect;
    QTextEdit *const evNode2Char_Desc;

    QPushButton *const addProp,
                *const removeProp;
    QWidget *const edit4Prop;
    QTableView  *const propTable;
    QStandardItemModel *const propModel;
    QList<qlonglong> ids_prop;
    /**
     * @brief 本函数用于刷新表单状态对table的每一个更改都会调用本函数进行刷新
     * @param evnodeId 事件节点id
     * @param charid 角色id
     */
    void displayPropChange(qlonglong charid, qlonglong evnodeId);



    QPushButton *const addSkill,
                *const removeSkill;
    QTableView  *const skillTable;
    QStandardItemModel *const skillModel;
    QList<qlonglong> ids_skill;
    /**
     * @brief 本函数用于刷新表单状态对table的每一个更改都会调用本函数进行刷新
     * @param evnodeId 事件节点id
     * @param charid 角色id
     */
    void displaySkillChange(qlonglong charid, qlonglong evnodeId);



    QPushButton *const addRelation,
                *const removeRelation;
    QTableView  *const relationTable;
    QStandardItemModel *const relationModel;
    QList<qlonglong> ids_relate;
    /**
     * @brief 本函数用于刷新表单状态对table的每一个更改都会调用本函数进行刷新
     * @param evnodeId 事件节点id
     * @param charid 角色id
     */
    void displayRelationChange(qlonglong charid, qlonglong evnodeId);





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

    /**
     * @brief 响应道具增加操作
     */
    void slot_Response4AddPropType();
    void slot_Response4RemovePropType();
    void slot_Response4PropModify(QStandardItem* item);

    /**
     * @brief 响应技能增加操作
     */
    void slot_Response4AddSkillType();
    void slot_Response4RemoveSkillType();
    void slot_Response4SkillModify(QStandardItem* item);

    /**
     * @brief 增加人际关系操作
     */
    void slot_Response4AddRelation();
    void slot_Response4RemoveRelation();
    void slot_Response4RelationChange(QStandardItem* item);
};

}
#endif // STORYBOARD_H
