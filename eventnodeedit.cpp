#include "eventnodeedit.h"

#include <QGridLayout>
#include <QGroupBox>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>

using namespace UIComp;

EventnodeEdit::EventnodeEdit(QWidget *parent):
    QDialog(parent),
    input(new QLineEdit),
    addItem(new QPushButton(tr("添加"))),
    removeItem(new QPushButton(tr("删除"))),
    apply(new QPushButton(tr("应用变更"))),
    eventTable(new QTableView),
    eventModel(new Support::HiddenIdModel),
    birthDay(new QPushButton(tr("xxxxx年xx月xx日"))),
    deathDay(new QPushButton(tr("xxxxx年xx月xx日"))),
    birthStatus(new Support::SuperDateTool(this)),
    deathStatus(new Support::SuperDateTool(this)),
    evNodeDesc(new QTextEdit),
    effect(new QGroupBox(tr("区域性影响"))),
    localInput(new QLineEdit),
    localTable(new QTableView),
    localProps(new QTableView),
    pre_socialDesc(new QLabel(tr("前社会描述"))),
    socialDesc(new QTextEdit),
    pre_sufaceDesc(new QLabel(tr("前地貌描述"))),
    sufaceDesc(new QTextEdit)
{
    auto bLayout(new QGridLayout);
    this->setLayout(bLayout);

    bLayout->addWidget(this->input, 0, 0, 1, 2);
    this->connect(this->input, &QLineEdit::textChanged,
                  this,        &EventnodeEdit::slot_queryEventNode);
    bLayout->addWidget(this->addItem, 0, 2);
    this->connect(this->addItem, &QPushButton::clicked,
                  this,          &EventnodeEdit::slot_respond2Additem);
    bLayout->addWidget(this->removeItem, 0, 3);
    bLayout->addWidget(eventTable, 1, 0, 10, 4);
    this->eventTable->setModel(this->eventModel);
    this->eventTable->setSelectionMode(QAbstractItemView::SingleSelection);
    this->eventTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    auto model = this->eventTable->selectionModel();
    this->connect(model,&QItemSelectionModel::selectionChanged,
                  this, &EventnodeEdit::slot_targetItemChanged);

    auto time(new QGroupBox(tr("始末时间")));
    auto inb(new QGridLayout);
    time->setLayout(inb);
    inb->addWidget(new QLabel(tr("始于：")));
    inb->addWidget(this->birthDay, 0, 1, 1, 3);
    this->connect(this->birthDay, &QPushButton::clicked,
                  this,           &EventnodeEdit::slot_editBeginTime);
    inb->addWidget(this->deathDay, 1, 1, 1, 3);
    this->connect(this->deathDay, &QPushButton::clicked,
                  this,           &EventnodeEdit::slot_editEndTime);
    inb->addWidget(new QLabel(tr("末于：")), 1, 0);
    bLayout->addWidget(time, 1, 4, 3, 4);
    bLayout->setRowMinimumHeight(1, 20);
    bLayout->setRowStretch(1,0);
    bLayout->setRowMinimumHeight(2, 20);
    bLayout->setRowStretch(2,0);
    bLayout->setRowMinimumHeight(3, 20);
    bLayout->setRowStretch(3,0);

    auto evndesc(new QGroupBox(tr("事件阶段性描述")));
    auto _layout(new QHBoxLayout);
    evndesc->setLayout(_layout);
    _layout->addWidget(this->evNodeDesc);
    bLayout->addWidget(evndesc, 4, 4, 7, 4);

    effect->setCheckable(true);
    effect->setChecked(false);
    auto _2layout(new QGridLayout);
    effect->setLayout(_2layout);
    bLayout->addWidget(effect, 1, 8, 10, 5);
    bLayout->addWidget(this->apply, 0, 12);

    _2layout->addWidget(this->localInput, 0, 0, 1, 2);
    _2layout->addWidget(this->localTable, 1, 0, 10, 2);
    _2layout->addWidget(this->localProps, 0, 2, 5, 3);
    _2layout->addWidget(this->pre_socialDesc, 5, 2, 1, 3);
    _2layout->addWidget(this->socialDesc, 6, 2, 2, 3);
    _2layout->addWidget(this->pre_sufaceDesc, 8, 2, 1, 3);
    _2layout->addWidget(this->sufaceDesc, 9, 2, 2, 3);

    this->birthDay->setEnabled(false);
    this->deathDay->setEnabled(false);
    this->addItem->setEnabled(false);
    this->removeItem->setEnabled(false);
    this->apply->setEnabled(false);
}

