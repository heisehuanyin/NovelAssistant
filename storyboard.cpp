#include "eventnodes.h"
#include "storyboard.h"
#include "items.h"
#include "ability.h"
#include "character.h"
#include "dbtool.h"

#include <QGridLayout>
#include <QGroupBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QtDebug>
#include <QMessageBox>

using namespace Editor;

StoryBoard::StoryBoard(qlonglong id, QWidget *parent):
    QDialog (parent),
    characterID(id),
    focuseEvent(-1),
    charName(new QLabel),
    nodeName(new QLabel),
    apply(new QPushButton(tr("应用变更"))),
    time_Story(new Component::StoryDisplay(this)),
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
    relationTable(new QTableView),
    relationModel(new QStandardItemModel),
    addCharacter(new QPushButton(tr("添加角色")))
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
    this->connect(this->apply,  &QPushButton::clicked,
                  this,         &StoryBoard::slot_Response4ApplyEventChange);

    baseLayout->addWidget(this->time_Story, 1, 0, 11, 3);
    this->connect(this->time_Story, &Component::StoryDisplay::focuse,
                  this,             &StoryBoard::slot_Response4Focuse);
    this->connect(this->time_Story, &Component::StoryDisplay::deFocuse,
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


    auto relatePanel(new QWidget(this));
    this->tabCon->addTab(relatePanel, "社会关系状态编辑");
    auto relateLayout(new QGridLayout(relatePanel));
    relatePanel->setLayout(relateLayout);
    relateLayout->addWidget(this->relationTable, 0, 0, 8, 4);
    this->relationTable->setModel(this->relationModel);
    relateLayout->addWidget(this->addCharacter, 8, 0);
    this->connect(this->addCharacter,   &QPushButton::clicked,
                  this,                 &StoryBoard::slot_AddRelationship);
    this->connect(this->relationModel,  &QStandardItemModel::itemChanged,
                  this,                 &StoryBoard::slot_Response4RelationChange);

    this->displayCharacterLifetracker(id);
}

StoryBoard::~StoryBoard()
{
    delete this->charName;
    delete this->nodeName;
    delete this->apply;
    delete this->time_Story;
    delete this->tabCon;
}

void StoryBoard::displayProp(qlonglong charid, qlonglong evnodeId)
{
    qlonglong maxTime,start;
    auto result = Support::DBTool::getRealtimeOfEventnode(evnodeId, start, maxTime);
    if(!result) return;

    QSqlQuery q;
    //[items_id, ]items_name, items_type, items_grade, items_number, record_comment
    result = Support::DBTool::getCharactersPropsUntilTime(true, charid, maxTime, q);
    if(!result) return;

    this->ids_prop.clear();
    this->propModel->clear();
    while (q.next()) {
        this->ids_prop.append(q.value(0).toLongLong());

        QList<QStandardItem*> row;
        for(int i=1; i< 6; ++i){
            auto cell = new QStandardItem(q.value(i).toString());
            row.append(cell);
            if(i >= 4)
                cell->setEditable(true);
        }

        this->propModel->appendRow(row);
    }

    this->propModel->setHeaderData(0, Qt::Horizontal, "道具名");
    this->propModel->setHeaderData(1, Qt::Horizontal, "类型");
    this->propModel->setHeaderData(2, Qt::Horizontal, "级别");
    this->propModel->setHeaderData(3, Qt::Horizontal, "道具数量");
    this->propModel->setHeaderData(4, Qt::Horizontal, "备注");
    this->propTable->resizeColumnsToContents();
}

