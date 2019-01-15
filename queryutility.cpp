#include "queryutility.h"

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
    ev_node(-1),
    place1(new QComboBox(this)),
    previousEvents(new QTableView(this)),
    pre_Events(new QSqlQueryModel(this)),
    character(new QComboBox(this)),
    char_items(new QTableView(this)),
    char_abilitys(new QTableView(this)),
    char_relates(new QTableView(this)),
    place2(new QComboBox(this)),
    placeItemsTable(new QTableView(this)),
    physicalView(new QTextEdit(this)),
    socialView(new QTextEdit(this))
{
    this->addTab(this->quickLook, tr("关键词"));
    this->quickLook->setModel(this->qlook);

    //事件汇总表
    auto panel0(new QWidget(this));
    auto layout0(new QVBoxLayout(panel0));
    panel0->setLayout(layout0);
    layout0->addWidget(this->place1);
    layout0->addWidget(this->previousEvents);
    this->previousEvents->setModel(this->pre_Events);
    this->addTab(panel0, tr("事件"));

    //查询角色演变
    auto panel1 = new QWidget(this);
    auto layout1(new QVBoxLayout(panel1));
    panel1->setLayout(layout1);
    layout1->addWidget(this->character);
    auto tableSep = new QTabWidget(this);
    layout1->addWidget(tableSep);
    tableSep->addTab(this->char_items, tr("道具"));
    tableSep->addTab(this->char_abilitys, tr("技能"));
    tableSep->addTab(this->char_relates, tr("人际"));
    this->addTab(panel1, tr("角色"));

    //地点查询
    auto panel2 = new QWidget(this);
    auto layout2 = new QVBoxLayout(panel2);
    panel2->setLayout(layout2);
    layout2->addWidget(this->place2);
    layout2->addWidget(this->placeItemsTable);
    layout2->addWidget(new QLabel(tr("物理状态")));
    layout2->addWidget(this->physicalView);
    layout2->addWidget(new QLabel(tr("人文状态")));
    layout2->addWidget(this->socialView);
    this->addTab(panel2, tr("地点"));

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
}

void QueryUtility::refreshEventMap()
{
    //this->ev_node:事件节点id
    QSqlQuery q;

}

void QueryUtility::refreshCharacter__About()
{

}

void QueryUtility::refreshPlace_About()
{

}

void QueryUtility::targetTabContextRefresh(int index)
{
    switch (index) {
    case 0:break;
    case 1:this->refreshEventMap();break;
    case 2:this->refreshCharacter__About();break;
    case 3:this->refreshPlace_About();break;
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

















