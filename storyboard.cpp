#include "eventnodeedit.h"
#include "storyboard.h"
#include "propedit.h"
#include "skilledit.h"
#include "charedit.h"

#include <QGridLayout>
#include <QGroupBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QtDebug>
#include <QMessageBox>

UIComp::StoryBoard::StoryBoard(qlonglong id, QWidget *parent):
    QDialog (parent),
    characterID(id),
    focuseEvent(-1),
    charName(new QLabel),
    nodeName(new QLabel),
    apply(new QPushButton(tr("应用变更"))),
    time_Story(new StoryDisplay(this)),
    tabCon(new QTabWidget(this)),
    addENode(new QPushButton(tr("添加事件"))),
    removeEvent(new QPushButton(tr("移除事件"))),
    locationSelect(new QComboBox),
    evNode2Char_Desc(new QTextEdit),
    addProp(new QPushButton(tr("添加道具"))),
    removeProp(new QPushButton(tr("删除源条目"))),
    edit4Prop(new QWidget),
    propTable(new QTableView),
    propModel(new QStandardItemModel),
    addSkill(new QPushButton(tr("添加技能"))),
    removeSkill(new QPushButton(tr("删除源条目"))),
    skillTable(new QTableView),
    skillModel(new QStandardItemModel),
    addRelation(new QPushButton(tr("添加关联"))),
    removeRelation(new QPushButton(tr("删除源条目"))),
    relationTable(new QTableView),
    relationModel(new QStandardItemModel)
{
    auto baseLayout(new QGridLayout);
    //row 12     col 8(3, 3, 2)
    this->setLayout(baseLayout);
    baseLayout->setRowStretch(0,0);
    baseLayout->setRowMinimumHeight(0, 10);

    baseLayout->addWidget(new QLabel(tr("姓名：")));
    baseLayout->addWidget(this->charName, 0, 1, 1, 2);
    baseLayout->addWidget(new QLabel(tr("事件节点：")), 0, 3);
    baseLayout->addWidget(this->nodeName, 0, 4, 1, 3);
    baseLayout->addWidget(this->apply, 0, 7);

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
    esBaseLayout->addWidget(this->addENode, 0, 1);
    this->connect(this->addENode,   &QPushButton::clicked,
                  this,             &StoryBoard::slot_Response4AddEventNode);
    esBaseLayout->addWidget(this->removeEvent, 0, 2);
    this->connect(this->removeEvent,&QPushButton::clicked,
                  this,             &StoryBoard::slot_Response4RemoveEventNode);
    esBaseLayout->addWidget(new QLabel(tr("角色身处地点")), 1, 0);
    esBaseLayout->addWidget(this->locationSelect, 1, 1, 1, 2);
    esBaseLayout->addWidget(this->evNode2Char_Desc, 2, 0, 8, 3);
    editStory->setLayout(esBaseLayout);


    auto propLayout(new QGridLayout(this->edit4Prop));
    this->edit4Prop->setLayout(propLayout);
    propLayout->addWidget(this->propTable, 0, 0, 8, 5);
    propLayout->addWidget(this->addProp, 8, 0);
    this->connect(this->addProp,    &QPushButton::clicked,
                  this,             &StoryBoard::slot_Response4AddPropType);
    propLayout->addWidget(this->removeProp, 8, 1);
    this->connect(this->removeProp, &QPushButton::clicked,
                  this,             &StoryBoard::slot_Response4RemovePropType);
    this->tabCon->addTab(this->edit4Prop, "道具状态编辑");
    this->propTable->setModel(this->propModel);
    this->connect(this->propModel,  &QStandardItemModel::itemChanged,
                  this,             &StoryBoard::slot_Response4PropModify);



    auto edit4Skill(new QWidget);
    auto skillLayout(new QGridLayout);
    edit4Skill->setLayout(skillLayout);
    skillLayout->addWidget(this->skillTable, 0, 0, 8, 5);
    skillLayout->addWidget(this->addSkill, 8, 0);
    this->connect(this->addSkill,   &QPushButton::clicked,
                  this,             &StoryBoard::slot_Response4AddSkillType);
    skillLayout->addWidget(this->removeSkill, 8, 1);
    this->connect(this->removeSkill,&QPushButton::clicked,
                  this,             &StoryBoard::slot_Response4RemoveSkillType);
    this->tabCon->addTab(edit4Skill, "技能状态编辑");
    this->skillTable->setModel(this->skillModel);
    this->connect(this->skillModel, &QStandardItemModel::itemChanged,
                  this,             &StoryBoard::slot_Response4SkillModify);


    auto edit4Relationship(new QWidget);
    auto relateLayout(new QGridLayout);
    edit4Relationship->setLayout(relateLayout);
    relateLayout->addWidget(this->relationTable, 0, 0, 8, 5);
    relateLayout->addWidget(this->addRelation, 8, 0);
    this->connect(this->addRelation,    &QPushButton::clicked,
                  this,                 &StoryBoard::slot_Response4AddRelation);
    relateLayout->addWidget(this->removeRelation, 8, 1);
    this->connect(this->removeRelation, &QPushButton::clicked,
                  this,                 &StoryBoard::slot_Response4RemoveRelation);
    this->tabCon->addTab(edit4Relationship, "社会关系状态编辑");
    this->relationTable->setModel(this->relationModel);
    this->connect(this->relationModel,  &QStandardItemModel::itemChanged,
                  this,                 &StoryBoard::slot_Response4RelationChange);

    this->displayCharacterLifetracker(id);
}

