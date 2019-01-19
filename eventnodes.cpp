#include "eventnodes.h"
#include "location.h"
#include "dbtool.h"

#include <QGridLayout>
#include <QGroupBox>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>

using namespace Editor;

EventNodes::EventNodes(QWidget *parent):
    QDialog(parent),
    eventNameInput(new QLineEdit),
    addItem(new QPushButton(tr("添加阶段"))),
    removeItem(new QPushButton(tr("删除阶段"))),
    apply(new QPushButton(tr("应用变更"))),
    eventTable(new QTableView),
    eventModel(new Support::HideIdModel(this)),
    tabCon(new QTabWidget),
    evtNodeNameInput(new QLineEdit),
    birthDay(new QPushButton(tr("xxxxx年xx月xx日"))),
    deathDay(new QPushButton(tr("xxxxx年xx月xx日"))),
    beginStatus(new Support::SuperDateTool(this)),
    endStatus(new Support::SuperDateTool(this)),
    evNodeDesc(new QTextEdit),
    evNodeComment(new QTextEdit),
    localTable(new QTableView),
    locals_Model(new QStandardItemModel(this)),
    addLocation(new QPushButton(tr("添加地点"))),
    removeLocation(new QPushButton(tr("移除地点")))
{
    auto baseLayout(new QGridLayout);
    this->setLayout(baseLayout);

    baseLayout->addWidget(this->eventNameInput, 0, 0, 1, 4);
    this->connect(this->eventNameInput, &QLineEdit::textChanged,
                  this,        &EventNodes::slot_queryEventNode);
    baseLayout->addWidget(this->addItem, 0, 4);
    this->connect(this->addItem, &QPushButton::clicked,
                  this,          &EventNodes::slot_respond2Additem);
    baseLayout->addWidget(this->removeItem, 0, 5);
    this->connect(this->removeItem, &QPushButton::clicked,
                  this,             &EventNodes::slot_respond2Removeitem);
    baseLayout->addWidget(this->apply, 0, 8);
    this->connect(this->apply,  &QPushButton::clicked,
                  this,         &EventNodes::slot_4Apply);

    baseLayout->addWidget(eventTable, 1, 0, 10, 4);
    this->eventTable->setModel(this->eventModel);
    this->eventTable->setSelectionMode(QAbstractItemView::SingleSelection);
    this->eventTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    auto model = this->eventTable->selectionModel();
    this->connect(model,&QItemSelectionModel::selectionChanged,
                  this, &EventNodes::slot_targetItemChanged);

    baseLayout->addWidget(this->tabCon, 1, 4, 10, 5);

    //基本信息面板
    auto panel1 = new QWidget(this);
    this->tabCon->addTab(panel1, "基本信息");
    auto layout1(new QGridLayout);
    panel1->setLayout(layout1);
    layout1->setRowMinimumHeight(0, 20);
    layout1->setRowStretch(0,0);
    layout1->setRowMinimumHeight(1, 20);
    layout1->setRowStretch(1,0);
    layout1->setRowMinimumHeight(2, 20);
    layout1->setRowStretch(2,0);
    layout1->setRowMinimumHeight(3, 20);
    layout1->setRowStretch(3,0);

    layout1->addWidget(new QLabel(tr("名称：")));
    layout1->addWidget(this->evtNodeNameInput, 0, 1, 1, 2);
    this->connect(this->evtNodeNameInput,    &QLineEdit::textChanged,
                  this,                 &EventNodes::slot_stateChanged);

    auto time(new QGroupBox(tr("始末时间")));
    auto inb(new QGridLayout);
    time->setLayout(inb);
    inb->addWidget(new QLabel(tr("始于：")));
    inb->addWidget(this->birthDay, 0, 1, 1, 3);
    this->connect(this->birthDay, &QPushButton::clicked,
                  this,           &EventNodes::slot_editBeginTime);
    inb->addWidget(this->deathDay, 1, 1, 1, 3);
    this->connect(this->deathDay, &QPushButton::clicked,
                  this,           &EventNodes::slot_editEndTime);
    inb->addWidget(new QLabel(tr("末于：")), 1, 0);
    layout1->addWidget(time, 1, 0, 3, 3);

    auto comments(new QGroupBox(tr("事件备注")));
    auto __layout(new QHBoxLayout);
    comments->setLayout(__layout);
    __layout->addWidget(this->evNodeComment);
    layout1->addWidget(comments, 0, 3, 4, 2);
    this->connect(this->evNodeComment,  &QTextEdit::textChanged,
                  this,                 &EventNodes::slot_stateChanged);

    auto evndesc(new QGroupBox(tr("事件阶段描述")));
    auto _layout(new QGridLayout);
    evndesc->setLayout(_layout);
    _layout->addWidget(this->evNodeDesc);
    layout1->addWidget(evndesc, 4, 0, 4, 5);
    this->connect(this->evNodeDesc, &QTextEdit::textChanged,
                  this,             &EventNodes::slot_stateChanged);


    //地域性影响面板
    auto panel2 = new QWidget(this);
    this->tabCon->addTab(panel2, "地域性影响");
    auto layout2(new QGridLayout);
    panel2->setLayout(layout2);

    layout2->addWidget(this->localTable,0,0,9,4);
    this->localTable->setModel(this->locals_Model);
    this->connect(this->locals_Model,   &QStandardItemModel::itemChanged,
                  this,                 &EventNodes::slot_ItemChanged);

    layout2->addWidget(this->addLocation, 9, 0);
    this->connect(this->addLocation,    &QPushButton::clicked,
                  this,                 &EventNodes::slot_Respond2LocationAdd);
    layout2->addWidget(this->removeLocation, 9, 1);
    this->connect(this->removeLocation, &QPushButton::clicked,
                  this,                 &EventNodes::slot_Respond2LocationRemove);

    this->birthDay->setEnabled(false);
    this->deathDay->setEnabled(false);
    this->addItem->setEnabled(false);
    this->removeItem->setEnabled(false);
    this->apply->setEnabled(false);
}

