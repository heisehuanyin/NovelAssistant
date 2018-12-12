#include "locationedit.h"

#include <QGridLayout>
#include <QLabel>
#include <QSqlQuery>
#include <QtDebug>
#include <QSqlError>

using namespace UIComp;

LocationEdit::LocationEdit(QWidget *parent):
    QDialog (parent),
    locName(new QLineEdit),
    locAdd(new QPushButton(tr("添加地点"))),
    locRemove(new QPushButton(tr("移除地点"))),
    locTable(new QTableView),
    locQueryModel(new Support::HiddenIdModel(this)),
    suffixInput(new QLineEdit),
    xposInput(new QSpinBox),
    yposInput(new QSpinBox),
    zposInput(new QSpinBox),
    storyInput(new QTextEdit),
    nickNames(new QListWidget),
    nickAdd(new QPushButton(tr("添加别名"))),
    nickRemove(new QPushButton(tr("删除别名"))),
    apply(new QPushButton(tr("保存修改")))
{
    this->setWindowTitle("地点编辑");
    auto baseL(new QGridLayout);
    this->setLayout(baseL);

    baseL->addWidget(this->locName, 0, 0, 1, 3);
    this->connect(this->locName, &QLineEdit::textChanged,
                  this,          &LocationEdit::slot_queryLocation);
    baseL->addWidget(this->locAdd, 0, 3);
    this->connect(this->locAdd, &QPushButton::clicked,
                  this,         &LocationEdit::slot_addLocation);
    baseL->addWidget(this->locRemove,0, 4);
    this->connect(this->locRemove, &QPushButton::clicked,
                  this,            &LocationEdit::slot_removeLocation);
    baseL->addWidget(this->apply,0, 5);
    this->connect(this->apply, &QPushButton::clicked,
                  this,            &LocationEdit::slot_changeApply);
    baseL->addWidget(this->locTable, 1, 0, 16, 3);
    this->locTable->setModel(this->locQueryModel);
    this->locTable->setSelectionMode(QAbstractItemView::SingleSelection);
    this->locTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    auto smodel = this->locTable->selectionModel();
    this->connect(smodel, &QItemSelectionModel::selectionChanged,
                  this,   &LocationEdit::slot_responseItemSelection);

    this->locAdd->setEnabled(false);
    this->locRemove->setEnabled(false);
    this->apply->setEnabled(false);

    auto corrdinateGroup(new QGroupBox("坐标位置："));
    baseL->addWidget(corrdinateGroup, 1, 3, 5, 3);
    auto grid2(new QGridLayout);
    corrdinateGroup->setLayout(grid2);
    grid2->addWidget(new QLabel("前缀："), 0, 0);
    grid2->addWidget(new QLabel("Xpos："), 1, 0);
    grid2->addWidget(new QLabel("Ypos："), 2, 0);
    grid2->addWidget(new QLabel("Zpos："), 3, 0);
    grid2->addWidget(this->suffixInput, 0, 1, 1, 4);
    grid2->addWidget(this->xposInput, 1, 1, 1, 4);
    this->xposInput->setMaximum(2100000000);
    this->xposInput->setMinimum(-2100000000);
    grid2->addWidget(this->yposInput, 2, 1, 1, 4);
    this->yposInput->setMaximum(2100000000);
    this->yposInput->setMinimum(-2100000000);
    grid2->addWidget(this->zposInput, 3, 1, 1, 4);
    this->zposInput->setMaximum(2100000000);
    this->zposInput->setMinimum(-2100000000);
    this->connect(this->suffixInput, &QLineEdit::textEdited,
                  this,              &LocationEdit::slot_statusChanged);
    this->connect(this->xposInput, static_cast<void(QSpinBox::*)(const QString &)>(&QSpinBox::valueChanged),
                  this,            &LocationEdit::slot_statusChanged);
    this->connect(this->yposInput, static_cast<void(QSpinBox::*)(const QString &)>(&QSpinBox::valueChanged),
                  this,            &LocationEdit::slot_statusChanged);
    this->connect(this->zposInput, static_cast<void(QSpinBox::*)(const QString &)>(&QSpinBox::valueChanged),
                  this,            &LocationEdit::slot_statusChanged);

    auto storyGroup(new QGroupBox("历史渊源："));
    baseL->addWidget(storyGroup, 6, 3, 5, 3);
    auto grid3(new QGridLayout);
    storyGroup->setLayout(grid3);
    grid3->addWidget(this->storyInput);
    this->connect(this->storyInput, &QTextEdit::textChanged,
                  this,             &LocationEdit::slot_statusChanged);

    auto nickName(new QGroupBox("别名："));
    baseL->addWidget(nickName, 11, 3, 6, 3);
    auto nickPlace(new QGridLayout);
    nickName->setLayout(nickPlace);
    nickPlace->addWidget(this->nickNames, 0, 0, 5, 1);
    this->nickNames->setSelectionMode(QAbstractItemView::SingleSelection);
    nickPlace->addWidget(this->nickAdd, 0, 2);
    nickPlace->addWidget(this->nickRemove, 1, 2);
    this->connect(this->nickAdd, &QPushButton::clicked,
                  this,          &LocationEdit::slot_addNick);
    this->connect(this->nickRemove, &QPushButton::clicked,
                  this,             &LocationEdit::slot_removeNick);
}

