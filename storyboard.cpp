#include "eventnodeedit.h"
#include "storyboard.h"

#include <QGridLayout>
#include <QGroupBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QtDebug>

UIComp::StoryBoard::StoryBoard(qlonglong id, QWidget *parent):
    QDialog (parent),
    characterID(id),
    focuseEvent(-1),
    charName(new QLabel),
    nodeName(new QLabel),
    time_Story(new StoryDisplay(this)),
    tabCon(new QTabWidget(this)),
    addEvent(new QPushButton(tr("添加事件"))),
    removeEvent(new QPushButton(tr("移除事件"))),
    apply(new QPushButton(tr("应用变更"))),
    locationSelect(new QComboBox),
    evNode2Char_Desc(new QTextEdit),
    addProp(new QPushButton(tr("添加道具"))),
    removeProp(new QPushButton(tr("移除道具"))),
    propTable(new QTableView),
    addSkill(new QPushButton(tr("添加技能"))),
    removeSkill(new QPushButton(tr("移除技能"))),
    skillTable(new QTableView),
    addRelation(new QPushButton(tr("添加关联"))),
    removeRelation(new QPushButton(tr("移除关联"))),
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
    this->connect(this->time_Story, &UIComp::StoryDisplay::focuse,
                  this,             &StoryBoard::slot_Response4Focuse);
    this->connect(this->time_Story, &UIComp::StoryDisplay::deFocuse,
                  this,             &StoryBoard::slot_Response4Defocuse);

    baseLayout->addWidget(this->tabCon, 1, 3, 11, 5);

    auto editStory(new QWidget);
    this->tabCon->addTab(editStory, "编辑阅历");
    auto esBaseLayout(new QGridLayout);
    esBaseLayout->addWidget(new QLabel(tr("添加事件节点：")), 0, 0);
    esBaseLayout->addWidget(this->addEvent, 0, 1);
    this->connect(this->addEvent,   &QPushButton::clicked,
                  this,             &StoryBoard::slot_Response4AddEventNode);
    esBaseLayout->addWidget(this->removeEvent, 0, 2);
    esBaseLayout->addWidget(this->apply, 0, 4);
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

    this->displayCharacterLifetracker(id);
}

UIComp::StoryBoard::~StoryBoard()
{

}

void UIComp::StoryBoard::displayCharacterLifetracker(qlonglong id)
{
    QSqlQuery q;
    q.prepare("select "
              "name "
              "from table_characterbasic "
              "where char_id = :id;");
    q.bindValue(":id", id);
    if(!q.exec()){
        qDebug() << q.lastError();
        this->charName->setText("未知角色，sql错误");
        return;
    }
    q.next();
    this->charName->setText(q.value(0).toString());


    q.prepare("select "
              "enb.ev_node_id, "
              "enb.event_name, "
              "enb.node_name, "
              "enb.begin_time, "
              "enb.end_time "
              "from table_characterlifetracker clt inner join table_eventnodebasic enb "
              "on clt.event_id = enb.ev_node_id "
              "where clt.char_id = :id;");
    q.bindValue(":id", characterID);
    if(!q.exec()){
        qDebug() << q.lastError();
        return;
    }

    this->eventNodeList.clear();
    while (q.next()) {
        auto name = q.value(1).toString();
        name += ":" + q.value(2).toString();

        auto node(new UIComp::EventSymbo(name, q.value(3).toLongLong(), q.value(4).toLongLong()));
        this->time_Story->addEvent(q.value(0).toLongLong(), node);
        this->eventNodeList.append(q.value(0).toLongLong());
    }
}

void UIComp::StoryBoard::displayEventDetial(qlonglong eventId)
{
    if(eventId == -1){
        this->nodeName->setText("");
        this->locationSelect->clear();
        this->evNode2Char_Desc->setText("");


    }
    //地点和简述

    //道具

    //技能

    //社会关系
}

void UIComp::StoryBoard::slot_Response4Focuse(qlonglong id)
{
    this->focuseEvent = id;
    this->displayEventDetial(id);
}

void UIComp::StoryBoard::slot_Response4Defocuse()
{
    this->focuseEvent = -1;
    this->displayEventDetial();
}

void UIComp::StoryBoard::slot_Response4AddEventNode()
{
    qlonglong unknownLocation;
    {
    prepareStep:
        QSqlQuery prp;
        prp.prepare("select "
                    "location_id "
                    "from table_locationlist "
                    "where location_name = '未知';");
        if(!prp.exec()){
            qDebug() << prp.lastError();
        }
        if(!prp.next()){
            prp.prepare("insert into table_locationlist "
                        "(location_name, corrdinate_suffix, xposition, yposition, zposition, nickname)"
                        "values('未知', '无', 0, 0, 0, '');");
            if(!prp.exec())
                qDebug() << prp.lastError();
            goto prepareStep;
        }else{
            unknownLocation = prp.value(0).toLongLong();
        }
    }

    auto list = UIComp::EventnodeEdit::getSelectedItems();
    QVariantList charids, evnodes, locations;

    for(int i=0; i< list.size(); ++i){
        charids << this->characterID;
        evnodes << list.at(i);
        locations << unknownLocation;
    }

    QSqlQuery q;
    q.prepare("insert into table_characterlifetracker "
              "(char_id, event_id, location_id)"
              "values (?, ?, ?);");
    q.addBindValue(charids);
    q.addBindValue(evnodes);
    q.addBindValue(locations);

    if(!q.execBatch())
        qDebug() << q.lastError();

    this->displayCharacterLifetracker(this->characterID);
    this->displayEventDetial();
}
