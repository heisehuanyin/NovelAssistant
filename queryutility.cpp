#include "queryutility.h"
#include "storydisplay.h"
#include "superdatetool.h"
#include "dbtool.h"

#include <QLabel>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>
#include <QVBoxLayout>

using namespace Component;

QueryUtility::QueryUtility(QWidget *parent):
    QTabWidget (parent),
    quickLook(new QTableView(this)),
    qlook(new QStandardItemModel(this)),
    ev_node(0),
    place1(new QComboBox(this)),
    previousEvents(new QTableView(this)),
    pre_Events(new QStandardItemModel(this)),
    eventsView(new StoryDisplay(this)),
    character(new QComboBox(this)),
    char_items(new QTableView(this)),
    citemsm(new QSqlQueryModel(this)),
    char_abilitys(new QTableView(this)),
    cabilitysm(new QSqlQueryModel(this)),
    char_relates(new QTableView(this)),
    crelatesm(new QSqlQueryModel(this)),
    place2(new QComboBox(this)),
    physicalView(new QTableView(this)),
    physicalModel(new QStandardItemModel(this)),
    socialView(new QTableView(this)),
    socialModel(new QStandardItemModel(this))
{
    this->addTab(this->quickLook, tr("关键词"));
    this->quickLook->setModel(this->qlook);

    //事件汇总表
    auto panel0(new QWidget(this));
    this->addTab(panel0, tr("事件"));
    auto layout0(new QVBoxLayout(panel0));
    panel0->setLayout(layout0);
    layout0->addWidget(this->place1);
    this->connect(this->place1, &QComboBox::currentTextChanged,
                  this,         &QueryUtility::refreshEventMap);
    auto tabstack(new QTabWidget(panel0));
    layout0->addWidget(tabstack);
    this->previousEvents->setModel(this->pre_Events);
    tabstack->addTab(this->previousEvents, tr("Table"));
    tabstack->addTab(this->eventsView, tr("EventsMap"));

    //查询角色演变
    auto panel1 = new QWidget(this);
    auto layout1(new QVBoxLayout(panel1));
    panel1->setLayout(layout1);
    layout1->addWidget(this->character);
    this->connect(this->character,  &QComboBox::currentTextChanged,
                  this,             &QueryUtility::refreshCharacter__About);
    auto tableSep = new QTabWidget(this);
    layout1->addWidget(tableSep);
    tableSep->addTab(this->char_items, tr("道具"));
    this->char_items->setModel(this->citemsm);
    tableSep->addTab(this->char_abilitys, tr("技能"));
    this->char_abilitys->setModel(this->cabilitysm);
    tableSep->addTab(this->char_relates, tr("人际"));
    this->char_relates->setModel(this->crelatesm);
    this->addTab(panel1, tr("角色属性"));

    //地点查询
    auto panel2 = new QWidget(this);
    auto layout2 = new QVBoxLayout(panel2);
    panel2->setLayout(layout2);
    layout2->addWidget(this->place2);
    this->connect(this->place2,     &QComboBox::currentTextChanged,
                  this,             &QueryUtility::refreshLocation_About);
    layout2->addWidget(new QLabel(tr("地貌景观")));
    layout2->addWidget(this->physicalView);
    this->physicalView->setModel(this->physicalModel);
    layout2->addWidget(new QLabel(tr("人文描述")));
    layout2->addWidget(this->socialView);
    this->socialView->setModel(this->socialModel);
    this->addTab(panel2, tr("地点演变"));

    this->map.insert("table_characterbasic", &QueryUtility::quickQueryCharacter);
    this->map.insert("table_eventnodebasic", &QueryUtility::quickQueryEvent);
    this->map.insert("table_locationlist", &QueryUtility::quickQueryPlace);
    this->map.insert("table_propbasic", &QueryUtility::quickQueryItems);
    this->map.insert("table_skilllist", &QueryUtility::quickQueryAbility);

    this->connect(this, &QueryUtility::currentChanged,
                  this, &QueryUtility::targetTabContextRefresh);
}

QueryUtility::~QueryUtility()
{

}

void QueryUtility::reset()
{
    this->character->clear();
    this->place1->clear();
    this->place2->clear();
    QSqlQuery q;
    if(!q.exec("select "
               "char_id, "
               "name "
               "from table_characterbasic ;")){
        qDebug() << q.lastError();
        return;
    }
    while (q.next()) {
        this->character->addItem(q.value(1).toString(),
                                 q.value(0).toLongLong());
    }

    if(!q.exec("select "
               "location_id, "
               "location_name, "
               "corrdinate_suffix "
               "from table_locationlist;")){
        qDebug() << q.lastError();
        return;
    }
    while (q.next()) {
        auto name = q.value(2).toString()+":"+q.value(1).toString();
        auto id = q.value(0).toLongLong();

        this->place2->addItem(name, id);
        this->place1->addItem(name, id);
    }
}


