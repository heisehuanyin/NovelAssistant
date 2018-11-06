#include "components.h"

#include <QGridLayout>
#include <QLabel>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include <iostream>

UIComp::GTME::GTME(QWidget *parent):
    QDialog (parent),
    groups(new QComboBox),
    types(new QComboBox),
    items(new QListView),
    itemsmodel(new QStandardItemModel),
    up(new QPushButton(tr("上移"))),
    down(new QPushButton(tr("下移"))),
    append(new QPushButton(tr("增加"))),
    remove(new QPushButton(tr("删除"))),
    apply(new QPushButton(tr("应用"))),
    insertType(new QPushButton(tr("添加类别")))
{
    this->setWindowTitle("领域、类别、等级编辑");
    this->items->setSelectionMode(QAbstractItemView::SingleSelection);
    auto x(new QGridLayout);
    this->setLayout(x);

    x->addWidget(new QLabel(tr("领域")),0,0);
    x->addWidget(this->groups, 0, 1, 1, 2);
    this->groups->addItem("道具级别");
    this->groups->addItem("技能级别");
    this->groups->addItem("境界级别");
    this->groups->addItem("<手动选择>");
    this->groups->setCurrentIndex(3);
    this->connect(this->groups, &QComboBox::currentTextChanged,
                  this,         &GTME::slot_groupsSelected);
    x->addWidget(new QLabel(tr("类别")), 0, 3);
    x->addWidget(this->types, 0, 4, 1, 2);
    this->connect(this->types,  &QComboBox::currentTextChanged,
                  this,         &GTME::slot_typesSelected);

    x->addWidget(this->insertType, 0, 6);
    this->insertType->setEnabled(false);
    this->connect(this->insertType, &QPushButton::clicked,
                  this, &GTME::slot_insertNewType);
    x->addWidget(this->items, 1, 0, 8, 6);
    this->items->setModel(this->itemsmodel);
    x->addWidget(this->up, 1, 6);
    this->connect(up, &QPushButton::clicked,
                  this,&GTME::slot_itemUp);
    x->addWidget(this->down,2,6);
    this->connect(down,&QPushButton::clicked,
                  this,&GTME::slot_itemDown);
    x->addWidget(this->append,3,6);
    this->connect(append,&QPushButton::clicked,
                  this,&GTME::slot_itemAppend);
    x->addWidget(this->remove,4,6);
    this->connect(remove,&QPushButton::clicked,
                  this,&GTME::slot_itemRemove);
    x->addWidget(this->apply, 8, 6);
    this->connect(this->apply, &QPushButton::clicked,
                  this,&GTME::slot_itemApply);

    this->up->setEnabled(false);
    this->down->setEnabled(false);
    this->remove->setEnabled(false);
    this->append->setEnabled(false);
    this->apply->setEnabled(false);

    auto s_model = this->items->selectionModel();
    this->connect(s_model, &QItemSelectionModel::selectionChanged,
                  this,    &GTME::slot_btnEnableRelyOnSelect);
    this->connect(this->itemsmodel, &QStandardItemModel::itemChanged,
                  this,             &GTME::slot_btnApplyRespond);
}

UIComp::GTME::~GTME()
{
    delete this->groups;
    delete this->types;
    delete this->items;
    delete itemsmodel;
    delete up;
    delete down;
    delete append;
    delete apply;
    delete remove;
    delete insertType;
}

void UIComp::GTME::slot_groupsSelected(const QString &text)
{
    if(text == "<手动选择>"){
        this->types->clear();
        return;
    }
    this->types->clear();
    this->itemsmodel->clear();
    QString exeStr = "select distinct type_name "
                     "from table_gtm "
                     "where group_name = \"" + text +"\" "
                     "order by mark_number;";
    QSqlQuery q;
    if(!q.exec(exeStr)){
        QMessageBox::critical(this, "ERROR", "SQL语句执行错误1");
        return;
    }
    while (q.next()) {
        auto item = q.value(0).toString();
        this->types->addItem(item);
    }
    this->types->addItem("<编辑&添加>");
    if(this->types->count() == 1){
        this->types->setEditable(true);
        this->types->setInsertPolicy(QComboBox::InsertAtBottom);
    }
}

void UIComp::GTME::slot_typesSelected(const QString &text)
{
    this->itemsmodel->clear();
    if(text == "<编辑&添加>"){
        this->types->setEditable(true);
        this->types->setInsertPolicy(QComboBox::InsertAtBottom);
        return;
    }
    if(text == QString())
        return;
    QString exeStr = "select mark_name, "
                     "mark_id "
                     "from table_gtm "
                     "where (group_name = \""+this->groups->currentText()+"\")"
                     "  and (type_name  = \""+text+"\") "
                     "order by mark_number;";
    QSqlQuery q;
    if(!q.exec(exeStr)){
        QMessageBox::critical(this, "ERROR", "SQL语句执行错误2");
        return;
    }
    this->list.clear();
    while (q.next()) {
        auto item = q.value(0).toString();
        auto xv = new QStandardItem(item);
        xv->setEditable(true);
        this->itemsmodel->appendRow(xv);
        this->list.append(q.value(1));
    }
    this->apply->setEnabled(false);
    if(this->itemsmodel->rowCount()==0){
        this->insertType->setEnabled(true);
        this->up->setEnabled(false);
        this->down->setEnabled(false);
        this->remove->setEnabled(false);
        this->append->setEnabled(false);
    }
    else{
        this->insertType->setEnabled(false);
    }
}