EventnodeEdit::~EventnodeEdit(){}

void EventnodeEdit::slot_queryEventNode(const QString &text)
{
    this->addItem->setEnabled(false);
    this->removeItem->setEnabled(false);
    this->apply->setEnabled(false);
    if(text == QString())
        return;
    QString execStr = "select "
                      "ev_node_id, "
                      "event_name, "
                      "node_name, "
                      "node_desc "
                      "from table_eventnodeeffect ";
    if(text != "*"){
        execStr += "where event_name like '%"+text+"%' ";
    }
    execStr += "order by begin_time;";
    this->eventModel->setQuery(4, execStr, 0);
    this->eventModel->setHorizontalHeader(0, "事件名称");
    this->eventModel->setHorizontalHeader(1, "阶段名称");
    this->eventModel->setHorizontalHeader(2, "阶段简述");

    if(this->eventModel->rowCount(QModelIndex()) == 0){
        if(text != "*")
            this->addItem->setEnabled(true);
    }else{
        this->addItem->setEnabled(false);
    }
}

void EventnodeEdit::slot_respond2Additem()
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
    q.prepare("insert into table_eventnodeeffect "
              "(node_name, event_name, begin_time, end_time, node_desc) "
              "values(:nName, :eName, :begin, :end, :desc);");
    q.bindValue(":eName", eName);
    q.bindValue(":nName", "新节点");
    q.bindValue(":begin", 0);
    q.bindValue(":end", 1);
    q.bindValue(":desc", "待输入");

    if(!q.exec()){
        qDebug() << q.lastError();
    }
    this->input->setText("");
    this->input->setText(eName);
}

void EventnodeEdit::slot_targetItemChanged(const QItemSelection &, const QItemSelection &)
{
    auto index = this->eventTable->currentIndex();
    if(!index.isValid())
        return;
    auto idvar = this->eventModel->oppositeID(index);
    if((!idvar.isValid())||idvar.isNull()){
        return;
    }else{
        this->addItem->setEnabled(true);
        this->removeItem->setEnabled(true);
    }
    QSqlQuery q;
    q.prepare("select "
              "begin_time, "
              "end_time, "
              "node_desc "
              "from table_eventnodeeffect "
              "where ev_node_id = :id;");
    q.bindValue(":id", idvar);

    if(!q.exec()){
        qDebug() << q.lastError();
        return;
    }
    q.next();
    this->evNodeDesc->setText(q.value(2).toString());
    this->birthStatus->resetDate(q.value(0).toLongLong());
    auto str1 = birthStatus->toString();
    this->birthDay->setText(str1);
    this->deathStatus->resetDate(q.value(1).toLongLong());
    str1 = deathStatus->toString();
    this->deathDay->setText(str1);

    this->birthDay->setEnabled(true);
    this->deathDay->setEnabled(true);

    q.prepare("select "
              "");
}

void EventnodeEdit::slot_editBeginTime()
{
    auto value = this->birthStatus->dateEdit();
    if(value > this->deathStatus->toLongLong()){
        QMessageBox::critical(this, "date error", "起始日期不可晚于终末日期！");
        return;
    }
    this->birthStatus->resetDate(value);
    this->birthDay->setText(this->birthStatus->toString());
    this->apply->setEnabled(true);
}

void EventnodeEdit::slot_editEndTime()
{
    auto value = this->deathStatus->dateEdit();
    if(this->birthStatus->toLongLong() > value){
        QMessageBox::critical(this, "date error", "起始日期不可晚于终末日期！");
        return;
    }
    this->deathStatus->resetDate(value);
    this->deathDay->setText(this->deathStatus->toString());
    this->apply->setEnabled(true);
}








































