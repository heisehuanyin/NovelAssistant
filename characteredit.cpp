#include "characteredit.h"

#include <QGridLayout>
#include <QGroupBox>
#include <QSqlError>
#include <QDebug>
#include <QSqlQuery>

using namespace UIComp;

CharacterEdit::CharacterEdit(QWidget *const parent):
    QDialog (parent),
    input(new QLineEdit),
    table(new QTableView),
    tableModel(new QSqlQueryModel(this)),
    addItem(new QPushButton(tr("增加条目"))),
    removeItem(new QPushButton(tr("删除条目"))),
    apply(new QPushButton(tr("保存修改"))),
    birthDay(new QLineEdit),
    birthDesc(new QLabel(tr("生于："))),
    deathDay(new QLineEdit),
    deathDesc(new QLabel(tr("卒于："))),
    nicknames(new QListWidget),
    addNick(new QPushButton(tr("增加昵称"))),
    removeNick(new QPushButton(tr("移除昵称"))),
    storyDisplay(new QListWidget),
    storyListModel(new QSqlQueryModel(this)),
    editStory(new QPushButton(tr("履历编辑"))),
    comment(new QTextEdit)
{
    this->setWindowTitle("角色编辑");
    auto grid(new QGridLayout);
    this->setLayout(grid);

    grid->addWidget(this->input, 0, 0, 1, 3);
    this->connect(input, &QLineEdit::textChanged,
                  this,  &CharacterEdit::slot_queryCharacter);
    grid->addWidget(this->table, 1, 0, 13, 3);
    this->table->setModel(this->tableModel);
    auto smodel = this->table->selectionModel();
    this->connect(smodel, &QItemSelectionModel::selectionChanged,
                  this,   &CharacterEdit::slot_responseItemSelection);
    this->table->setSelectionMode(QAbstractItemView::SingleSelection);
    this->table->setSelectionBehavior(QAbstractItemView::SelectRows);
    grid->addWidget(this->addItem, 0, 3, 1, 2);
    this->connect(this->addItem, &QPushButton::clicked,
                  this,          &CharacterEdit::slot_addItem);
    grid->addWidget(this->removeItem, 0, 5, 1, 2);
    grid->addWidget(this->apply, 0, 9, 1, 2);
    grid->addWidget(this->editStory, 0, 7, 1, 2);

    auto group1(new QGroupBox(tr("昵称管理")));
    grid->addWidget(group1, 4, 3, 6, 4);
    auto grid1(new QGridLayout);
    group1->setLayout(grid1);
    grid1->addWidget(this->addNick, 1, 0);
    grid1->addWidget(this->removeNick, 1, 1);
    grid1->addWidget(this->nicknames, 0, 0, 1, 2);
    this->nicknames->setSelectionMode(QAbstractItemView::SingleSelection);

    auto group2(new QGroupBox(tr("生卒时间")));
    grid->addWidget(group2, 1, 3, 3, 4);
    auto grid2(new QGridLayout);
    group2->setLayout(grid2);
    grid2->addWidget(this->birthDesc);
    grid2->addWidget(this->birthDay, 0, 1, 1, 3);
    grid2->addWidget(this->deathDesc, 1, 0);
    grid2->addWidget(this->deathDay, 1, 1, 1, 3);

    auto group3(new QGroupBox(tr("阅历表")));
    grid->addWidget(group3, 1, 7, 13, 4);
    auto grid3(new QGridLayout);
    group3->setLayout(grid3);
    grid3->addWidget(this->storyDisplay,0,0,1,2);
    this->storyDisplay->setSelectionMode(QAbstractItemView::SingleSelection);

    auto group4(new QGroupBox(tr("备注")));
    grid->addWidget(group4, 10, 3, 4, 4);
    auto grid4(new QGridLayout);
    group4->setLayout(grid4);
    grid4->addWidget(this->comment);

}

CharacterEdit::~CharacterEdit()
{
    delete input;
    delete table;
    delete addItem;
    delete removeItem;
    delete apply;
    delete birthDay;
    delete birthDesc;
    delete deathDay;
    delete deathDesc;
    delete nicknames;
    delete addNick;
    delete removeNick;
    delete storyDisplay;
    delete editStory;
    delete comment;
}

void CharacterEdit::slot_queryCharacter(const QString &text)
{
    this->tableModel->clear();
    this->addItem->setEnabled(false);
    this->removeItem->setEnabled(false);
    this->apply->setEnabled(false);
    if(text == QString())
        return;
    QString execStr = "select "
                      "name "
                      "from table_characterbasic ";
    if(text != "*"){
        execStr += "where name like '%:name%' ";
        execStr.replace(":name", text);
    }
    execStr += "order by birthday;";

    this->tableModel->setQuery(execStr);
    if (tableModel->lastError().isValid())
        qDebug() << tableModel->lastError();
    this->tableModel->setHeaderData(0,Qt::Horizontal, "名称");

    if(this->tableModel->rowCount() == 0){
        this->addItem->setEnabled(true);
    }else{
        this->addItem->setEnabled(false);
    }
}

void CharacterEdit::slot_addItem()
{
    QSqlQuery q;
    q.prepare( "insert into table_characterbasic "
               "(name, nikename, birthday, deathday, comment) "
               "values(:name, '袋鼠', 0, 0, '没啥说的');");
    q.bindValue(":name",this->input->text());
    q.exec();
    auto xname = this->input->text();
    this->input->setText("");
    this->input->setText(xname);
}

void CharacterEdit::slot_responseItemSelection(const QItemSelection &, const QItemSelection &)
{
    auto index = this->table->currentIndex();
    if(!index.isValid())
        return;
    auto name = this->tableModel->data(index.sibling(index.row(), 0)).toString();
    QSqlQuery q;
    q.prepare("select "
              "nikename,"
              "birthday, "
              "deathday, "
              "comment "
              "from table_characterbasic "
              "where name = :name;");
    q.bindValue(":name", name);
    q.exec();

    q.next();
    auto nikes = q.value(0).toString().split(",");
    this->nicknames->clear();
    nikes.removeAll(QString());
    this->nicknames->addItems(nikes);

    this->birthDay->setText(QString("%1").arg(q.value(1).toLongLong()));
    this->deathDay->setText(QString("%1").arg(q.value(2).toLongLong()));
    this->comment->setText(q.value(3).toString());
}






