void QueryUtility::resetFocusedKeywords(qlonglong item, const QString &type)
{
    auto node = qMakePair(type, item);
    //此节点存在
    auto index = this->keywordsList.indexOf(node);
    if(index != -1){
        auto item = this->keywordsList.takeAt(index);
        this->keywordsList.insert(0, item);

        //移动此节点相关信息
        auto row = this->qlook->takeRow(index);
        this->qlook->insertRow(0, row);
    }else{
        this->keywordsList.insert(0, node);

        //获取此节点相关信息:数据驱动
        QSqlQuery q;
        auto val = this->map.value(node.first);
        (this->*val)(node,q);

        QList<QStandardItem*> row;
        if(q.next()){
            for(int i=0; i<4; i++){
                auto item = new QStandardItem(q.value(i).toString());
                row.append(item);
            }
            this->qlook->insertRow(0, row);
            this->quickLook->resizeColumnsToContents();
        }
        QStringList header;
        header << "分类" <<"种属" <<"名称" << "备注";
        this->qlook->setHorizontalHeaderLabels(header);

        for(int count=200; count < this->qlook->rowCount(); ++count){
            this->qlook->removeRow(count);
        }
    }
}

void QueryUtility::resetBaseEvent(qlonglong ev_node)
{
    this->ev_node = ev_node;
    this->targetTabContextRefresh(this->currentIndex());
}

void QueryUtility::refreshEventMap()
{
    //this->ev_node:事件节点id
    auto locid = this->place1->currentData();

    QSqlQuery q;
    q.prepare("select end_time "
              "from table_eventnodebasic "
              "where ev_node_id = :enode;");
    q.bindValue(":enode", this->ev_node);
    if(!q.exec()){
        qDebug() << q.lastError();
        return;
    }
    if(!q.next())
        return;

    auto endtime = q.value(0);


    q.prepare("select "
              "'角色', "
              "tcb.name, "
              "tenb.event_name, "
              "tenb.node_name, "
              "tclt.char_desc, "
              "tenb.begin_time, "
              "tenb.end_time, "
              "tenb.ev_node_id "
              "from table_characterlifetracker tclt "
              "inner join table_eventnodebasic tenb on tclt.event_id = tenb.ev_node_id "
              "inner join table_characterbasic tcb on tclt.char_id = tcb.char_id "
              "where (tclt.location_id = :id) and (tenb.begin_time < :time) "
              "order by tenb.begin_time desc");
    q.bindValue(":id",locid);
    q.bindValue(":time", endtime);
    if(!q.exec()){
        qDebug() << q.lastError();
        return;
    }
    this->pre_Events->clear();
    this->eventsView->clear();
    auto timeTool = new Support::SuperDateTool(this);

    //table layout: 分类，名称，事件名称（事件名+节点名），阶段简述，开始时间，结束时间

    while (q.next()) {
        QList<QStandardItem*> row;
        QString eventName = "";

        for(int i=0; i<7; ++i){
            //事件名称处理
            if(i==2){
                auto item = new QStandardItem(q.value(2).toString()+":"+q.value(3).toString());
                eventName = item->text();
                row.append(item);
                continue;
            }else if(i==3)
                continue;
            //涉及到日期处理
            if(i==5 || i==6){
                timeTool->resetDate(q.value(i).toLongLong());
                auto item = new QStandardItem(timeTool->toString());
                row.append(item);
                continue;
            }
            auto item = new QStandardItem(q.value(i).toString());
            row.append(item);
        }
        this->pre_Events->appendRow(row);
        this->eventsView->addEvent(q.value(7).toLongLong(), eventName, q.value(5).toLongLong(), q.value(6).toLongLong());
    }
    QStringList header;
    header <<"分类" <<"名称" <<"事件" <<"阶段简述" <<"开始时间" <<"结束时间";
    this->pre_Events->setHorizontalHeaderLabels(header);
    this->previousEvents->resizeColumnsToContents();
}

void QueryUtility::refreshCharacter__About()
{
    auto char_id = this->character->currentData();

    qlonglong anyTime,endtime;
    auto result = Support::DBTool::getRealtimeOfEventnode(this->ev_node, anyTime, endtime);
    if(!result) return;


    QSqlQuery q;
    result = Support::DBTool::getCharactersPropsUntilTime(false, char_id.toLongLong(), endtime, q);
    if(!result) return;

    this->citemsm->setQuery(q);
    this->citemsm->setHeaderData(0, Qt::Horizontal, "名称");
    this->citemsm->setHeaderData(1, Qt::Horizontal, "类型");
    this->citemsm->setHeaderData(2, Qt::Horizontal, "级别");
    this->citemsm->setHeaderData(3, Qt::Horizontal, "数量");
    this->citemsm->setHeaderData(4, Qt::Horizontal, "备注");
    this->char_items->resizeColumnsToContents();

    result = Support::DBTool::getCharactersRelationshipUntilTime(false, char_id.toLongLong(), endtime, q);
    if(!result) return;

    this->crelatesm->setQuery(q);
    this->crelatesm->setHeaderData(0, Qt::Horizontal, "角色");
    this->crelatesm->setHeaderData(1, Qt::Horizontal, "关系");
    this->crelatesm->setHeaderData(2, Qt::Horizontal, "备注");
    this->char_relates->resizeColumnsToContents();

    result = Support::DBTool::getCharactersSkillsUntilTime(false, char_id.toLongLong(), endtime, q);
    if(!result) return;

    this->cabilitysm->setQuery(q);
    this->cabilitysm->setHeaderData(0, Qt::Horizontal, "名称");
    this->cabilitysm->setHeaderData(1, Qt::Horizontal, "类型");
    this->cabilitysm->setHeaderData(2, Qt::Horizontal, "等级");
    this->cabilitysm->setHeaderData(3, Qt::Horizontal, "备注");
    this->char_abilitys->resizeColumnsToContents();
}