UIComp::StoryBoard::~StoryBoard()
{
    delete this->charName;
    delete this->nodeName;
    delete this->apply;
    delete this->time_Story;
    delete this->tabCon;
}

void UIComp::StoryBoard::displayPropChange(qlonglong charid, qlonglong evnodeId)
{
    qlonglong maxTime;
    {
        QSqlQuery q;
        q.prepare("select "
                  "enb.end_time "
                  "from table_eventnodebasic enb "
                  "where enb.ev_node_id = :id");
        q.bindValue(":id", evnodeId);
        if(!q.exec())
            qDebug() << q.lastError();
        if(!q.next())
            return;
        maxTime = q.value(0).toLongLong();
    }

    QSqlQuery q;
    q.prepare("SELECT cpc.id, "
                    "pb.name, "
                    "cpc.number, "
                    "cpc.comment "
              "FROM table_characterpropchange cpc "
                "INNER JOIN "
                "table_eventnodebasic enb ON cpc.event_node = enb.ev_node_id "
                "INNER JOIN "
                "table_propbasic pb ON cpc.prop = pb.prop_id "
              "WHERE (enb.end_time <= :endtime) and "
                    "(cpc.char_id = :id) "
            "GROUP BY cpc.prop "
              "HAVING enb.end_time = max(enb.end_time);");
    q.bindValue(":endtime", maxTime);
    q.bindValue(":id", charid);
    if(!q.exec())
        qDebug() << q.lastError();

    this->ids_prop.clear();
    this->propModel->clear();
    while (q.next()) {
        this->ids_prop.append(q.value(0).toLongLong());

        QList<QStandardItem*> row;
        auto cell1 = new QStandardItem(q.value(1).toString());
        row.append(cell1);

        auto cell2 = new QStandardItem(q.value(2).toString());
        cell2->setEditable(true);
        row.append(cell2);

        auto cell3 = new QStandardItem(q.value(3).toString());
        cell3->setEditable(true);
        row.append(cell3);

        this->propModel->appendRow(row);
    }

    this->propModel->setHeaderData(0, Qt::Horizontal, "道具名");
    this->propModel->setHeaderData(1, Qt::Horizontal, "道具数量");
    this->propModel->setHeaderData(2, Qt::Horizontal, "备注");
}

