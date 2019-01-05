#include "characteredit.h"
#include "storyboard.h"

#include <QGridLayout>
#include <QGroupBox>
#include <QSqlError>
#include <QDebug>
#include <QSqlQuery>
#include <QMessageBox>

using namespace UIComp;

CharacterEdit::CharacterEdit(QWidget *const parent):
    QDialog (parent),
    input(new QLineEdit),
    table(new QTableView),
    tableModel(new Support::HiddenIdModel(this)),
    addItem(new QPushButton(tr("增加条目"))),
    removeItem(new QPushButton(tr("删除条目"))),
    apply(new QPushButton(tr("保存修改"))),
    birthDay(new QPushButton(tr("xxxx年xx月xx日"))),
    birthEdit(new Support::SuperDateTool(this)),
    deathDay(new QPushButton(tr("xxxx年xx月xx日"))),
    deathEdit(new Support::SuperDateTool(this)),
    nicknames(new QListWidget),
    addNick(new QPushButton(tr("增加昵称"))),
    removeNick(new QPushButton(tr("移除昵称"))),
    storyDisplay(new StoryDisplay(this)),
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
    this->connect(this->removeItem, &QPushButton::clicked,
                  this,             &CharacterEdit::slot_removeItem);
    grid->addWidget(this->editStory, 0, 7, 1, 2);
    this->connect(this->editStory,  &QPushButton::clicked,
                  this,             &CharacterEdit::slot_editCharcterStory);
    grid->addWidget(this->apply, 0, 9, 1, 2);
    this->connect(this->apply,  &QPushButton::clicked,
                  this,         &CharacterEdit::slot_4apply);

    auto group1(new QGroupBox(tr("昵称管理")));
    grid->addWidget(group1, 8, 3, 6, 4);
    auto grid1(new QGridLayout);
    group1->setLayout(grid1);
    grid1->addWidget(this->nicknames, 0, 0, 1, 2);
    this->nicknames->setSelectionMode(QAbstractItemView::SingleSelection);
    grid1->addWidget(this->addNick, 1, 0);
    this->connect(this->addNick, &QPushButton::clicked,
                  this,          &CharacterEdit::slot_addNickName);
    grid1->addWidget(this->removeNick, 1, 1);
    this->connect(this->removeNick, &QPushButton::clicked,
                  this,             &CharacterEdit::slot_removeNickName);

    auto group2(new QGroupBox(tr("生卒时间")));
    grid->addWidget(group2, 5, 3, 3, 4);
    auto grid2(new QGridLayout);
    group2->setLayout(grid2);
    grid2->addWidget(new QLabel(tr("生于：")));
    grid2->addWidget(this->birthDay, 0, 1, 1, 3);
    this->connect(this->birthDay,   &QPushButton::clicked,
                  this,             &CharacterEdit::slot_birthEdit);
    grid2->addWidget(new QLabel(tr("卒于：")), 1, 0);
    grid2->addWidget(this->deathDay, 1, 1, 1, 3);
    this->connect(this->deathDay,   &QPushButton::clicked,
                  this,             &CharacterEdit::slot_deathEdit);

    auto group3(new QGroupBox(tr("阅历表")));
    grid->addWidget(group3, 5, 7, 9, 4);
    auto grid3(new QGridLayout);
    group3->setLayout(grid3);
    grid3->addWidget(this->storyDisplay,0,0,1,2);

    auto group4(new QGroupBox(tr("人物备注")));
    grid->addWidget(group4, 1, 3, 4, 8);
    auto grid4(new QGridLayout);
    group4->setLayout(grid4);
    grid4->addWidget(this->comment);
    this->connect(this->comment,    &QTextEdit::textChanged,
                  this,             &CharacterEdit::slot_statusChanged);

}

