#include "storyboard.h"

#include <QGridLayout>
#include <QGroupBox>

UIComp::StoryBoard::StoryBoard(QWidget *parent):
    QDialog (parent),
    charName(new QLabel),
    nodeName(new QLabel),
    time_Story(new StoryDisplay(this)),
    tabCon(new QTabWidget(this)),
    addEvent(new QPushButton(tr("添加事件"))),
    removeEvent(new QPushButton(tr("移除事件"))),
    locationSelect(new QComboBox),
    evNode2Char_Desc(new QTextEdit),
    addProp(new QPushButton(tr("添加道具"))),
    removeProp(new QPushButton(tr("移除事件"))),
    propTable(new QTableView),
    addSkill(new QPushButton(tr("添加技能"))),
    removeSkill(new QPushButton(tr("移除事件"))),
    skillTable(new QTableView),
    addRelation(new QPushButton(tr("添加关联"))),
    removeRelation(new QPushButton(tr("移除关系"))),
    relationTable(new QTableView)
{
    auto baseLayout(new QGridLayout);
    //row 12     col 8(3, 3, 2)
    this->setLayout(baseLayout);
    baseLayout->setRowStretch(0,0);
    baseLayout->setRowMinimumHeight(0, 10);

    baseLayout->addWidget(new QLabel(tr("姓名：")));
    baseLayout->addWidget(this->charName, 0, 1, 1, 2);
    baseLayout->addWidget(new QLabel(tr("事件节点：")), 0, 3);
    baseLayout->addWidget(this->nodeName, 0, 4, 1, 4);

    baseLayout->addWidget(this->time_Story, 1, 0, 11, 3);
    baseLayout->addWidget(this->tabCon, 1, 3, 11, 5);

    auto editStory(new QWidget);
    this->tabCon->addTab(editStory, "编辑阅历");
    auto esBaseLayout(new QGridLayout);
    esBaseLayout->addWidget(new QLabel(tr("添加事件节点到事件阅历：")), 0, 0, 1, 3);
    esBaseLayout->addWidget(this->addEvent, 0, 3);
    esBaseLayout->addWidget(this->removeEvent, 0, 4);
    esBaseLayout->addWidget(new QLabel(tr("角色身处地点")), 1, 0);
    esBaseLayout->addWidget(this->locationSelect, 1, 1, 1, 2);
    esBaseLayout->addWidget(this->evNode2Char_Desc, 2, 0, 8, 3);
    esBaseLayout->addWidget(new QGroupBox(tr("选中事件描述")), 1, 3, 9, 2);
    editStory->setLayout(esBaseLayout);


    auto edit4Prop(new QWidget);
    auto propLayout(new QGridLayout);
    edit4Prop->setLayout(propLayout);
    propLayout->addWidget(this->propTable, 0, 0, 8, 5);
    propLayout->addWidget(this->addProp, 8, 0);
    propLayout->addWidget(this->removeProp, 8, 1);
    this->tabCon->addTab(edit4Prop, "道具状态编辑");



    auto edit4Skill(new QWidget);
    auto skillLayout(new QGridLayout);
    edit4Skill->setLayout(skillLayout);
    skillLayout->addWidget(this->skillTable, 0, 0, 8, 5);
    skillLayout->addWidget(this->addSkill, 8, 0);
    skillLayout->addWidget(this->removeSkill, 8, 1);
    this->tabCon->addTab(edit4Skill, "技能状态编辑");


    auto edit4Relationship(new QWidget);
    auto relateLayout(new QGridLayout);
    edit4Relationship->setLayout(relateLayout);
    relateLayout->addWidget(this->relationTable, 0, 0, 8, 5);
    relateLayout->addWidget(this->addRelation, 8, 0);
    relateLayout->addWidget(this->removeRelation, 8, 1);
    this->tabCon->addTab(edit4Relationship, "社会关系状态编辑");
}

UIComp::StoryBoard::~StoryBoard()
{

}
