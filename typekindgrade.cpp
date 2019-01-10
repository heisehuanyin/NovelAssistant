#include "typekindgrade.h"

#include <QGridLayout>
#include <QLabel>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include <iostream>

using namespace Editor;

TypeKindGrade::TypeKindGrade(QWidget *parent):
    QDialog (parent),
    groups(new QComboBox),
    types(new QComboBox),
    items(new QTableView),
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
                  this,         &TypeKindGrade::slot_groupsSelected);
    x->addWidget(new QLabel(tr("类别")), 0, 3);
    x->addWidget(this->types, 0, 4, 1, 2);
    this->connect(this->types,  &QComboBox::currentTextChanged,
                  this,         &TypeKindGrade::slot_typesSelected);

    x->addWidget(this->insertType, 0, 6);
    this->insertType->setEnabled(false);
    this->connect(this->insertType, &QPushButton::clicked,
                  this, &TypeKindGrade::slot_insertNewType);
    x->addWidget(this->items, 1, 0, 8, 6);
    this->items->setModel(this->itemsmodel);
    x->addWidget(this->up, 1, 6);
    this->connect(up, &QPushButton::clicked,
                  this,&TypeKindGrade::slot_itemUp);
    x->addWidget(this->down,2,6);
    this->connect(down,&QPushButton::clicked,
                  this,&TypeKindGrade::slot_itemDown);
    x->addWidget(this->append,3,6);
    this->connect(append,&QPushButton::clicked,
                  this,&TypeKindGrade::slot_itemAppend);
    x->addWidget(this->remove,4,6);
    this->connect(remove,&QPushButton::clicked,
                  this,&TypeKindGrade::slot_itemRemove);
    x->addWidget(this->apply, 8, 6);
    this->connect(this->apply, &QPushButton::clicked,
                  this,&TypeKindGrade::slot_itemApply);

    this->up->setEnabled(false);
    this->down->setEnabled(false);
    this->remove->setEnabled(false);
    this->append->setEnabled(false);
    this->apply->setEnabled(false);

    auto s_model = this->items->selectionModel();
    this->connect(s_model, &QItemSelectionModel::selectionChanged,
                  this,    &TypeKindGrade::slot_btnEnableRelyOnSelect);
    this->connect(this->itemsmodel, &QStandardItemModel::itemChanged,
                  this,             &TypeKindGrade::slot_btnApplyRespond);
}

TypeKindGrade::~TypeKindGrade()
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

void TypeKindGrade::slot_groupsSelected(const QString &text)
{
    this->types->setEditable(false);
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

void TypeKindGrade::slot_typesSelected(const QString &text)
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
                     "mark_id,"
                     "comment "
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
        auto rowExm = QList<QStandardItem*>();

        auto item = q.value(0).toString();
        auto u1 = new QStandardItem(item);
        u1->setEditable(true);
        rowExm.append(u1);

        item = q.value(2).toString();
        auto u2 = new QStandardItem(item);
        u2->setEditable(true);
        rowExm.append(u2);

        this->itemsmodel->appendRow(rowExm);

        this->list.append(q.value(1));
    }
    this->itemsmodel->setHeaderData(0,Qt::Horizontal,"名称");
    this->itemsmodel->setHeaderData(1,Qt::Horizontal,"备注");
    this->items->resizeColumnsToContents();
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

void TypeKindGrade::slot_insertNewType()
{
    QString exeStr  = "insert into table_gtm "
                      "(group_name,type_name,mark_number,mark_name,comment) "
                      "values(\""+this->groups->currentText()+"\","
                             "\""+this->types->currentText()+"\","
                             "0, \"双击修改名称\", \"双击修改备注\");";
    QSqlQuery q;
    if(!q.exec(exeStr)){
        QMessageBox::critical(this, "ERROR", "SQL语句执行错误3");
        return;
    }
    auto tempText = this->types->currentText();
    this->slot_groupsSelected(this->groups->currentText());
    if(this->types->currentText() != tempText)
        this->types->setCurrentText(tempText);
    else
        this->slot_typesSelected(tempText);
    this->apply->setEnabled(true);
    this->types->setEditable(false);
}

void TypeKindGrade::slot_itemUp()
{
    auto x = this->items->currentIndex();
    auto target = itemsmodel->takeRow(x.row());
    this->itemsmodel->insertRow(x.row()-1,target);
    this->apply->setEnabled(true);
}

void TypeKindGrade::slot_itemDown()
{
    auto x = this->items->currentIndex();
    auto target = itemsmodel->takeRow(x.row());
    this->itemsmodel->insertRow(x.row()+1,target);
    this->apply->setEnabled(true);
}

void TypeKindGrade::slot_itemAppend()
{
    auto xv1 = new QStandardItem("双击修改名称");
    xv1->setEditable(true);
    auto xv2 = new QStandardItem("双击修改备注");
    xv2->setEditable(true);
    auto row = QList<QStandardItem*>();
    row.append(xv1);
    row.append(xv2);

    this->itemsmodel->appendRow(row);
    this->apply->setEnabled(true);
}

void TypeKindGrade::slot_itemRemove()
{
    auto x = this->items->currentIndex();
    if(this->itemsmodel->rowCount() == 1)
        this->itemsmodel->clear();
    else
        this->itemsmodel->removeRow(x.row());
    this->apply->setEnabled(true);
}

void TypeKindGrade::slot_itemApply()
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
                  "(group_name, type_name, mark_number, mark_name, comment)"
                  "values(?, ?, -1, ?, ?);");
        QVariantList gname,tname,kname,cname;
        for(int i=0; i<appendnum; ++i){
            gname << groupName;
            tname << typeName;
            kname << "Please Enter!";
            cname << "Please Enter!";
        }
        q.addBindValue(gname);
        q.addBindValue(tname);
        q.addBindValue(kname);
        q.addBindValue(cname);

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

    q.prepare("update table_gtm "
              "set "
              "mark_number = ?, "
              "mark_name = ? ,"
              "comment = ? "
              "where mark_id = ?;");
    QVariantList marknum, markname, ids, comment;
    for(int i=0; i<this->itemsmodel->rowCount(); ++i){
        auto item1 = this->itemsmodel->item(i);
        marknum << i;
        markname<< item1->text();
        ids << this->list.at(i);
        comment << this->itemsmodel->item(i,1)->text();
    }
    q.addBindValue(marknum);
    q.addBindValue(markname);
    q.addBindValue(comment);
    q.addBindValue(ids);

    if(!q.execBatch()){
        qDebug() << q.lastError();
    }
    this->apply->setEnabled(false);
}

void TypeKindGrade::slot_btnEnableRelyOnSelect(const QItemSelection &selected, const QItemSelection &)
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

void TypeKindGrade::slot_btnApplyRespond(QStandardItem *)
{
    this->apply->setEnabled(true);
}





























