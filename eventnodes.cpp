#include "eventnodes.h"
#include "location.h"

#include <QGridLayout>
#include <QGroupBox>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>

using namespace Editor;

EventNodes::EventNodes(QWidget *parent):
    QDialog(parent),
    input(new QLineEdit),
    addItem(new QPushButton(tr("添加阶段"))),
    removeItem(new QPushButton(tr("删除阶段"))),
    apply(new QPushButton(tr("应用变更"))),
    eventTable(new QTableView),
    eventModel(new Support::HideIdModel(this)),
    tabCon(new QTabWidget),
    evNameInput(new QLineEdit),
    birthDay(new QPushButton(tr("xxxxx年xx月xx日"))),
    deathDay(new QPushButton(tr("xxxxx年xx月xx日"))),
    beginStatus(new Support::SuperDateTool(this)),
    endStatus(new Support::SuperDateTool(this)),
    evNodeDesc(new QTextEdit),
    evNodeComment(new QTextEdit),
    localTable(new QTableView),
    locals_Model(new QStandardItemModel(this)),
    addLocation(new QPushButton(tr("添加地点"))),
    removeLocation(new QPushButton(tr("移除地点"))),
    socialDesc(new QTextEdit),
    physicalDesc(new QTextEdit)
{
    auto bLayout(new QGridLayout);
    this->setLayout(bLayout);

    bLayout->addWidget(this->input, 0, 0, 1, 4);
    this->connect(this->input, &QLineEdit::textChanged,
                  this,        &EventNodes::slot_queryEventNode);
    bLayout->addWidget(this->addItem, 0, 4);
    this->connect(this->addItem, &QPushButton::clicked,
                  this,          &EventNodes::slot_respond2Additem);
    bLayout->addWidget(this->removeItem, 0, 5);
    this->connect(this->removeItem, &QPushButton::clicked,
                  this,             &EventNodes::slot_respond2Removeitem);
    bLayout->addWidget(this->apply, 0, 8);
    this->connect(this->apply,  &QPushButton::clicked,
                  this,         &EventNodes::slot_4Apply);

    bLayout->addWidget(eventTable, 1, 0, 10, 4);
    this->eventTable->setModel(this->eventModel);
    this->eventTable->setSelectionMode(QAbstractItemView::SingleSelection);
    this->eventTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    auto model = this->eventTable->selectionModel();
    this->connect(model,&QItemSelectionModel::selectionChanged,
                  this, &EventNodes::slot_targetItemChanged);




    bLayout->addWidget(this->tabCon, 1, 4, 10, 5);

    auto panel_1 = new QWidget(this);
    this->tabCon->addTab(panel_1, "基本信息");
    auto panel_1_layout(new QGridLayout);
    panel_1->setLayout(panel_1_layout);
    panel_1_layout->setRowMinimumHeight(0, 20);
    panel_1_layout->setRowStretch(0,0);
    panel_1_layout->setRowMinimumHeight(1, 20);
    panel_1_layout->setRowStretch(1,0);
    panel_1_layout->setRowMinimumHeight(2, 20);
    panel_1_layout->setRowStretch(2,0);
    panel_1_layout->setRowMinimumHeight(3, 20);
    panel_1_layout->setRowStretch(3,0);

    panel_1_layout->addWidget(new QLabel(tr("名称：")));
    panel_1_layout->addWidget(this->evNameInput, 0, 1, 1, 2);
    this->connect(this->evNameInput,    &QLineEdit::textChanged,
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
    panel_1_layout->addWidget(time, 1, 0, 3, 3);

    auto comments(new QGroupBox(tr("事件备注")));
    auto __layout(new QHBoxLayout);
    comments->setLayout(__layout);
    __layout->addWidget(this->evNodeComment);
    panel_1_layout->addWidget(comments, 0, 3, 4, 2);
    this->connect(this->evNodeComment,  &QTextEdit::textChanged,
                  this,                 &EventNodes::slot_stateChanged);

    auto evndesc(new QGroupBox(tr("事件阶段描述")));
    auto _layout(new QGridLayout);
    evndesc->setLayout(_layout);
    _layout->addWidget(this->evNodeDesc);
    panel_1_layout->addWidget(evndesc, 4, 0, 4, 5);
    this->connect(this->evNodeDesc, &QTextEdit::textChanged,
                  this,             &EventNodes::slot_stateChanged);


    auto panel_2 = new QWidget(this);
    this->tabCon->addTab(panel_2, "地域性影响");
    auto panel_2_layout(new QGridLayout);
    panel_2->setLayout(panel_2_layout);

    panel_2_layout->addWidget(this->localTable, 0, 0, 9, 3);
    this->localTable->setModel(this->locals_Model);
    auto smodel = this->localTable->selectionModel();
    this->connect(smodel,   &QItemSelectionModel::currentRowChanged,
                  this,     &EventNodes::slot_TargetLocationChanged);
    panel_2_layout->addWidget(new QLabel(tr("人文描述")), 0, 3, 1, 2);
    panel_2_layout->addWidget(this->socialDesc, 1, 3, 4, 2);
    panel_2_layout->addWidget(new QLabel(tr("地貌描述")), 5, 3, 1, 2);
    panel_2_layout->addWidget(this->physicalDesc, 6, 3, 4, 2);
    panel_2_layout->addWidget(this->addLocation, 9, 0);
    this->connect(this->addLocation,    &QPushButton::clicked,
                  this,                 &EventNodes::slot_Respond2LocationAdd);
    panel_2_layout->addWidget(this->removeLocation, 9, 1);
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
    if(this->eventModel->rowCount(QModelIndex()) == 0){
        eName = this->input->text();
    }else{
        auto index = this->eventTable->currentIndex();
        if(!index.isValid())
            return;
        auto eventName = this->eventModel->data(index.sibling(index.row(), 0), Qt::DisplayRole);
        if(!eventName.isValid())
            return;
        if(eventName.isNull())
            return;

        eName = eventName.toString();
    }

    QSqlQuery q;
    q.prepare("insert into table_eventnodebasic "
              "(node_name, event_name, begin_time, end_time, node_desc, comment) "
              "values('新节点', :eName, 0, 1, '待输入', '无备注');");
    q.bindValue(":eName", eName);

    if(!q.exec()){
        qDebug() << q.lastError();
    }
    this->input->setText("");
    this->input->setText(eName);
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
    this->input->setText("");
    this->input->setText(name.toString());

    this->removeItem->setEnabled(false);
}

void EventNodes::slot_targetItemChanged(const QItemSelection &, const QItemSelection &)
{
    auto index = this->eventTable->currentIndex();
    if(!index.isValid())
        return;
    auto evtid = this->eventModel->oppositeID(index);

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

    this->evNodeDesc->setText(q.value(2).toString());
    this->beginStatus->resetDate(q.value(0).toLongLong());
    this->evNameInput->setText(q.value(3).toString());
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
              "tll.story, "
              "tll.location_id "
              "from table_locationchange tlc "
              "inner join table_locationlist tll on tlc.location = tll.location_id "
              "where tlc.event_node = :enode;");
    q.bindValue(":enode", evtid);
    if(!q.exec()){
        qDebug() << q.lastError();
        return;
    }

    this->locals_Model->clear();
    this->local_List_ids.clear();
    while (q.next()) {
        QList<QStandardItem*> row;
        for(int i=0; i<3; ++i){
            auto item(new QStandardItem(q.value(i).toString()));
            row.append(item);
        }
        this->locals_Model->appendRow(row);
        this->local_List_ids.append(q.value(3));
    }
    QStringList headers;
    headers <<"前缀" <<"名称" <<"历史渊源";
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
    auto index = this->eventTable->currentIndex();
    if(!index.isValid())
        return;
    auto id = this->eventModel->oppositeID(index);

    auto nodeName = this->evNameInput->text();
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
    auto evname = this->eventModel->data(index.sibling(index.row(), 0), Qt::DisplayRole);
    this->input->setText("");
    this->input->setText(evname.toString());
}