CharacterEdit::~CharacterEdit()
{
    delete input;
    delete table;
    delete tableModel;
    delete addItem;
    delete removeItem;
    delete apply;
    delete birthDay;
    delete birthEdit;
    delete deathDay;
    delete deathEdit;
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
                      "char_id, "
                      "name, "
                      "nikename "
                      "from table_characterbasic ";
    if(text != "*"){
        execStr += "where name like '%:name%' ";
        execStr.replace(":name", text);
    }
    execStr += "order by birthday;";

    this->tableModel->setQuery(3, execStr);
    this->tableModel->setHorizontalHeader(0, "角色名称");
    this->tableModel->setHorizontalHeader(1, "昵称列表");

    if(this->tableModel->rowCount(QModelIndex()) == 0){
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

void CharacterEdit::slot_removeItem()
{
    auto index = this->table->currentIndex();
    if(!index.isValid())
        return;
    auto id = this->tableModel->oppositeID(index);
    QSqlQuery q;
    q.prepare("delete from table_characterbasic "
              "where char_id = :id");
    q.bindValue(":id", id);
    if(!q.exec())
        qDebug() << q.lastError();
    auto name = this->tableModel->data(index.sibling(index.row(), 0), Qt::DisplayRole);
    this->input->setText("");
    this->input->setText(name.toString());
    this->removeItem->setEnabled(false);
}

void CharacterEdit::slot_4apply()
{
    auto index = this->table->currentIndex();
    if(!index.isValid())
        return;
    auto id = this->tableModel->oppositeID(index);

    auto comment = this->comment->toPlainText();
    auto birthNum = this->birthEdit->toLongLong();
    auto deathNum = this->deathEdit->toLongLong();

    QString cstr;
    for(int i=0; i< this->nicknames->count(); ++i){
        cstr += this->nicknames->item(i)->text() + ",";
    }
    QSqlQuery q;
    q.prepare("update table_characterbasic "
              "set "
              "nikename = :nick, "
              "birthday = :bday, "
              "deathday = :dday, "
              "comment = :cmt "
              "where char_id = :id ;");
    q.bindValue(":nick", cstr);
    q.bindValue(":bday", birthNum);
    q.bindValue(":dday", deathNum);
    q.bindValue(":cmt", comment);
    q.bindValue(":id", id);
    if(!q.exec())
        qDebug() << q.lastError();

    auto name = this->tableModel->data(index.sibling(index.row(), 0), Qt::DisplayRole);
    this->input->setText("");
    this->input->setText(name.toString());
}

void CharacterEdit::slot_birthEdit()
{
    auto time_temp = this->birthEdit->dateEdit();
    if(time_temp > this->deathEdit->toLongLong()){
        QMessageBox::critical(this, "Date Error", "出生日期不能大于死亡日期！");
        return;
    }
    this->birthEdit->resetDate(time_temp);
    auto dateStr = birthEdit->toString();
    this->birthDay->setText(dateStr);
    this->apply->setEnabled(true);
    this->slot_statusChanged();
}

void CharacterEdit::slot_deathEdit()
{
    auto time_temp = this->deathEdit->dateEdit();
    if(this->birthEdit->toLongLong() > time_temp){
        QMessageBox::critical(this, "Date Error", "出生日期不能大于死亡日期！");
        return;
    }
    this->deathEdit->resetDate(time_temp);
    auto dataStr = deathEdit->toString();
    this->deathDay->setText(dataStr);
    this->apply->setEnabled(true);
    this->slot_statusChanged();
}

void CharacterEdit::slot_addNickName()
{
    this->nicknames->addItem("双击修改");
    auto item = this->nicknames->item(nicknames->count()-1);
    item->setFlags(item->flags()|Qt::ItemIsEditable);
    this->slot_statusChanged();
}

void CharacterEdit::slot_removeNickName()
{
    auto row = this->nicknames->currentRow();

    auto x = this->nicknames->takeItem(row);
    if(!x)
        return;
    delete x;
    this->slot_statusChanged();
}

void CharacterEdit::slot_responseItemSelection(const QItemSelection &, const QItemSelection &)
{
    auto index = this->table->currentIndex();
    if(!index.isValid())
        return;
    auto id = this->tableModel->oppositeID(index);

    QSqlQuery q;
    q.prepare("select "
              "nikename,"
              "birthday, "
              "deathday, "
              "comment "
              "from table_characterbasic "
              "where char_id = :id;");
    q.bindValue(":id", id);
    q.exec();

    q.next();
    auto nikes = q.value(0).toString().split(",");
    this->nicknames->clear();
    nikes.removeAll(QString());
    QString its;
    foreach (its, nikes) {
        this->nicknames->addItem(its);
        auto item = this->nicknames->item(this->nicknames->count() -1);
        item->setFlags(item->flags() | Qt::ItemIsEditable);
    }

    auto birthdayNum = q.value(1).toLongLong();
    auto deathdayNum = q.value(2).toLongLong();

    this->birthEdit->resetDate(birthdayNum);
    this->birthDay->setText(this->birthEdit->toString());
    this->deathEdit->resetDate(deathdayNum);
    this->deathDay->setText(this->deathEdit->toString());
    this->comment->setText(q.value(3).toString());

    q.prepare("select "
              "enb.ev_node_id, "
              "enb.event_name, "
              "enb.node_name, "
              "enb.begin_time, "
              "enb.end_time "
              "from table_characterlifetracker clt inner join table_eventnodebasic enb "
              "on clt.event_id = enb.ev_node_id "
              "where clt.char_id = :id;");
    q.bindValue(":id", id);
    if(!q.exec()){
        qDebug() << q.lastError();
        return;
    }
    this->storyDisplay->clear();
    while (q.next()) {
        auto name = q.value(1).toString();
        name += ":" + q.value(2).toString();

        auto node(new UIComp::EventSymbo(name, q.value(3).toLongLong(), q.value(4).toLongLong()));
        this->storyDisplay->addEvent(q.value(0).toLongLong(), node);
    }

    this->removeItem->setEnabled(true);
    this->apply->setEnabled(false);
}

void CharacterEdit::slot_editCharcterStory()
{
    auto index = this->table->currentIndex();
    if(!index.isValid())
        return;
    auto id = this->tableModel->oppositeID(index);

    auto dialog(new StoryBoard(id.toLongLong(), this));
    dialog->exec();
    this->apply->setEnabled(true);
}

void CharacterEdit::slot_statusChanged()
{
    this->apply->setEnabled(true);
}






