void UIComp::StoryBoard::displaySkillChange(qlonglong charid, qlonglong evnodeId)
{

    qlonglong maxTime;
    {
        QSqlQuery q;
        q.prepare("select "
                  "enb.end_time "
                  "from table_eventnodebasic enb "
                  "where enb.ev_node_id = :id");
        q.bindValue(":id", evnodeId);
        if(!q.exec())
            qDebug() << q.lastError();
        if(!q.next())
            return;
        maxTime = q.value(0).toLongLong();
    }

    QSqlQuery q2;
    q2.prepare("SELECT cs.id, "
                 "sl.name, "
                 "cs.comment "
                "FROM table_characterskills cs "
                    "INNER JOIN "
                    "table_eventnodebasic enb ON cs.event_node = enb.ev_node_id "
                    "INNER JOIN "
                    "table_skilllist sl ON cs.skill = sl.skill_id "
                "WHERE (cs.character = :csid) AND "
                    "(enb.end_time <= :endtime);");
    q2.bindValue(":csid", charid);
    q2.bindValue(":endtime", maxTime);

    if(!q2.exec())
        qDebug() << q2.lastError();

    this->ids_skill.clear();
    this->skillModel->clear();
    while (q2.next()) {
        this->ids_skill.append(q2.value(0).toLongLong());

        QList<QStandardItem*> row;
        auto cell1 = new QStandardItem(q2.value(1).toString());
        row.append(cell1);

        auto cell2 = new QStandardItem(q2.value(2).toString());
        cell2->setEditable(true);
        row.append(cell2);

        this->skillModel->appendRow(row);
    }
    this->skillModel->setHeaderData(0, Qt::Horizontal, "技能名称");
    this->skillModel->setHeaderData(1, Qt::Horizontal, "备注");
}

void UIComp::StoryBoard::displayRelationChange(qlonglong charid, qlonglong evnodeId)
{

    qlonglong maxTime;
    {
        QSqlQuery q;
        q.prepare("select "
                  "enb.end_time "
                  "from table_eventnodebasic enb "
                  "where enb.ev_node_id = :id");
        q.bindValue(":id", evnodeId);
        if(!q.exec())
            qDebug() << q.lastError();
        if(!q.next())
            return;
        maxTime = q.value(0).toLongLong();
    }

    QSqlQuery q;
    q.prepare("SELECT crs.id, "
                "cb.name, "
                "crs.relationship, "
                "crs.comment "
              "FROM table_characterrelationship crs "
                "INNER JOIN "
                "table_eventnodebasic enb ON crs.event_id = enb.ev_node_id "
                "INNER JOIN "
                "table_characterbasic cb ON crs.target_id = cb.char_id "
              "WHERE (crs.char_id = :id) AND "
                "(enb.end_time <= :endt) "
              "GROUP BY crs.target_id "
                "HAVING enb.end_time = max(enb.end_time);");
    q.bindValue(":id", charid);
    q.bindValue(":endt", maxTime);

    if(!q.exec())
        qDebug() << q.lastError();

    this->ids_relate.clear();
    this->relationModel->clear();
    while (q.next()) {
        this->ids_relate.append(q.value(0).toLongLong());

        QList<QStandardItem*> row;
        auto cell1 = new QStandardItem(q.value(1).toString());
        row.append(cell1);

        auto cell2 = new QStandardItem(q.value(2).toString());
        cell2->setEditable(true);
        row.append(cell2);

        auto cell3 = new QStandardItem(q.value(3).toString());
        cell3->setEditable(true);
        row.append(cell3);

        this->relationModel->appendRow(row);
    }
    this->relationModel->setHeaderData(0, Qt::Horizontal, "角色名称");
    this->relationModel->setHeaderData(1, Qt::Horizontal, "关系描述");
    this->relationModel->setHeaderData(2, Qt::Horizontal, "备注");

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
    this->time_Story->clear();
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

        this->propModel->clear();
        this->skillModel->clear();
        this->relationModel->clear();
        return;
    }



    //地点和简述
    QSqlQuery q;
    if(!q.exec("select "
              "location_id, "
              "location_name "
              "from table_locationlist;"))
        qDebug() << q.lastError();

    this->locationSelect->clear();
    while (q.next()) {
        this->locationSelect->addItem(q.value(1).toString(), q.value(0));
    }



    q.prepare("select "
              "event_name, "
              "node_name "
              "from table_eventnodebasic "
              "where ev_node_id = :id;");
    q.bindValue(":id", eventId);
    if(!q.exec())
        qDebug() << q.lastError();
    if(q.next())
        this->nodeName->setText(q.value(0).toString() + ":" + q.value(1).toString());





    q.prepare("select "
              "location_id, "
              "char_desc "
              "from table_characterlifetracker "
              "where (char_id = :id) and (event_id = :evNode);");
    q.bindValue(":id", this->characterID);
    q.bindValue(":evNode", eventId);
    if(!q.exec())
        qDebug() << q.lastError();

    if(q.next()){
        for(int cycle=0; cycle < this->locationSelect->count(); ++cycle){
            if(this->locationSelect->itemData(cycle) == q.value(0)){
                this->locationSelect->setCurrentIndex(cycle);
                break;
            }
        }
        this->evNode2Char_Desc->setText(q.value(1).toString());
    }else
        return;

    //道具
    this->displayPropChange(this->characterID, this->focuseEvent);
    //技能
    this->displaySkillChange(this->characterID, this->focuseEvent);
    //社会关系
    this->displayRelationChange(this->characterID, this->focuseEvent);
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