LocationEdit::~LocationEdit()
{

    delete locName;
    delete locAdd;
    delete locRemove;
    delete locTable;
    delete locQueryModel;
    delete suffixInput;
    delete xposInput;
    delete yposInput;
    delete zposInput;
    delete storyInput;
    delete nickNames;
    delete nickAdd;
    delete nickRemove;
    delete apply;
}

void LocationEdit::slot_queryLocation(const QString &text)
{

    this->locAdd->setEnabled(false);
    this->locRemove->setEnabled(false);
    this->apply->setEnabled(false);

    if(text == QString()){
        this->locQueryModel->clear();
        return;
    }
    QString execStr = "select "
                      "location_id, "
                      "corrdinate_suffix, "
                      "location_name, "
                      "nickname "
                      "from table_locationlist ";
    if(text != "*" )
        execStr +=    "where location_name like '%" + text + "%' " ;

    execStr += "order by corrdinate_suffix;";

    this->locQueryModel->setQuery(4, execStr);
    this->locQueryModel->setHorizontalHeader(0, "坐标系前缀");
    this->locQueryModel->setHorizontalHeader(1, "地点名称");
    this->locQueryModel->setHorizontalHeader(2, "地名别称");

    if(this->locQueryModel->rowCount(QModelIndex()) == 0){
        this->locAdd->setEnabled(true);
    }
}

void LocationEdit::slot_addLocation()
{
    auto loc_name = this->locName->text();
    QString execStr = "insert into table_locationlist "
                      "(location_name, corrdinate_suffix, xposition, yposition, zposition) "
                      "values('" + loc_name + "', '无', 0, 0, 0);";
    QSqlQuery q;
    if(!q.exec(execStr))
        qDebug() << q.lastError();

    this->slot_queryLocation(loc_name);
}

void LocationEdit::slot_removeLocation()
{
    auto index = this->locTable->currentIndex();
    auto id = this->locQueryModel->oppositeID(index);
    auto name=this->locQueryModel->data(index.sibling(index.row(), 1), Qt::DisplayRole).toString();
    QString exeStr = "delete "
                     "from table_locationlist "
                     "where location_id = " +id.toString() + ";";
    QSqlQuery q;
    if(!q.exec(exeStr))
        qDebug() << q.lastError();

    this->locName->setText("");
    this->locName->setText(name);
}

void LocationEdit::slot_statusChanged()
{
    this->apply->setEnabled(true);
}

void LocationEdit::slot_changeApply()
{
    auto index = this->locTable->currentIndex();
    auto id = this->locQueryModel->oppositeID(index);
    auto name=this->locQueryModel->data(index.sibling(index.row(), 1), Qt::DisplayRole).toString();

    QSqlQuery p;
    p.prepare("update table_locationlist "
              "set corrdinate_suffix = :suffix, "
              " xposition = :xpos, "
              " yposition = :ypos, "
              " zposition = :zpos, "
              " nickname  = :nicks, "
              " story = :story "
              "where location_id = :id;");

    p.bindValue(":suffix", this->suffixInput->text());
    p.bindValue(":xpos",   this->xposInput->value());
    p.bindValue(":ypos",   this->yposInput->value());
    p.bindValue(":zpos",   this->zposInput->value());
    QString strx;
    for(int i=0; i<this->nickNames->count(); ++i){
        auto x = this->nickNames->item(i);
        strx += x->text() + ",";
    }

    p.bindValue(":nicks",  strx);
    p.bindValue(":story",  this->storyInput->toPlainText());
    p.bindValue(":id",     id);

    if(!p.exec())
        qDebug() << p.lastError();

    this->locName->setText("");
    this->locName->setText(name);
}

void LocationEdit::slot_responseItemSelection(const QItemSelection &selected, const QItemSelection &)
{
    this->locRemove->setEnabled(true);
    auto index = selected.indexes().at(0);
    auto item = this->locQueryModel->oppositeID(index);

    QString exeStr = "select "
                     "corrdinate_suffix, "
                     "xposition, "
                     "yposition, "
                     "zposition, "
                     "story, "
                     "nickname "
                     "from table_locationlist "
                     "where location_id = "+item.toString()+";";
    QSqlQuery q;
    if(!q.exec(exeStr))
        qDebug() << q.lastError();
    q.next();
    this->suffixInput->setText(q.value(0).toString());
    this->xposInput->setValue(q.value(1).toInt());
    this->yposInput->setValue(q.value(2).toInt());
    this->zposInput->setValue(q.value(3).toInt());

    this->storyInput->setText(q.value(4).toString());
    auto nicks = q.value(5).toString().split(",");
    this->nickNames->clear();
    QString x;
    foreach (x, nicks) {
        if(x == QString())
            continue;
        this->nickNames->addItem(x);
        auto item = this->nickNames->item(nickNames->count()-1);
        item->setFlags(item->flags()|Qt::ItemIsEditable);
    }
    this->apply->setEnabled(false);
}

void LocationEdit::slot_addNick()
{
    this->nickNames->addItem("双击修改");
    auto item = this->nickNames->item(nickNames->count()-1);
    item->setFlags(item->flags()|Qt::ItemIsEditable);
}

void LocationEdit::slot_removeNick()
{
    auto row = this->nickNames->currentRow();

    auto x = this->nickNames->takeItem(row);
    if(!x)
        return;
    delete x;
}