EventNodes::~EventNodes(){}

QList<QVariant> EventNodes::getSelectedItems()
{
    auto instance(new EventNodes);
    instance->eventModel->changeCheckable(true);
    instance->exec();
    return instance->eventModel->selectedRecordIDs();
}

void EventNodes::slot_queryEventNode(const QString &text)
{
    this->addItem->setEnabled(false);
    this->removeItem->setEnabled(false);
    this->apply->setEnabled(false);
    if(text == QString()){
        this->eventModel->clear();
        return;
    }
    QString execStr = "select "
                      "ev_node_id, "
                      "event_name, "
                      "node_name, "
                      "comment "
                      "from table_eventnodebasic ";
    if(text != "*"){
        execStr += "where event_name like '%"+text+"%' ";
    }
    execStr += "order by begin_time;";
    this->eventModel->setQuery(4, execStr, 0);
    this->eventModel->setHorizontalHeader(0, "事件名称");
    this->eventModel->setHorizontalHeader(1, "阶段名称");
    this->eventModel->setHorizontalHeader(2, "阶段备注");

    if(this->eventModel->rowCount(QModelIndex()) == 0){
        if(text != "*")
            this->addItem->setEnabled(true);
    }else{
        this->addItem->setEnabled(false);
    }

    this->eventTable->resizeColumnsToContents();
}

void EventNodes::slot_respond2Additem()
{
    QString eName;
    qlonglong startTime=0,endTime=1;
    if(this->eventModel->rowCount(QModelIndex()) == 0){
        eName = this->eventNameInput->text();
    }else{
        auto index = this->eventTable->currentIndex();
        if(!index.isValid())
            return;
        auto eventName = this->eventModel->data(index.sibling(index.row(), 0), Qt::DisplayRole);
        if(!eventName.isValid())
            return;
        if(eventName.isNull())
            return;

        auto evnode = this->eventModel->oppositeID(index);
        auto result = Support::DBTool::getRealtimeOfEventnode(evnode.toLongLong(), startTime, endTime);
        if(!result) return;

        eName = eventName.toString();
    }

    QSqlQuery q;
    q.prepare("insert into table_eventnodebasic "
              "(node_name, event_name, begin_time, end_time, node_desc, comment) "
              "values('新节点', :eName, :start, :end, '待输入', '无备注');");
    q.bindValue(":eName", eName);
    q.bindValue(":start", startTime);
    q.bindValue(":end", endTime);

    if(!q.exec()){
        qDebug() << q.lastError();
    }
    this->eventNameInput->setText("");
    this->eventNameInput->setText(eName);
}