void StoryBoard::displaySkills(qlonglong charid, qlonglong evnodeId)
{

    qlonglong maxTime,start;
    auto result = Support::DBTool::getRealtimeOfEventnode(evnodeId, start, maxTime);
    if(!result) return;

    QSqlQuery q;
    result = Support::DBTool::getCharactersSkillsUntilTime(true, charid, maxTime, q);
    if(!result) return;

    this->ids_skill.clear();
    this->skillModel->clear();
    while (q.next()) {
        this->ids_skill.append(q.value(0).toLongLong());

        QList<QStandardItem*> row;
        for(int i=1; i<5; ++i){
            auto cell = new QStandardItem(q.value(i).toString());
            row.append(cell);
            if(i==4)
                cell->setEditable(true);
        }

        this->skillModel->appendRow(row);
    }
    this->skillModel->setHeaderData(0, Qt::Horizontal, "技能名称");
    this->skillModel->setHeaderData(1, Qt::Horizontal, "类型");
    this->skillModel->setHeaderData(2, Qt::Horizontal, "等级");
    this->skillModel->setHeaderData(3, Qt::Horizontal, "备注");
    this->skillTable->resizeColumnsToContents();
}

void StoryBoard::displayRelation(qlonglong charid, qlonglong evnodeId)
{

    qlonglong start, maxTime;
    auto result = Support::DBTool::getRealtimeOfEventnode(evnodeId, start, maxTime);
    if(!result) return;

    QSqlQuery q;

    //[target_id, ]target_name, relationship, relationcomment
    result = Support::DBTool::getCharactersRelationshipUntilTime(true, charid, maxTime, q);
    if(!result)
        return;

    this->ids_character.clear();
    this->relationModel->clear();
    while (q.next()) {
        this->ids_character.append(q.value(0).toLongLong());

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
    this->relationTable->resizeColumnsToContents();
}

void StoryBoard::displayCharacterLifetracker(qlonglong id)
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

        this->time_Story->addEvent(q.value(0).toLongLong(), name, q.value(3).toLongLong(), q.value(4).toLongLong());
        this->eventNodeList.append(q.value(0).toLongLong());
    }
}

void StoryBoard::displayEventDetial(qlonglong eventId)
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
               "location_name, "
               "corrdinate_suffix "
               "from table_locationlist;"))
        qDebug() << q.lastError();

    this->locationSelect->clear();
    while (q.next()) {
        this->locationSelect->addItem(q.value(2).toString()+":"+q.value(1).toString(),
                                      q.value(0));
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
    this->displayProp(this->characterID, this->focuseEvent);
    //技能
    this->displaySkills(this->characterID, this->focuseEvent);
    //社会关系
    this->displayRelation(this->characterID, this->focuseEvent);
}

void StoryBoard::slot_Response4Focuse(qlonglong id)
{
    this->focuseEvent = id;
    this->displayEventDetial(id);
}

void StoryBoard::slot_Response4Defocuse()
{
    this->focuseEvent = -1;
    this->displayEventDetial();
}