void UIComp::StoryBoard::slot_Response4RemoveEventNode()
{
    if(this->focuseEvent==-1)
        return;
    QSqlQuery q;
    q.prepare("delete from table_characterlifetracker "
              "where (char_id = :cid) and (event_id=:enode);");
    q.bindValue(":cid", this->characterID);
    q.bindValue(":enode", this->focuseEvent);
    if(!q.exec()){
        qDebug() << q.lastError();
        return;
    }
    q.prepare("delete from table_characterpropchange "
              "where (char_id=:cid) and (event_node=:enode);");
    q.bindValue(":cid",this->characterID);
    q.bindValue(":enode", this->focuseEvent);
    if(!q.exec()){
        qDebug() << q.lastError();
        return;
    }
    q.prepare("delete from table_characterrelationship "
              "where (char_id=:cid) and (event_id=:enode);");
    q.bindValue(":cid",this->characterID);
    q.bindValue(":enode", this->focuseEvent);
    if(!q.exec()){
        qDebug() << q.lastError();
        return;
    }
    q.prepare("delete from table_characterskills "
              "where(character=:cid) and (event_node=:enode);");
    q.bindValue(":cid", this->characterID);
    q.bindValue(":enode", this->focuseEvent);
    if(!q.exec()){
        qDebug() << q.lastError();
        return;
    }
    this->displayCharacterLifetracker(this->characterID);
    this->displayEventDetial();
}

void UIComp::StoryBoard::slot_Response4AddPropType()
{
    auto list = PropEdit::getSelectedItems();

    QVariantList c_ids,ev_ids,id_props;
    for(int i=0; i<list.length(); ++i){
        auto item = list.at(i);
        if(this->ids_prop.contains(item.toLongLong()))
            continue;
        id_props << item;
        c_ids << this->characterID;
        ev_ids << this->focuseEvent;
    }
    if(id_props.size() == 0)
        return;
    if(this->focuseEvent == -1){
        QMessageBox::critical(nullptr, "OPERATE ERROR", "未选择事件");
        return;
    }

    QSqlQuery q;
    q.prepare("insert into table_characterpropchange "
              "(char_id, event_node, prop, number, comment) "
              "values(?, ?, ?, 1, '无备注');");
    q.addBindValue(c_ids);
    q.addBindValue(ev_ids);
    q.addBindValue(id_props);

    if(!q.execBatch())
        qDebug() << q.lastError();

    this->displayPropChange(this->characterID, this->focuseEvent);
}

void UIComp::StoryBoard::slot_Response4RemovePropType()
{
    auto index = this->propTable->currentIndex();
    if(!index.isValid())
        return;

    auto id = this->ids_prop.at(index.row());

    QSqlQuery q;
    q.prepare("delete from table_characterpropchange "
              "where id = :id;");
    q.bindValue(":id", id);
    if(!q.exec())
        qDebug() << q.lastError();

    this->displayPropChange(this->characterID, this->focuseEvent);
}

