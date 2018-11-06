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
    input(new QLineEdit ),
    table(new QTableView ),
    tableModel(new QSqlQueryModel(this)),
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

    grid->addWidget(this->input, 0, 0, 1, 3);
    this->connect(this->input, &QLineEdit::textChanged,
                  this,        &SkillEdit::slot_queryProps);
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
    //x->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
    grid->addWidget(x, 1, 3);
    grid->addWidget(this->level, 1, 4, 1, 2);
    this->connect(this->level, &QComboBox::currentTextChanged,
                  this,        &SkillEdit::slot_statusChanged);
    grid->addWidget(this->levelEdit, 2, 5);
    this->connect(this->levelEdit, &QPushButton::clicked,
                  this,            &SkillEdit::slot_levelEdit);
    grid->addWidget(this->descLine, 2, 3, 1, 2);
    x = new QLabel(tr("数值:"));
    //x->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
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
}

SkillEdit::~SkillEdit()
{

}

void SkillEdit::slot_queryProps(const QString &text)
{
    this->tableModel->clear();
    this->addItem->setEnabled(false);
    this->removeItem->setEnabled(false);
    this->apply->setEnabled(false);
    if(text == QString())
        return;
    QString execStr = "select "
                      "g.type_name, "
                      "s.name, "
                      "s.skill_id "
                      "from table_skilllist s inner join table_gtm g "
                      "on s.mark = g.mark_id ";
    if(text != "*"){
        execStr += "where s.name like '%:name%' ";
        execStr.replace(":name", text);
    }
    execStr += "order by g.type_name;";

    this->tableModel->setQuery(execStr);
    if (tableModel->lastError().isValid())
        qDebug() << tableModel->lastError();
    this->tableModel->setHeaderData(0,Qt::Horizontal, "类别");
    this->tableModel->setHeaderData(1,Qt::Horizontal, "名称");
    this->tableModel->setHeaderData(2,Qt::Horizontal, "Innr_ID");

    if(this->tableModel->rowCount() == 0){
        this->addItem->setEnabled(true);
    }else{
        this->addItem->setEnabled(false);
    }
}

void SkillEdit::slot_addItem()
{
    QSqlQuery q;
    q.exec("select "
           "mark_id "
           "from table_gtm "
           "where (type_name = '被清除') "
           "and   (mark_name = '请重新编辑');");
    QVariant id;
    if(q.next()){
        id = q.value(0);
    }else{
        q.exec("insert into table_gtm "
               "(group_name, type_name, mark_number, mark_name)"
               "values('技能等级','被清除', 0, '请重新编辑');");
        q.exec("select "
               "mark_id "
               "from table_gtm "
               "where (type_name = '被清除') "
               "and   (mark_name = '请重新编辑');");
        q.next();
        id = q.value(0);
    }


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

void SkillEdit::slot_responseItemSelection(const QItemSelection &, const QItemSelection &)
{
    auto index = this->table->currentIndex();
    if(!index.isValid())
        return;
    auto id = this->tableModel->data(index.sibling(index.row(), 2)).toString();
    QSqlQuery q;
    q.prepare("select "
              "skill_desc,"
              "mark, "
              "number "
              "from table_skilllist "
              "where skill_id = :id;");
    q.bindValue(":id", id);
    q.exec();

    q.next();
    auto number = q.value(2).toString();
    this->value->setText(number);
    auto skill_desc = q.value(0).toString();
    this->descBlock->setText(skill_desc);
    auto mark = q.value(1);
    QString sqlStr = "select "
                     "mark_id, "
                     "group_name, "
                     "type_name, "
                     "mark_number,"
                     "mark_name "
                     "from table_gtm "
                     "where group_name = '技能级别' "
                     "order by type_name;";

    if(!q.exec(sqlStr))
        qDebug() << q.lastError();
    this->level->clear();
    while (q.next()) {
        auto id = q.value(0).toLongLong();
        auto item = "(" + QString("%1").arg(q.value(3).toLongLong())
                   +")" + q.value(2).toString() + ":"
                   + q.value(4).toString();
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
              "mark_number,"
              "mark_name "
              "from table_gtm "
              "where mark_id = :id;");
    q.bindValue(":id",mark);
    if(!q.exec()){
        qDebug() << q.lastError();
    }
    q.next();
    this->descLine->setText("::技能级别>>"
                            "[" + q.value(1).toString()+ "]"
                            + q.value(0).toString() + ":"
                            + q.value(2).toString());

    this->apply->setEnabled(false);
}

void SkillEdit::slot_responseApply()
{
    auto index = this->table->currentIndex();
    if(!index.isValid())
        return;
    auto id = this->tableModel->data(index.sibling(index.row(), 2)).toString();
    QString exeStr = "update table_skilllist "
                     "set "
                     "mark = :mark, "
                     "number = :number,"
                     "skill_desc = :desc "
                     "where skill_id = :id ;";
    QSqlQuery q;
    q.prepare(exeStr);
    q.bindValue(":id", id.toLongLong());
    q.bindValue(":mark", this->level->currentData());
    q.bindValue(":number", this->value->text().toLongLong());
    q.bindValue(":desc", this->descBlock->toPlainText());

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
    auto content = this->level->currentText();
    content.replace("(","::技能级别>>[").replace(")","]");
    this->descLine->setText(content);
}

void SkillEdit::slot_levelEdit()
{
    UIComp::GTME x(this);
    x.exec();

    this->slot_responseItemSelection(QItemSelection(), QItemSelection());
}