void StoryBoard::slot_Response4AddEventNode()
{
    qlonglong unknownLocation;
    {
        QSqlQuery prp;
prepareStep:
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

    auto list = Editor::EventNodes::getSelectedItems();
    QVariantList charids, evnodes, locations;

    for(int i=0; i< list.size(); ++i){
        if(this->eventNodeList.contains(list.at(i).toLongLong()))
            continue;
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

void StoryBoard::slot_Response4RemoveEventNode()
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

void StoryBoard::slot_Response4ApplyEventChange()
{
    auto loc_id = this->locationSelect->currentData();

    this->updateRelationshipOnLocationChanged(this->characterID, loc_id.toLongLong());
    QSqlQuery q;
    q.prepare("update table_characterlifetracker "
              "set "
              "location_id = :locID, "
              "char_desc = :desc "
              "where (char_id = :cid) and "
              "(event_id = :eid);");
    q.bindValue(":locID", loc_id);
    q.bindValue(":desc", this->evNode2Char_Desc->toPlainText());
    q.bindValue(":cid", this->characterID);
    q.bindValue(":eid", this->focuseEvent);
    if(!q.exec())
        qDebug() << q.lastError();
}

void StoryBoard::updateRelationshipOnLocationChanged(qlonglong character, qlonglong locid)
{
    //获取之前的角色位置
    QSqlQuery q;
    qlonglong p_loc;
    qlonglong maxTime;
    auto evtExists = Support::DBTool::getRealLocationOnEventnode(this->focuseEvent, character, p_loc, maxTime);
    if(!evtExists)
        return;

    //没变化就结束
    if(p_loc == locid)
        return;

    //获取指定地点、指定时间、对指定对象相关的关系记录条目
    q.prepare("SELECT tcrs.id "
              "FROM table_characterrelationship tcrs "
              "INNER JOIN "
              "table_characterlifetracker tclt ON (tcrs.event_id = tclt.event_id AND "
              "tcrs.char_id = tclt.char_id) "
              "WHERE (tcrs.event_id = :eid) AND "
              "(tclt.location_id = :p_loc) AND "
              "(tcrs.char_id = :cid) "
              "UNION "
              "SELECT tcrs.id "
              "FROM table_characterrelationship tcrs "
              "INNER JOIN "
              "table_characterlifetracker tclt ON (tcrs.event_id = tclt.event_id AND "
              "tcrs.target_id = tclt.char_id) "
              "WHERE (tcrs.event_id = :eid) AND "
              "(tclt.location_id = :p_loc) AND "
              "(tcrs.target_id = :cid);");
    q.bindValue(":eid", this->focuseEvent);
    q.bindValue(":p_loc", p_loc);
    q.bindValue(":cid", character);

    if(!q.exec()){
        qDebug() << q.lastError();
        return;
    }
    QVariantList iddel;
    while (q.next()) {
        iddel << q.value(0);
    }

    //删除此事件之前地点确立的关系
    if(iddel.size()>0){
        q.prepare("delete from table_characterrelationship "
                  "where id=?;");
        q.addBindValue(iddel);
        q.execBatch();
    }


    //获取指定事件发生时在新地点出现的角色
    q.prepare("select "
              "char_id "
              "from table_characterlifetracker "
              "where(event_id=:eid)and(location_id=:loc);");
    q.bindValue(":eid", this->focuseEvent);
    q.bindValue(":loc", locid);
    if(!q.exec()){
        qDebug() << q.lastError();
        return;
    }
    QVariantList targets, hosts, events;
    while (q.next()) {
        targets << q.value(0);
        hosts << character;
        events << this->focuseEvent;
    }

    auto result = Support::DBTool::getCharactersRelationshipUntilTime(true, character, maxTime, q);
    if(!result)
        return;

    //已经确立关系的需要过滤掉，除非手动修改，否则保持关系
    while (q.next()) {
        if(targets.contains(q.value(0))){
            targets.removeAll(q.value(0));
            hosts.removeFirst();
            events.removeFirst();
        }
    }

    //批量添加指定角色与当地(未曾建立过关系的)角色的关系
    if(targets.size() > 0){
        q.prepare("insert into table_characterrelationship "
                  "(char_id, target_id, relationship, event_id)"
                  "values(?,?,'路人关系', ?);");
        q.addBindValue(hosts);
        q.addBindValue(targets);
        q.addBindValue(events);
        if(!q.execBatch()){
            qDebug() << q.lastError();
            return;
        }
    }


    //最后更新tracker条目信息
    q.prepare("update table_characterlifetracker "
              "set "
              "location_id=:loc "
              "where (char_id=:cid)and(event_id=:eid)");
    q.bindValue(":loc", locid);
    q.bindValue(":cid", character);
    q.bindValue(":eid", this->focuseEvent);
    if(!q.exec())
        qDebug() << q.lastError();
}

void StoryBoard::slot_Response4AddPropType()
{
    auto list = Editor::Items::getSelectedItems();

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

    this->displayProp(this->characterID, this->focuseEvent);
}

void StoryBoard::slot_Response4RemovePropType()
{
    auto index = this->propTable->currentIndex();
    if(!index.isValid())
        return;

    auto id = this->ids_prop.at(index.row());

    QSqlQuery q;
    q.prepare("delete from table_characterpropchange "
              "where (char_id=:cid)and(prop = :id);");
    q.bindValue(":cid", this->characterID);
    q.bindValue(":id", id);
    if(!q.exec())
        qDebug() << q.lastError();

    this->displayProp(this->characterID, this->focuseEvent);
}

void StoryBoard::slot_Response4PropModify(QStandardItem *item)
{
    auto index = item->index();
    if(!index.isValid())
        return;

    auto propType = this->ids_prop.at(index.row());
    auto propNum = this->propModel->data(index.sibling(index.row(), 3)).toLongLong();
    auto propCmt = this->propModel->data(index.sibling(index.row(), 4));

    QSqlQuery q;
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

        q.prepare("update table_characterpropchange "
                  "set "
                  "number = :num, "
                  "comment = :cmt "
                  "where id = :id; ");
        q.bindValue(":num", propNum);
        q.bindValue(":cmt", propCmt);
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
        q.bindValue(":num", propNum);
        q.bindValue("cmt", propCmt);

        if(!q.exec())
            qDebug() << q.lastError();

    }
    this->displayProp(this->characterID, this->focuseEvent);
}

void StoryBoard::slot_Response4AddSkillType()
{
    if(this->focuseEvent == -1){
        QMessageBox::critical(nullptr, "OPERATE ERROR", "未选择事件");
        return;
    }
    QVariantList chars, evnodes, skills;
    auto list = Editor::Ability::getSelectedItems();
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

    this->displaySkills(this->characterID, this->focuseEvent);
}

void StoryBoard::slot_Response4RemoveSkillType()
{
    auto index = this->skillTable->currentIndex();
    if(!index.isValid())
        return;
    auto skill_id = this->ids_skill.at(index.row());

    QSqlQuery q;
    q.prepare("delete from table_characterskills "
              "where (skill = :id) and (character=:cid);");
    q.bindValue(":id", skill_id);
    q.bindValue(":cid", this->characterID);

    if(!q.exec()){
        qDebug() << q.lastError();
        return;
    }

    this->displaySkills(this->characterID, this->focuseEvent);
}

void StoryBoard::slot_Response4SkillModify(QStandardItem *item)
{
    auto index = item->index();
    if(!index.isValid())
        return;
    auto skillType = this->ids_skill.at(index.row());

    QSqlQuery q;
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
        q.bindValue(":cid", this->characterID);
        q.bindValue(":skill", skillType);
        q.bindValue(":enode", this->focuseEvent);
        q.bindValue(":cmt", item->text());
    }
    if(!q.exec()){
        qDebug() << q.lastError();
        return;
    }
    this->displaySkills(this->characterID, this->focuseEvent);
}

void StoryBoard::slot_Response4RelationChange(QStandardItem *item)
{
    auto index = item->index();
    if(!index.isValid())
        return;
    auto charid = this->ids_character.at(index.row());

    auto relationship = this->relationModel->data(index.sibling(index.row(), 1));
    auto comment = this->relationModel->data(index.sibling(index.row(), 2));

    QSqlQuery q;
    q.prepare("SELECT "
              "id "
              "FROM table_characterrelationship "
              "WHERE (char_id = :id) AND "
              "(target_id = :tid) AND "
              "(event_id = :node) "
              "UNION "
              "SELECT "
              "id "
              "FROM table_characterrelationship "
              "WHERE (char_id = :tid) AND "
              "(target_id = :id) AND "
              "(event_id = :node);");
    q.bindValue(":id", this->characterID);
    q.bindValue(":tid", charid);
    q.bindValue(":node", this->focuseEvent);
    if(!q.exec()){
        qDebug() << q.lastError();
        return;
    }
    if(q.next()){
        //如果条目存在，那么唯一
        auto itemId = q.value(0);
        //修改关系
        q.prepare("update table_characterrelationship "
                  "set "
                  "relationship = :relate, "
                  "comment = :cmt "
                  "where id = :id;");
        q.bindValue(":relate", relationship);
        q.bindValue(":cmt", comment);
        q.bindValue(":id", itemId);
        if(!q.exec()){
            qDebug() << q.lastError();
            return;
        }
    }else{
        q.prepare("insert into table_characterrelationship "
                  "(char_id, target_id, relationship, event_id, comment) "
                  "values(:cid, :tid, :relate, :evt, :cmt);");
        q.bindValue(":cid", this->characterID);
        q.bindValue(":tid", charid);
        q.bindValue(":relate", relationship);
        q.bindValue(":evt", this->focuseEvent);
        q.bindValue(":cmt", comment);

        if(!q.exec()){
            qDebug() << q.lastError();
            return;
        }
    }

    qlonglong anyWhere, anyTime;
    //指定角色是否经历过该事件
    qlonglong eventExists = Support::DBTool::getRealLocationOnEventnode(this->focuseEvent, charid, anyWhere, anyTime);

    qlonglong here;
    Support::DBTool::getRealLocationOnEventnode(this->focuseEvent, this->characterID, here, anyTime);

    //指定角色在指定时刻经历此事件
    if(eventExists){
        this->updateRelationshipOnLocationChanged(charid, here);
    }else{
        q.prepare("insert into table_characterlifetracker "
                  "(char_id, event_id, location_id, char_desc, comment) "
                  "values(:char, :evt, :loc, '未知', '无备注');");
        q.bindValue(":char", charid);
        q.bindValue(":evt", this->focuseEvent);
        q.bindValue(":loc", here);
        if(q.exec())
            qDebug() << q.lastError();
    }

    this->displayRelation(this->characterID, this->focuseEvent);
}

void StoryBoard::slot_AddRelationship()
{
    qlonglong here,anytime; //基准角色此时的真实位置
    Support::DBTool::getRealLocationOnEventnode(this->focuseEvent, this->characterID, here, anytime);

    auto list = Editor::Character::getSelectedItems();

    QVariantList relation_hosts,relation_targets, relation_events;
    QVariantList tracker_chars,tracker_evts,tracker_locs;
    foreach (auto item, list) {
        if(this->ids_character.contains(item.toLongLong()) || item.toLongLong() == this->characterID)
            continue;
        relation_targets << item;
        relation_hosts << this->characterID;
        relation_events << this->focuseEvent;

        //检测指定对象lifetracker是否包含本事件
        //如果包含(之前在不同地点经历过这个事件)，更新关系，删除就有，添加新的关系，相当于改变地点了
        //如果不包含，在lifetracker中添加本事件
        qlonglong anyWhere;
        bool eventExists = Support::DBTool::getRealLocationOnEventnode(this->focuseEvent,
                                                                   item.toLongLong(),
                                                                   anyWhere,
                                                                   anytime);
        //指定角色存在对该事件的记录
        if(eventExists){
            this->updateRelationshipOnLocationChanged(item.toLongLong(), here);
        }else{
            tracker_chars << item;
            tracker_evts << this->focuseEvent;
            tracker_locs << here;
        }
    }

    if(relation_targets.size()==0)
        return;

    if(tracker_chars.size() > 0){
        QSqlQuery q;
        q.prepare("insert into table_characterlifetracker "
                  "(char_id, event_id, location_id, char_desc, comment) "
                  "values(?, ?, ?, '未知', '无备注');");
        q.addBindValue(tracker_chars);
        q.addBindValue(tracker_evts);
        q.addBindValue(tracker_locs);
        if(!q.execBatch()){
            qDebug() << q.lastError();
            return;
        }
    }

    QSqlQuery q;
    q.prepare("insert into table_characterrelationship "
              "(char_id, target_id, event_id, relationship, comment) "
              "values(?,?,?,'路人关系', '无备注');");
    q.addBindValue(relation_hosts);
    q.addBindValue(relation_targets);
    q.addBindValue(relation_events);
    if(!q.execBatch()){
        qDebug() << q.lastError();
        return;
    }
    this->displayRelation(this->characterID, this->focuseEvent);
}