void EventNodes::slot_Respond2LocationAdd()
{
    auto index = this->eventTable->currentIndex();
    if(!index.isValid())
        return;
    auto evtid = this->eventModel->oppositeID(index);

    auto list = Editor::Location::getSelectedItemsID();
    //获取新增location
    QVariantList locates,evts;
    foreach(auto loc, list){
        if(!this->local_List_ids.contains(loc)){
            locates << loc;
            evts << evtid;
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
              "where (location = :loc);");
    q.bindValue(":loc",locid);
    if(!q.exec())
        qDebug() << q.lastError();
    this->slot_targetItemChanged(QItemSelection(),QItemSelection());
}

void EventNodes::slot_TargetLocationChanged(const QModelIndex &, const QModelIndex &)
{
    auto index = this->localTable->currentIndex();
    if(!index.isValid())
        return;
    auto locid = this->local_List_ids.at(index.row());

    auto index2 = this->eventTable->currentIndex();
    if(!index2.isValid())
        return;
    auto entid = this->eventModel->oppositeID(index2);

    QSqlQuery q;
    q.prepare("select "
              "end_time "
              "from table_eventnodebasic "
              "where ev_node_id=:eid;");
    q.bindValue(":eid",entid);
    if(!q.exec()){
        qDebug() << q.lastError();
        return;
    }
    if(!q.next())
        return;
    auto timepoint = q.value(0);

    q.prepare("select "
              "location_desc, "
              "social_desc "
              "from table_locationchange tlc "
              "inner join table_eventnodebasic tenb on tlc.event_node = tenb.ev_node_id "
              "where (tenb.end_time <= :time)and(tlc.location = :loc) "
              "group by tlc.location "
              "having tenb.end_time = Max(tenb.end_time);");
    q.bindValue(":time", timepoint);
    q.bindValue(":loc",locid);
    if(!q.exec()){
        qDebug() << q.lastError();
        return;
    }
    if(q.next()){
        this->physicalDesc->setText(q.value(0).toString());
        this->socialDesc->setText(q.value(1).toString());
    }
}








































