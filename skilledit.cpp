#include "skilledit.h"

#include <QGroupBox>
#include <QSqlQuery>
#include <qgridlayout.h>
#include <QSqlError>
#include <QDebug>
#include <QMessageBox>
#include "components.h"

using namespace UIComp;

SkillEdit::SkillEdit(QWidget * parent):
    QDialog (parent),
    typeLimit(new QComboBox),
    input(new QLineEdit ),
    table(new QTableView ),
    tableModel(new Support::HiddenIdModel(this)),
    addItem(new QPushButton(tr("添加技能"))),
    removeItem(new QPushButton(tr("删除技能"))),
    apply(new QPushButton(tr("应用修改"))),
    level(new QComboBox),
    levelEdit(new QPushButton(tr("编辑等级"))),
    descLine(new QLabel(tr("::详细解释"))),
    value(new QLineEdit),
    descBlock(new QTextEdit)
{
    this->setWindowTitle("技能编辑");
    auto grid(new QGridLayout);
    this->setLayout(grid);

    grid->addWidget(this->typeLimit);
    this->connect(this->typeLimit, &QComboBox::currentTextChanged,
                  this,            &SkillEdit::slot_clearStatus);
    grid->addWidget(this->input, 0, 1, 1, 2);
    this->connect(this->input, &QLineEdit::textChanged,
                  this,        &SkillEdit::slot_querySkills);
    grid->addWidget(this->addItem, 0, 3);
    this->connect(this->addItem,&QPushButton::clicked,
                  this,        &SkillEdit::slot_addItem);
    grid->addWidget(this->removeItem, 0, 4);
    grid->addWidget(this->apply, 0, 5);
    this->connect(this->apply, &QPushButton::clicked,
                  this,        &SkillEdit::slot_responseApply);
    this->addItem->setEnabled(false);
    this->removeItem->setEnabled(false);
    this->apply->setEnabled(false);

    grid->addWidget(this->table, 1, 0, 10, 3);
    this->table->setModel(this->tableModel);
    this->table->setSelectionMode(QAbstractItemView::SingleSelection);
    this->table->setSelectionBehavior(QAbstractItemView::SelectRows);
    auto smodel = this->table->selectionModel();
    this->connect(smodel, &QItemSelectionModel::selectionChanged,
                  this,   &SkillEdit::slot_responseItemSelection);
    auto x = new QLabel(tr("等级:"));
    grid->addWidget(x, 1, 3);
    grid->addWidget(this->level, 1, 4);
    this->connect(this->level, &QComboBox::currentTextChanged,
                  this,        &SkillEdit::slot_statusChanged);
    grid->addWidget(this->levelEdit, 1, 5);
    this->connect(this->levelEdit, &QPushButton::clicked,
                  this,            &SkillEdit::slot_levelEdit);
    grid->addWidget(this->descLine, 2, 3, 1, 2);
    x = new QLabel(tr("数值:"));
    grid->addWidget(x, 3, 3);
    grid->addWidget(this->value, 3, 4, 1, 2);
    this->connect(this->value, &QLineEdit::textEdited,
                  this,        &SkillEdit::slot_statusChanged);
    this->connect(this->descBlock, &QTextEdit::textChanged,
                  this,            &SkillEdit::slot_statusChanged);

    auto group(new QGroupBox(tr("技能描述")));
    grid->addWidget(group, 4, 3, 7, 3);
    auto grid2(new QGridLayout);
    grid2->addWidget(this->descBlock);
    group->setLayout(grid2);

    QSqlQuery q;
    q.exec("select "
           "distinct type_name "
           "from table_gtm "
           "where group_name = '技能级别';");
    while (q.next()) {
        this->typeLimit->addItem(q.value(0).toString());
    }
}

SkillEdit::~SkillEdit()
{

}

QList<QVariant> SkillEdit::getSelectedItems()
{
    auto instance(new SkillEdit);
    instance->tableModel->changeCheckable(true);
    instance->exec();
    return instance->tableModel->selectedRecordIDs();
}

void SkillEdit::slot_querySkills(const QString &text)
{
    this->tableModel->clear();
    this->addItem->setEnabled(false);
    this->removeItem->setEnabled(false);
    this->apply->setEnabled(false);
    if(text == QString())
        return;

    QString typeStr = this->typeLimit->currentText();
    if(typeStr == QString()){
        QMessageBox::critical(this,"TYPE ERROR","数据库中没有“技能级别-分类”定义！");
        return;
    }
    QString execStr = "select "
                      "st.skill_id, "
                      "st.name, "
                      "gt.mark_name "
                      "from table_skilllist st inner join table_gtm gt "
                      "on st.mark=gt.mark_id "
                      "where "
                      "(st.mark in (select "
                      "mark_id "
                      "from table_gtm "
                      "where (group_name='技能级别') "
                      "and   (type_name =':tname'))"
                      ")";
    if(text != "*"){
        execStr += "and (st.name like '%:name%') ";
        execStr.replace(":name", text);
    }
    execStr.replace(":tname", typeStr);
    execStr += "order by gt.mark_number;";

    this->tableModel->setQuery(3, execStr);
    this->tableModel->setHorizontalHeader(0, "名称");
    this->tableModel->setHorizontalHeader(1, "等级");

    if(this->tableModel->rowCount(QModelIndex()) == 0){
        this->addItem->setEnabled(true);
    }else{
        this->addItem->setEnabled(false);
    }
}