void QueryUtility::refreshLocation_About()
{
    qlonglong anyTime,endtime;
    auto result = Support::DBTool::getRealtimeOfEventnode(this->ev_node, anyTime, endtime);
    if(!result) return;

    auto locid = this->place2->currentData();

    QSqlQuery q;
    q.prepare("select "
              "enb.event_name, "
              "enb.node_name, "
              "lc.location_desc, "
              "lc.social_desc "
              "from table_locationchange lc "
              "inner join table_locationlist ll on lc.location = ll.location_id "
              "inner join table_eventnodebasic enb on lc.event_node = enb.ev_node_id "
              "where (lc.location = :loc) and (enb.end_time <= :time) "
              "order by enb.end_time desc;");
    q.bindValue(":time", endtime);
    q.bindValue(":loc", locid);

    if(!q.exec()){
        qDebug() << q.lastError();
        return;
    }
    this->physicalModel->clear();
    this->socialModel->clear();

    while (q.next()) {
        auto name = q.value(0).toString()+":"+q.value(1).toString();
        QList<QStandardItem*> row1;
        row1.append(new QStandardItem(name));
        row1.append(new QStandardItem(q.value(2).toString()));
        this->physicalModel->appendRow(row1);

        QList<QStandardItem*> row2;
        row2.append(new QStandardItem(name));
        row2.append(new QStandardItem(q.value(3).toString()));
        this->socialModel->appendRow(row2);
    }
    this->physicalView->resizeColumnsToContents();
    this->socialView->resizeColumnsToContents();
}

void QueryUtility::targetTabContextRefresh(int index)
{
    switch (index) {
    case 0:break;
    case 1:this->refreshEventMap();break;
    case 2:this->refreshCharacter__About();break;
    case 3:this->refreshLocation_About();break;
    default:qDebug() << "error";
    }
}
// 分类        种属            名称         备注
void QueryUtility::quickQueryCharacter(QPair<QString, qlonglong> &node, QSqlQuery &exec)
{
    exec.prepare("select "
                 "'人物', "
                 "'无', "
                 "name, "
                 "comment "
                 "from table_characterbasic "
                 "where char_id = :id;");
    exec.bindValue(":id", node.second);
    if(!exec.exec()){
        qDebug() << exec.lastError();
    }
}

void QueryUtility::quickQueryItems(QPair<QString, qlonglong> &node, QSqlQuery &exec)
{
    exec.prepare("select "
                 "'道具', "
                 "tg.type_name, "
                 "tp.name, "
                 "tp.comment "
                 "from table_gtm tg "
                 "inner join table_propbasic tp on tg.mark_id = tp.mark "
                 "where tp.prop_id = :id;");
    exec.bindValue(":id", node.second);
    if(!exec.exec())
        qDebug() << exec.lastError();
}

void QueryUtility::quickQueryAbility(QPair<QString, qlonglong> &node, QSqlQuery &exec)
{
    exec.prepare("select "
                 "'技能', "
                 "tg.type_name, "
                 "ts.name, "
                 "ts.comment "
                 "from table_gtm tg "
                 "inner join table_skilllist ts on tg.mark_id = ts.mark "
                 "where ts.skill_id = :id;");
    exec.bindValue(":id", node.second);
    if(!exec.exec())
        qDebug() << exec.lastError();
}

void QueryUtility::quickQueryPlace(QPair<QString, qlonglong> &node, QSqlQuery &exec)
{
    exec.prepare("select "
                 "'地点', "
                 "corrdinate_suffix, "
                 "location_name, "
                 "comment "
                 "from table_locationlist "
                 "where location_id = :id;");
    exec.bindValue(":id", node.second);
    if(!exec.exec())
        qDebug() << exec.lastError();
}

void QueryUtility::quickQueryEvent(QPair<QString, qlonglong> &node, QSqlQuery &exec)
{
    exec.prepare("select "
                 "'事件', "
                 "event_name, "
                 "node_name, "
                 "comment "
                 "from table_eventnodebasic "
                 "where ev_node_id = :id;");
    exec.bindValue(":id", node.second);
    if(!exec.exec())
        qDebug() << exec.lastError();
}

