void UIComp::StoryBoard::slot_Response4PropModify(QStandardItem *item)
{
    auto index = item->index();
    if(!index.isValid())
        return;
    auto id = this->ids_prop.at(index.row());

    QSqlQuery q;

    //获取道具种类
    q.prepare("select prop, "
              "number, "
              "comment "
              "from table_characterpropchange "
              "where id = :id;");
    q.bindValue(":id", id);
    if(!q.exec())
        qDebug() << q.lastError();
    q.next();
    auto propType = q.value(0);
    auto propNum = q.value(1);
    auto propCmt = q.value(2);


    //检测是否当前事件中存在该条目记录
    q.prepare("select "
              "id "
              "from table_characterpropchange "
              "where (char_id = :cid) and (event_node = :enode) and (prop = :prop);");
    q.bindValue(":cid", this->characterID);
    q.bindValue(":enode", this->focuseEvent);
    q.bindValue(":prop", propType);

    if(!q.exec())
        qDebug() << q.lastError();

    if(q.next()){
        //targetId便是唯一的条目，如果存在的话
        auto targetId = q.value(0);
        if(index.column() == 1){
            q.prepare("update table_characterpropchange "
                      "set "
                      "number = :num "
                      "where id = :id; ");
            q.bindValue(":num", item->text().toLongLong());
        }
        else if(index.column() == 2){
            q.prepare("update table_characterpropchange "
                      "set "
                      "comment = :cmt "
                      "where id = :id;");
            q.bindValue(":cmt", item->text());
        }

        q.bindValue(":id", targetId);

        if(!q.exec())
            qDebug() << q.lastError();
    }
    else{
        q.prepare("insert into table_characterpropchange "
                  "(char_id, event_node, prop, number, comment) "
                  "values(:cid, :enode, :prop, :num, :cmt);");

        q.bindValue(":cid", this->characterID);
        q.bindValue(":enode", this->focuseEvent);
        q.bindValue(":prop", propType);
        if(index.column() == 1){
            q.bindValue(":num", item->text().toLongLong());
            q.bindValue("cmt", propCmt);
        }
        else if(index.column() == 2){
            q.bindValue(":cmt", item->text());
            q.bindValue("num", propNum);
        }

        if(!q.exec())
            qDebug() << q.lastError();

    }

    this->displayPropChange(this->characterID, this->focuseEvent);

}

void UIComp::StoryBoard::slot_Response4AddSkillType()
{
    if(this->focuseEvent == -1){
        QMessageBox::critical(nullptr, "OPERATE ERROR", "未选择事件");
        return;
    }
    QVariantList chars, evnodes, skills;
    auto list = SkillEdit::getSelectedItems();
    for(int i=0; i< list.size(); ++i){
        auto item = list.at(i);
        if(this->ids_skill.contains(item.toLongLong()))
            continue;
        chars << this->characterID;
        evnodes << this->focuseEvent;
        skills << item;
    }
    if(skills.size() == 0)
        return;

    QSqlQuery q;
    q.prepare("insert into table_characterskills "
              "(character, event_node, skill, comment)"
              "values(?, ?, ?, '无备注');");
    q.addBindValue(chars);
    q.addBindValue(evnodes);
    q.addBindValue(skills);

    if(!q.execBatch()){
        qDebug() << q.lastError();
        return;
    }

    this->displaySkillChange(this->characterID, this->focuseEvent);
}

void UIComp::StoryBoard::slot_Response4RemoveSkillType()
{
    auto index = this->skillTable->currentIndex();
    if(!index.isValid())
        return;
    auto id = this->ids_skill.at(index.row());

    QSqlQuery q;
    q.prepare("delete from table_characterskills "
              "where id = :id;");
    q.bindValue(":id", id);
    if(!q.exec()){
        qDebug() << q.lastError();
        return;
    }

    this->displaySkillChange(this->characterID, this->focuseEvent);
}

void UIComp::StoryBoard::slot_Response4SkillModify(QStandardItem *item)
{
    auto index = item->index();
    if(!index.isValid())
        return;
    auto id = this->ids_skill.at(index.row());

    QSqlQuery q;
    q.prepare("select skill "
              "from table_characterskills "
              "where id = :id;");
    q.bindValue(":id", id);
    if(!q.exec()){
        qDebug() << q.lastError();
        return;
    }
    if(!q.next())
        return;
    //肯定能获得唯一 skill_id
    auto skillType = q.value(0);

    q.prepare("select id "
              "from table_characterskills "
              "where (character = :cid) and "
              "(event_node = :enode) and "
              "(skill = :skill);");
    q.bindValue(":cid", this->characterID);
    q.bindValue(":enode", this->focuseEvent);
    q.bindValue(":skill", skillType);

    if(!q.exec()){
        qDebug() << q.lastError();
        return;
    }
    if(q.next()){
        auto recordId = q.value(0);
        q.prepare("update table_characterskills "
                  "set "
                  "comment = :cmt "
                  "where id = :id;");
        q.bindValue(":id", recordId);
        q.bindValue(":cmt", item->text());
    }else{
        q.prepare("insert into table_characterskills "
                  "(character, skill, event_node, comment) "
                  "values(:cid, :skill, :enode, :cmt);");
        q.bindValue("cid", this->characterID);
        q.bindValue(":skill", skillType);
        q.bindValue(":enode", this->focuseEvent);
        q.bindValue(":cmt", item->text());
    }
    if(!q.exec()){
        qDebug() << q.lastError();
        return;
    }
    this->displaySkillChange(this->characterID, this->focuseEvent);
}