void SkillEdit::slot_addItem()
{
    QSqlQuery q;
    q.exec("select mark_id "
           "from table_gtm "
           "where (group_name = '技能级别') "
           "group by type_name "
           "having (mark_number=min(mark_number))"
           "and    (type_name  ='"+this->typeLimit->currentText()+"');");
    QVariant id;
    if(!q.next())
        return;
    id = q.value(0);

    q.prepare( "insert into table_skilllist "
               "(name, skill_desc, mark, number) "
               "values(:name, '待输入', :mark, 0);");
    q.bindValue(":name", this->input->text());
    q.bindValue(":mark", id);
    q.exec();
    auto xname = this->input->text();
    this->input->setText("");
    this->input->setText(xname);
}

void SkillEdit::slot_removeItem()
{
    auto index = this->table->currentIndex();
    if(!index.isValid())
        return;

    auto id = this->tableModel->oppositeID(index);
    QString exec = "delete "
                   "from table_skilllist "
                   "where skill_id = :id;";
    QSqlQuery q;
    q.prepare(exec);
    q.bindValue(":id", id);
    if(!q.exec())
        qDebug() << q.lastError();
    this->removeItem->setEnabled(false);
    m
}

void SkillEdit::slot_clearStatus()
{
    this->tableModel->clear();
    this->input->clear();
    this->level->clear();
    this->descLine->clear();
    this->value->clear();
    this->descBlock->clear();
    this->apply->setEnabled(false);
}

void SkillEdit::slot_responseItemSelection(const QItemSelection &, const QItemSelection &)
{
    auto index = this->table->currentIndex();
    if(!index.isValid())
        return;

    auto id = this->tableModel->oppositeID(index);

    QSqlQuery q;
    q.prepare("select "
              "skill_desc, "
              "mark, "
              "number "
              "from table_skilllist "
              "where skill_id = :id;");
    q.bindValue(":id", id);
    if(!q.exec()){
        qDebug() << q.lastError();
        return;
    }

    if(!q.next()){
        QMessageBox::critical(this, "ERROR", "空集");
        return;
    }
    auto number = q.value(2).toString();
    this->value->setText(number);
    auto skill_desc = q.value(0).toString();
    this->descBlock->setText(skill_desc);
    auto mark = q.value(1);
    q.prepare("select "
              "mark_id, "
              "mark_number "
              "from table_gtm "
              "where (group_name = '技能级别') "
              "and   (type_name  = :tname)"
              "order by mark_number;");
    q.bindValue(":tname", this->typeLimit->currentText());

    if(!q.exec()){
        qDebug() << q.lastError();
        return;
    }
    this->level->clear();
    while (q.next()) {
        auto id = q.value(0);
        auto item = q.value(1).toString();
        this->level->addItem(item, id);
    }
    if((!mark.isNull()) || (mark.isValid())){
        auto id = mark.toLongLong();
        for(int i=0;i<this->level->count(); ++i){
            if(this->level->itemData(i).toLongLong() == id){
                this->level->setCurrentIndex(i);
                break;
            }
        }
    }
    q.prepare("select "
              "type_name, "
              "mark_name "
              "from table_gtm "
              "where mark_id = :id;");
    q.bindValue(":id",mark);
    if(!q.exec()){
        qDebug() << q.lastError();
    }
    if(q.next())
        this->descLine->setText("::技能级别>>"
                                + q.value(0).toString() + ":"
                                + q.value(1).toString());

    this->apply->setEnabled(false);
    this->removeItem->setEnabled(true);
}

void SkillEdit::slot_responseApply()
{
    auto index = this->table->currentIndex();
    if(!index.isValid())
        return;

    auto id = this->tableModel->oppositeID(index);

    QSqlQuery q;
    QString exeStr = "update table_skilllist "
                     "set "
                     "mark = :mark, "
                     "number = :number,"
                     "skill_desc = :desc "
                     "where skill_id = :id;";
    q.prepare(exeStr);
    q.bindValue(":mark", this->level->currentData());
    q.bindValue(":number", this->value->text().toLongLong());
    q.bindValue(":desc", this->descBlock->toPlainText());
    q.bindValue(":id", id);

    if(!q.exec())
        qDebug() << q.lastError();

    this->apply->setEnabled(false);
    auto xname = this->input->text();
    this->input->setText("");
    this->input->setText(xname);
}

void SkillEdit::slot_statusChanged()
{
    this->apply->setEnabled(true);
    auto id = this->level->currentData();
    if((!id.isValid()) || (id.isNull()))
        return;
    QSqlQuery q;
    q.prepare("select group_name, type_name, mark_name "
              "from table_gtm "
              "where mark_id = :id;");
    q.bindValue(":id",id);
    if(!q.exec())
        return;
    q.next();
    QString content = "::" + q.value(0).toString()
                    + ">>" + q.value(1).toString()
                    + ":"  + q.value(2).toString();
    this->descLine->setText(content);
}

void SkillEdit::slot_levelEdit()
{
    UIComp::GTME x(this);
    x.exec();

    this->slot_responseItemSelection(QItemSelection(), QItemSelection());
}
