void EventNodes::slot_respond2Removeitem()
{
    auto index = this->eventTable->currentIndex();
    if(!index.isValid()){
        return;
    }
    auto id = this->eventModel->oppositeID(index);
    QSqlQuery q;
    q.prepare("delete "
              "from table_eventnodebasic "
              "where ev_node_id = :id;");
    q.bindValue(":id", id);
    if(!q.exec())
        qDebug() << q.lastError();
    auto name = this->eventModel->data(index.sibling(index.row(), 0), Qt::DisplayRole);
    this->eventNameInput->setText("");
    this->eventNameInput->setText(name.toString());

    this->removeItem->setEnabled(false);
}

void EventNodes::slot_targetItemChanged(const QItemSelection &, const QItemSelection &)
{
    auto index = this->eventTable->currentIndex();
    if(!index.isValid())
        return;
    auto evtid = this->eventModel->oppositeID(index);
    this->focusedID = evtid;

    this->addItem->setEnabled(true);
    this->removeItem->setEnabled(true);

    QSqlQuery q;
    q.prepare("select "
              "begin_time, "
              "end_time, "
              "node_desc, "
              "node_name, "
              "comment "
              "from table_eventnodebasic "
              "where ev_node_id = :id;");
    q.bindValue(":id", evtid);

    if(!q.exec()){
        qDebug() << q.lastError();
        return;
    }
    if(!q.next())
        return;
    auto maxtime = q.value(1);

    this->evNodeDesc->setText(q.value(2).toString());
    this->beginStatus->resetDate(q.value(0).toLongLong());
    this->evtNodeNameInput->setText(q.value(3).toString());
    auto str1 = beginStatus->toString();
    this->birthDay->setText(str1);
    this->endStatus->resetDate(q.value(1).toLongLong());
    str1 = endStatus->toString();
    this->deathDay->setText(str1);
    this->evNodeComment->setText(q.value(4).toString());

    this->birthDay->setEnabled(true);
    this->deathDay->setEnabled(true);

    this->apply->setEnabled(false);

    q.prepare("select "
              "tll.corrdinate_suffix, "
              "tll.location_name, "
              "tlc.location_desc, "
              "tlc.social_desc, "
              "tll.location_id "
              "from table_locationchange tlc "
              "inner join table_locationlist tll on tlc.location = tll.location_id "
              "inner join table_eventnodebasic tenb on tenb.ev_node_id = tlc.event_node "
              "where tenb.end_time <= :end "
              "group by tlc.location "
              "having tenb.end_time = max(tenb.end_time);");
    q.bindValue(":end", maxtime);
    if(!q.exec()){
        qDebug() << q.lastError();
        return;
    }

    this->locals_Model->clear();
    this->local_List_ids.clear();
    while (q.next()) {
        QList<QStandardItem*> row;
        for(int i=0; i<4; ++i){
            auto item(new QStandardItem(q.value(i).toString()));
            row.append(item);
            if(i<2)
                item->setEditable(false);
        }
        this->locals_Model->appendRow(row);
        this->local_List_ids.append(q.value(4));
    }
    QStringList headers;
    headers <<"前缀" <<"名称" <<"地貌景观"<<"人文表述";
    this->locals_Model->setHorizontalHeaderLabels(headers);
}

void EventNodes::slot_editBeginTime()
{
    auto time_value = this->beginStatus->dateEdit();
    if(time_value > this->endStatus->toLongLong()){
        QMessageBox::critical(this, "date error", "起始日期不可晚于终末日期！");
        return;
    }
    this->beginStatus->resetDate(time_value);
    this->birthDay->setText(this->beginStatus->toString());
    this->slot_stateChanged();
}

void EventNodes::slot_editEndTime()
{
    auto time_value = this->endStatus->dateEdit();
    if(this->beginStatus->toLongLong() > time_value){
        QMessageBox::critical(this, "date error", "起始日期不可晚于终末日期！");
        return;
    }
    this->endStatus->resetDate(time_value);
    this->deathDay->setText(this->endStatus->toString());
    this->slot_stateChanged();
}