void UIComp::StoryBoard::slot_Response4AddRelation()
{
    if(this->focuseEvent == -1){
        QMessageBox::critical(nullptr, "OPERATE ERROR", "未选择事件");
        return;
    }
    QVariantList target, evnodes, host;
    auto list = CharEdit::getSelectedItems();
    for(int i=0; i<list.size(); ++i){
        auto item = list.at(i);
        if(this->ids_relate.contains(item.toLongLong()))
            continue;
        target << item;
        evnodes << this->focuseEvent;
        host << this->characterID;
    }
    if(target.size() == 0)
        return;

    QSqlQuery q;
    q.prepare("insert into table_characterrelationship "
              "(char_id, target_id, relationship, event_id, comment) "
              "values(?, ?, '路人关系', ?, '无备注');");
    q.addBindValue(host);
    q.addBindValue(target);
    q.addBindValue(evnodes);
    if(!q.execBatch()){
        qDebug() << q.lastError();
        return;
    }
    this->displayRelationChange(this->characterID, this->focuseEvent);
}

void UIComp::StoryBoard::slot_Response4RemoveRelation()
{
    auto index = this->relationTable->currentIndex();
    if(!index.isValid())
        return;
    auto id = this->ids_relate.at(index.row());

    QSqlQuery q;
    q.prepare("delete from table_characterrelationship "
              "where id = :id;");
    q.bindValue(":id", id);
    if(!q.exec()){
        qDebug() << q.lastError();
        return;
    }
    this->displayRelationChange(this->characterID, this->focuseEvent);
}

void UIComp::StoryBoard::slot_Response4RelationChange(QStandardItem *item)
{
    auto index = item->index();
    if(!index.isValid())
        return;
    auto id = this->ids_relate.at(index.row());
    QSqlQuery q;
    q.prepare("select "
              "target_id, "
              "relationship, "
              "comment "
              "from table_characterrelationship "
              "where id = :id;");
    q.bindValue(":id", id);
    if(!q.exec()){
        qDebug() << q.lastError();
        return;
    }
    if(!q.next())
        return;
    auto target_id = q.value(0);
    auto relationship = q.value(1);
    auto comment = q.value(2);

    q.prepare("select "
              "id "
              "from table_characterrelationship "
              "where(char_id = :id) and (target_id = :tid) and (event_id = :node);");
    q.bindValue(":id", this->characterID);
    q.bindValue(":tid", target_id);
    q.bindValue(":node", this->focuseEvent);
    if(!q.exec()){
        qDebug() << q.lastError();
        return;
    }
    if(q.next()){
        //如果条目存在，那么唯一
        auto itemId = q.value(0);
        //修改关系
        if(index.column() == 1){
            q.prepare("update table_characterrelationship "
                      "set "
                      "relationship = :relate "
                      "where id = :id;");
            q.bindValue(":relate", item->text());
        }
        //修改注释
        else if(index.column() == 2){
            q.prepare("update table_characterrelationship "
                      "set "
                      "comment = :cmt "
                      "where id = :id;");
            q.bindValue(":cmt", item->text());
        }
        q.bindValue(":id", itemId);
    }else{
        q.prepare("insert into table_characterrelationship "
                  "(char_id, target_id, relationship, event_id, comment) "
                  "values(:cid, :tid, :relate, :node, :cmt);");
        q.bindValue(":cid", this->characterID);
        q.bindValue(":tid", target_id);
        q.bindValue("node", this->focuseEvent);

        if(index.column() == 1){
            q.bindValue(":relate", item->text());
            q.bindValue(":cmt", comment);
        }else if(index.column() == 2){
            q.bindValue("relate", relationship);
            q.bindValue(":cmt", item->text());
        }
    }

    if(!q.exec()){
        qDebug() << q.lastError();
        return;
    }
    this->displayRelationChange(this->characterID, this->focuseEvent);
}