void UIComp::GTME::slot_insertNewType()
{
    QString exeStr  = "insert into table_gtm "
                      "(group_name,type_name,mark_number,mark_name) "
                      "values(\""+this->groups->currentText()+"\","
                             "\""+this->types->currentText()+"\","
                             "0, \"Items-0\");";
    QSqlQuery q;
    if(!q.exec(exeStr)){
        QMessageBox::critical(this, "ERROR", "SQL语句执行错误3");
        return;
    }
    this->slot_typesSelected(this->types->currentText());
    this->apply->setEnabled(true);
}

void UIComp::GTME::slot_itemUp()
{
    auto x = this->items->currentIndex();
    auto target = itemsmodel->takeRow(x.row());
    this->itemsmodel->insertRow(x.row()-1,target);
    this->apply->setEnabled(true);
}

void UIComp::GTME::slot_itemDown()
{
    auto x = this->items->currentIndex();
    auto target = itemsmodel->takeRow(x.row());
    this->itemsmodel->insertRow(x.row()+1,target);
    this->apply->setEnabled(true);
}

void UIComp::GTME::slot_itemAppend()
{
    auto xv = new QStandardItem("双击修改");
    xv->setEditable(true);
    this->itemsmodel->appendRow(xv);
    this->apply->setEnabled(true);
}

void UIComp::GTME::slot_itemRemove()
{
    auto x = this->items->currentIndex();
    if(this->itemsmodel->rowCount() == 1)
        this->itemsmodel->clear();
    else
        this->itemsmodel->removeRow(x.row());
    this->apply->setEnabled(true);
}

void UIComp::GTME::slot_itemApply()
{
    auto groupName = this->groups->currentText();
    auto typeName = this->types->currentText();

    QSqlQuery q;
    int listLen = this->list.length();
    int itemCount=this->itemsmodel->rowCount();

    if( listLen > itemCount){
        q.prepare("update table_gtm set "
                  "type_name = '被清除', "
                  "mark_name = '请重新编辑' "
                  "where mark_id = ?;");
        QVariantList ids;
        for(int i=itemCount; i< listLen; ++i){
            ids << this->list.takeAt(i);
        }
        q.addBindValue(ids);
        if(!q.execBatch())
            qDebug() << q.lastError();
    }else if(listLen < itemCount){
        int appendnum = itemCount - listLen;
        q.prepare("insert into table_gtm "
                  "(group_name, type_name, mark_number, mark_name)"
                  "values(?, ?, -1, ?);");
        QVariantList gname,tname,kname;
        for(int i=0; i<appendnum; ++i){
            gname << groupName;
            tname << typeName;
            kname << "Please Enter!";
        }
        q.addBindValue(gname);
        q.addBindValue(tname);
        q.addBindValue(kname);

        if(!q.execBatch())
            qDebug() << q.lastError();

        q.prepare("select "
                  "mark_id "
                  "from table_gtm "
                  "where (group_name = :gname) "
                  "and   (type_name = :tname) "
                  "and   (mark_number = -1);");
        q.bindValue(":gname", groupName);
        q.bindValue(":tname", typeName);
        if(!q.exec())
            qDebug() << q.lastError();
        while (q.next()) {
            this->list.append(q.value(0));
        }
    }

    q.prepare("update table_gtm set "
              "mark_number = ?, "
              "mark_name = ?  "
              "where mark_id = ?;");
    QVariantList marknum, markname, ids;
    for(int i=0; i<this->itemsmodel->rowCount(); ++i){
        auto item = this->itemsmodel->item(i);
        marknum << i;
        markname<< item->text();
        ids << this->list.at(i);
    }
    q.addBindValue(marknum);
    q.addBindValue(markname);
    q.addBindValue(ids);

    if(!q.execBatch()){
        qDebug() << q.lastError();
    }
    this->apply->setEnabled(false);
}

void UIComp::GTME::slot_btnEnableRelyOnSelect(const QItemSelection &selected, const QItemSelection &deselected)
{
    auto index = selected.indexes().at(0);
    this->up->setEnabled(true);
    this->down->setEnabled(true);
    this->append->setEnabled(true);
    this->remove->setEnabled(true);

    if(index.row() == 0){
        this->up->setEnabled(false);
    }
    if(index.row() == this->itemsmodel->rowCount()-1){
        this->down->setEnabled(false);
    }
}

void UIComp::GTME::slot_btnApplyRespond(QStandardItem *)
{
    this->apply->setEnabled(true);
}





