void EventNodes::slot_stateChanged()
{
    this->apply->setEnabled(true);
}

void EventNodes::slot_4Apply()
{
    auto id = this->focusedID;

    auto nodeName = this->evtNodeNameInput->text();
    auto beginNum = this->beginStatus->toLongLong();
    auto endNum = this->endStatus->toLongLong();
    auto nodeComment = this->evNodeComment->toPlainText();
    auto nodeDesc = this->evNodeDesc->toPlainText();


    QSqlQuery q;
    q.prepare("update table_eventnodebasic "
              "set "
              "node_name = :nName, "
              "begin_time = :btime, "
              "end_time = :etime, "
              "node_desc = :desc, "
              "comment = :cmt "
              "where ev_node_id = :id ;" );
    q.bindValue(":nName", nodeName);
    q.bindValue(":btime", beginNum);
    q.bindValue(":etime", endNum);
    q.bindValue(":desc", nodeDesc);
    q.bindValue(":cmt", nodeComment);
    q.bindValue(":id", id);
    if(!q.exec())
        qDebug() << q.lastError();
    auto evname = this->eventNameInput->text();
    this->eventNameInput->setText("");
    this->eventNameInput->setText(evname);

}

void EventNodes::slot_Respond2LocationAdd()
{
    auto list = Editor::Location::getSelectedItemsID();
    //获取新增location
    QVariantList locates,evts;
    foreach(auto loc, list){
        if(!this->local_List_ids.contains(loc)){
            locates << loc;
            evts << this->focusedID;
        }
    }
    if(locates.size()==0)
        return;

    //list
    QSqlQuery q;
    q.prepare("insert into table_locationchange "
              "(location, event_node)"
              "values(?,?);");
    q.addBindValue(locates);
    q.addBindValue(evts);
    if(!q.execBatch()){
        qDebug() << q.lastError();
        return;
    }
    this->slot_targetItemChanged(QItemSelection(),QItemSelection());
}

void EventNodes::slot_Respond2LocationRemove()
{
    auto index = this->localTable->currentIndex();
    if(!index.isValid())
        return;
    auto locid = this->local_List_ids.at(index.row());

    QSqlQuery q;
    q.prepare("delete "
              "from table_locationchange "
              "where (location = :loc)and(event_node=:eid);");
    q.bindValue(":loc",locid);
    q.bindValue(":eid", this->focusedID);
    if(!q.exec())
        qDebug() << q.lastError();
    this->slot_targetItemChanged(QItemSelection(),QItemSelection());
}

void EventNodes::slot_ItemChanged(QStandardItem *item)
{
    auto index = item->index();
    auto locationid = this->local_List_ids.at(index.row());

    auto physicDesc = this->locals_Model->data(index.sibling(index.row(), 2));
    auto socialDesc = this->locals_Model->data(index.sibling(index.row(), 3));
    if(index.column()==2)
        physicDesc = item->text();
    else
        socialDesc = item->text();

    //this->focuseID
    QSqlQuery q;
    q.prepare("select "
              "id "
              "from table_locationchange "
              "where(location=:loc)and(event_node=:eid);");
    q.bindValue(":loc",locationid);
    q.bindValue(":eid", this->focusedID);
    if(!q.exec()){
        qDebug() << q.lastError();
        return;
    }
    if(q.next()){
        //如果存在，那么唯一
        auto recordID = q.value(0);
        q.prepare("update table_locationchange "
                  "set "
                  "location_desc=:ldesc, "
                  "social_desc=:sdesc "
                  "where id=:id;");
        q.bindValue(":ldesc", physicDesc);
        q.bindValue(":sdesc", socialDesc);
        q.bindValue(":id", recordID);

        if(!q.exec())
            qDebug() << q.lastError();
    }else{
        q.prepare("insert into table_locationchange "
                  "(location, event_node, location_desc, social_desc) "
                  "values(:loc, :evt, :ldesc, :sdesc);");
        q.bindValue(":loc", locationid);
        q.bindValue(":evt", this->focusedID);
        q.bindValue(":ldesc", physicDesc);
        q.bindValue(":sdesc", socialDesc);

        if(!q.exec())
            qDebug() << q.lastError();
    }
}






















